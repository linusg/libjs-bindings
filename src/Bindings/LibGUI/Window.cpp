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

#include "Window.h"
#include "../GlobalObject.h"

namespace Bindings {
namespace LibGUI {

// BEGIN_OBJECT
Window* Window::create(JS::GlobalObject& global_object)
{
    return global_object.heap().allocate<Window>(*static_cast<GlobalObject&>(global_object).gui_window_prototype());
}

Window::Window(JS::Object& prototype)
    : Object(&prototype)
    , m_window(GUI::Window::construct())
{
    set_prototype(&prototype);
}
// END_OBJECT

// BEGIN_CONSTRUCTOR
WindowConstructor::WindowConstructor()
    : JS::NativeFunction(*interpreter().global_object().function_prototype())
{
    define_property("length", JS::Value(0), JS::Attribute::Configurable);
}

JS::Value WindowConstructor::call(JS::Interpreter& interpreter)
{
    return construct(interpreter);
}

JS::Value WindowConstructor::construct(JS::Interpreter& interpreter)
{
    return Window::create(interpreter.global_object());
}
// END_CONSTRUCTOR

// BEGIN_PROTOTYPE
WindowPrototype::WindowPrototype()
    : Object(interpreter().global_object().object_prototype())
{
    u8 attr = JS::Attribute::Writable | JS::Attribute::Configurable;
    define_native_function("show", show, 0, attr);

    define_native_property("title", title_getter, title_setter, 0);
}

static GUI::Window* window_from(JS::Interpreter& interpreter)
{
    auto* this_object = interpreter.this_value().to_object(interpreter);
    if (!this_object)
        return nullptr;
    if (StringView("Window") != this_object->class_name()) {
        interpreter.throw_exception<JS::TypeError>("Not a Window object");
        return nullptr;
    }
    return static_cast<Window*>(this_object)->window();
}

JS::Value WindowPrototype::show(JS::Interpreter& interpreter)
{
    auto* window = window_from(interpreter);
    if (!window)
        return {};
    window->show();
    return JS::js_undefined();
}

JS::Value WindowPrototype::title_getter(JS::Interpreter& interpreter)
{
    auto* window = window_from(interpreter);
    if (!window)
        return {};
    return JS::js_string(interpreter, window->title());
}

void WindowPrototype::title_setter(JS::Interpreter& interpreter, JS::Value value)
{
    auto* window = window_from(interpreter);
    if (!window)
        return;
    auto title = value.to_string(interpreter);
    if (interpreter.exception())
        return;
    window->set_title(title);
}
// END_PROTOTYPE

}
}
