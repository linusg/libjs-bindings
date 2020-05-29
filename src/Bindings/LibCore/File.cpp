/*
 * Copyright (c) 2020, Linus Groh <mail@linusgroh.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "File.h"
#include "../GlobalObject.h"

#include <AK/ByteBuffer.h>

#define FILE_OPEN_MODES                                \
    __ENUMERATE_FILE_OPEN_MODES(NOT_OPEN, NotOpen)     \
    __ENUMERATE_FILE_OPEN_MODES(READ_ONLY, ReadOnly)   \
    __ENUMERATE_FILE_OPEN_MODES(WRITE_ONLY, WriteOnly) \
    __ENUMERATE_FILE_OPEN_MODES(READ_WRITE, ReadWrite) \
    __ENUMERATE_FILE_OPEN_MODES(APPEND, Append)        \
    __ENUMERATE_FILE_OPEN_MODES(TRUNCATE, Truncate)    \
    __ENUMERATE_FILE_OPEN_MODES(MUST_BE_NEW, MustBeNew)

#define FILE_SEEK_MODES                                                     \
    __ENUMERATE_FILE_SEEK_MODES(SET_POSITION, SetPosition)                  \
    __ENUMERATE_FILE_SEEK_MODES(FROM_CURRENT_POSITION, FromCurrentPosition) \
    __ENUMERATE_FILE_SEEK_MODES(FROM_END_POSITION, FromEndPosition)

namespace Bindings {
namespace LibCore {

// BEGIN_OBJECT
File* File::create(JS::GlobalObject& global_object, const StringView& filename)
{
    return global_object.heap().allocate<File>(*static_cast<GlobalObject&>(global_object).core_file_prototype(), filename);
}

File::File(JS::Object& prototype, const StringView& filename)
    : Object(&prototype)
    , m_file(Core::File::construct(filename))
{
    set_prototype(&prototype);
}
// END_OBJECT

// BEGIN_CONSTRUCTOR
FileConstructor::FileConstructor()
    : JS::NativeFunction(*interpreter().global_object().function_prototype())
{
    define_property("length", JS::Value(1), JS::Attribute::Configurable);

    u8 attr = JS::Attribute::Writable | JS::Attribute::Configurable;
    define_native_function("exists", exists, 1, attr);
    define_native_function("isDirectory", is_directory, 1, attr);

    auto* open_mode = JS::Object::create_empty(interpreter(), interpreter().global_object());
    define_property("OpenMode", open_mode, JS::Attribute::Enumerable);
#define __ENUMERATE_FILE_OPEN_MODES(key, value) \
    open_mode->define_property(#key, JS::Value(Core::File::OpenMode::value), JS::Attribute::Enumerable);
    FILE_OPEN_MODES
#undef __ENUMERATE_FILE_OPEN_MODES

    auto* seek_mode = JS::Object::create_empty(interpreter(), interpreter().global_object());
    define_property("SeekMode", seek_mode, JS::Attribute::Enumerable);
#define __ENUMERATE_FILE_SEEK_MODES(key, value) \
    seek_mode->define_property(#key, JS::Value((i32)Core::IODevice::SeekMode::value), JS::Attribute::Enumerable);
    FILE_SEEK_MODES
#undef __ENUMERATE_FILE_SEEK_MODES
}

JS::Value FileConstructor::call(JS::Interpreter& interpreter)
{
    return construct(interpreter);
}

JS::Value FileConstructor::construct(JS::Interpreter& interpreter)
{
    auto filename = interpreter.argument(0).to_string(interpreter);
    if (interpreter.exception())
        return {};
    return File::create(interpreter.global_object(), filename);
}

JS::Value FileConstructor::exists(JS::Interpreter& interpreter)
{
    auto filename = interpreter.argument(0).to_string(interpreter);
    if (interpreter.exception())
        return {};
    return JS::Value(Core::File::exists(filename));
}

JS::Value FileConstructor::is_directory(JS::Interpreter& interpreter)
{
    auto filename = interpreter.argument(0).to_string(interpreter);
    if (interpreter.exception())
        return {};
    return JS::Value(Core::File::is_directory(filename));
}
// END_CONSTRUCTOR

// BEGIN_PROTOTYPE
FilePrototype::FilePrototype()
    : Object(interpreter().global_object().object_prototype())
{
    u8 attr = JS::Attribute::Writable | JS::Attribute::Configurable;
    define_native_function("isDirectory", is_directory, 0, attr);
    define_native_function("open", open, 1, attr);
    define_native_function("close", close, 0, attr);
    define_native_function("read", read, 1, attr);
    define_native_function("readLine", read_line, 1, attr);
    define_native_function("readAll", read_all, 0, attr);
    define_native_function("write", write, 1, attr);

    define_native_property("canRead", can_read_getter, nullptr, 0);
    define_native_property("canReadLine", can_read_line_getter, nullptr, 0);
    define_native_property("mode", mode_getter, nullptr, 0);
    define_native_property("isOpen", is_open_getter, nullptr, 0);
    define_native_property("eof", eof_getter, nullptr, 0);
    define_native_property("hasError", has_error_getter, nullptr, 0);
    define_native_property("error", error_getter, nullptr, 0);
    define_native_property("errorString", error_string_getter, nullptr, 0);
    define_native_property("filename", filename_getter, filename_setter, JS::Attribute::Writable);
}

static bool is_valid_open_mode(i32 open_mode)
{
    return
#define __ENUMERATE_FILE_OPEN_MODES(key, value) \
    open_mode == Core::File::OpenMode::value ||
        FILE_OPEN_MODES
#undef __ENUMERATE_FILE_OPEN_MODES
        false;
}

static bool is_valid_seek_mode(i32 seek_mode)
{
    return
#define __ENUMERATE_FILE_SEEK_MODES(key, value) \
    seek_mode == (i32)Core::IODevice::SeekMode::value ||
        FILE_SEEK_MODES
#undef __ENUMERATE_FILE_SEEK_MODES
        false;
}

static Core::File* file_from(JS::Interpreter& interpreter)
{
    auto* this_object = interpreter.this_value().to_object(interpreter);
    if (!this_object)
        return nullptr;
    if (StringView("File") != this_object->class_name()) {
        interpreter.throw_exception<JS::TypeError>("Not a File object");
        return nullptr;
    }
    return static_cast<File*>(this_object)->file();
}

JS::Value FilePrototype::is_directory(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    return JS::Value(file->is_directory());
}

JS::Value FilePrototype::open(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    auto open_mode = interpreter.argument(0).to_i32(interpreter);
    if (interpreter.exception())
        return {};
    if (!is_valid_open_mode(open_mode)) {
        interpreter.throw_exception<JS::TypeError>("Invalid open mode");
        return {};
    }
    return JS::Value(file->open(static_cast<Core::File::OpenMode>(open_mode)));
}

JS::Value FilePrototype::close(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    return JS::Value(file->close());
}

JS::Value FilePrototype::read(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    auto max_size = interpreter.argument(0).to_size_t(interpreter);
    if (interpreter.exception())
        return {};
    auto byte_buffer = file->read(max_size);
    return JS::js_string(interpreter, byte_buffer.is_empty() ? "" : String::copy(byte_buffer));
}

JS::Value FilePrototype::read_line(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    auto max_size = interpreter.argument(0).to_size_t(interpreter);
    if (interpreter.exception())
        return {};
    auto byte_buffer = file->read_line(max_size);
    return JS::js_string(interpreter, byte_buffer.is_empty() ? "" : String::copy(byte_buffer));
}

JS::Value FilePrototype::write(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    auto text = interpreter.argument(0).to_string(interpreter);
    if (interpreter.exception())
        return {};
    return JS::Value(file->write(text));
}

JS::Value FilePrototype::read_all(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    auto byte_buffer = file->read_all();
    return JS::js_string(interpreter, byte_buffer.is_empty() ? "" : String::copy(byte_buffer));
}

JS::Value FilePrototype::can_read_getter(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    return JS::Value(file->can_read());
}

JS::Value FilePrototype::can_read_line_getter(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    return JS::Value(file->can_read_line());
}

JS::Value FilePrototype::mode_getter(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    return JS::Value(file->mode());
}

JS::Value FilePrototype::is_open_getter(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    return JS::Value(file->is_open());
}

JS::Value FilePrototype::eof_getter(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    return JS::Value(file->eof());
}

JS::Value FilePrototype::has_error_getter(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    return JS::Value(file->has_error());
}

JS::Value FilePrototype::error_getter(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    return JS::Value(file->error());
}

JS::Value FilePrototype::error_string_getter(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    return JS::js_string(interpreter, String(file->error_string()));
}

JS::Value FilePrototype::filename_getter(JS::Interpreter& interpreter)
{
    auto* file = file_from(interpreter);
    if (!file)
        return {};
    return JS::js_string(interpreter, file->filename());
}

void FilePrototype::filename_setter(JS::Interpreter& interpreter, JS::Value value)
{
    auto* file = file_from(interpreter);
    if (!file)
        return;
    auto filename = value.to_string(interpreter);
    if (interpreter.exception())
        return;
    file->set_filename(filename);
}
// END_PROTOTYPE

}
}
