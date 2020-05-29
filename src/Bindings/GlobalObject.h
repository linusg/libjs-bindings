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

#pragma once

#include "../Forward.h"
#include "LibCore/File.h"
#include "LibGUI/Notification.h"

#include <LibJS/Interpreter.h>
#include <LibJS/Runtime/Error.h>
#include <LibJS/Runtime/GlobalObject.h>

namespace Bindings {

class GlobalObject : public JS::GlobalObject {
public:
    GlobalObject() {};
    virtual ~GlobalObject() override {};
    virtual void initialize() override;

    int argc() const { return m_argc; };
    char** argv() const { return m_argv; };
    void set_argc(int argc) { m_argc = argc; }
    void set_argv(char** argv) { m_argv = argv; }

#define __JS_BINDINGS_ENUMERATE(Namespace, ObjectName, object_name, ClassName, class_name, PrototypeName, ConstructorName)           \
    Namespace::ConstructorName* object_name##_##class_name##_constructor() { return m_##object_name##_##class_name##_constructor; }; \
    Namespace::PrototypeName* object_name##_##class_name##_prototype() { return m_##object_name##_##class_name##_prototype; };
    JS_BINDINGS_ENUMERATE_NATIVE_OBJECTS
#undef __JS_BINDINGS_ENUMERATE

private:
    virtual const char* class_name() const override { return "GlobalObject"; }
    virtual void visit_children(Visitor&) override;

    static JS::Value argc_getter(JS::Interpreter&);
    static JS::Value argv_getter(JS::Interpreter&);

    int m_argc { 0 };
    char** m_argv {};

#define __JS_BINDINGS_ENUMERATE(Namespace, ObjectName, object_name, ClassName, class_name, PrototypeName, ConstructorName) \
    Namespace::ConstructorName* m_##object_name##_##class_name##_constructor { nullptr };                                  \
    Namespace::PrototypeName* m_##object_name##_##class_name##_prototype { nullptr };
    JS_BINDINGS_ENUMERATE_NATIVE_OBJECTS
#undef __JS_BINDINGS_ENUMERATE
};

}
