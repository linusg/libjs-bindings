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

#include "GlobalObject.h"

#include <LibJS/Runtime/Array.h>
#include <LibJS/Runtime/GlobalObject.h>

namespace Bindings {

void GlobalObject::initialize()
{
    JS::GlobalObject::initialize();

    define_native_property("argc", argc_getter, nullptr, 0);
    define_native_property("argv", argv_getter, nullptr, 0);

    u8 attr = JS::Attribute::Writable | JS::Attribute::Configurable;
    auto* core_object = JS::Object::create_empty(interpreter(), interpreter().global_object());
    auto* gui_object = JS::Object::create_empty(interpreter(), interpreter().global_object());
    define_property("Core", core_object, attr);
    define_property("GUI", gui_object, attr);

#define __JS_BINDINGS_ENUMERATE(Namespace, ObjectName, object_name, ClassName, class_name, PrototypeName, ConstructorName)                                                           \
    m_##object_name##_##class_name##_prototype = heap().allocate<Namespace::PrototypeName>();                                                                                        \
    m_##object_name##_##class_name##_constructor = heap().allocate<Namespace::ConstructorName>();                                                                                    \
    m_##object_name##_##class_name##_constructor->define_property("prototype", m_##object_name##_##class_name##_prototype, 0);                                                       \
    m_##object_name##_##class_name##_constructor->define_property("name", js_string(heap(), #ClassName), JS::Attribute::Configurable);                                               \
    m_##object_name##_##class_name##_prototype->define_property("constructor", m_##object_name##_##class_name##_constructor, JS::Attribute::Writable | JS::Attribute::Configurable); \
    object_name##_object->define_property(#ClassName, m_##object_name##_##class_name##_constructor, JS::Attribute::Writable | JS::Attribute::Configurable);
    JS_BINDINGS_ENUMERATE_NATIVE_OBJECTS
#undef __JS_BINDINGS_ENUMERATE
}

void GlobalObject::visit_children(Visitor& visitor)
{
    JS::GlobalObject::visit_children(visitor);

#define __JS_BINDINGS_ENUMERATE(Namespace, ObjectName, object_name, ClassName, class_name, PrototypeName, ConstructorName) \
    visitor.visit(m_##object_name##_##class_name##_constructor);                                                           \
    visitor.visit(m_##object_name##_##class_name##_prototype);
    JS_BINDINGS_ENUMERATE_NATIVE_OBJECTS
#undef __JS_BINDINGS_ENUMERATE
}

JS::Value GlobalObject::argc_getter(JS::Interpreter& interpreter)
{
    auto& global_object = static_cast<GlobalObject&>(interpreter.global_object());
    return JS::Value(global_object.argc());
}

JS::Value GlobalObject::argv_getter(JS::Interpreter& interpreter)
{
    auto& global_object = static_cast<GlobalObject&>(interpreter.global_object());
    auto* argv_array = JS::Array::create(global_object);
    for (int i = 0; i < global_object.argc(); ++i)
        argv_array->indexed_properties().append(js_string(interpreter, String(global_object.argv()[i])));
    return argv_array;
}

}
