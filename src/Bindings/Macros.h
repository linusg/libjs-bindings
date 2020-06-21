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

#define END \
    }       \
    ;

#define OBJECT(Name)                            \
    class Name final : public JS::Object {      \
                                                \
    public:                                     \
        static Name* create(JS::GlobalObject&); \
                                                \
        Name(JS::Object&);                      \
        virtual ~Name() override {};            \
                                                \
    private:                                    \
        virtual const char* class_name() const override { return #Name; }

#define OBJECT_WITH_ARGS(Name, args...)               \
    class Name final : public JS::Object {            \
                                                      \
    public:                                           \
        static Name* create(JS::GlobalObject&, args); \
                                                      \
        Name(JS::Object&, args);                      \
        virtual ~Name() override {};                  \
                                                      \
    private:                                          \
        virtual const char* class_name() const override { return #Name; }

#define MEMBER(T, name)                 \
public:                                 \
    T name() const { return m_##name; } \
                                        \
private:                                \
    T m_##name;

#define CONSTRUCTOR(Name)                                                      \
    class Name##Constructor final : public JS::NativeFunction {                \
        JS_OBJECT(Name##Constructor, JS::NativeFunction);                      \
                                                                               \
    public:                                                                    \
        Name##Constructor(JS::GlobalObject&);                                  \
        virtual void initialize(JS::Interpreter&, JS::GlobalObject&) override; \
        virtual ~Name##Constructor() override {};                              \
                                                                               \
        virtual JS::Value call(JS::Interpreter&) override;                     \
        virtual JS::Value construct(JS::Interpreter&) override;                \
                                                                               \
    private:                                                                   \
        virtual bool has_constructor() const override { return true; }

#define PROTOTYPE(Name)                                                        \
    class Name##Prototype final : public JS::Object {                          \
        JS_OBJECT(Name##Prototype, JS::Object);                                \
                                                                               \
    public:                                                                    \
        Name##Prototype(JS::GlobalObject&);                                    \
        virtual void initialize(JS::Interpreter&, JS::GlobalObject&) override; \
        virtual ~Name##Prototype() override {};                                \
                                                                               \
    private:

#define SIMPLE_GETTER(Prototype, object, name)                                                        \
    JS::Value Prototype::name##_getter(JS::Interpreter& interpreter, JS::GlobalObject& global_object) \
    {                                                                                                 \
        auto* object = object##_from(interpreter, global_object);                                     \
        if (!object)                                                                                  \
            return {};                                                                                \
        return JS::Value(object->name());                                                             \
    }

#define SIMPLE_STRING_GETTER(Prototype, object, name)                                                 \
    JS::Value Prototype::name##_getter(JS::Interpreter& interpreter, JS::GlobalObject& global_object) \
    {                                                                                                 \
        auto* object = object##_from(interpreter, global_object);                                     \
        if (!object)                                                                                  \
            return {};                                                                                \
        return JS::js_string(interpreter, object->name());                                            \
    }

#define _SIMPLE_SETTER(Prototype, object, name, type)                                                             \
    void Prototype::name##_setter(JS::Interpreter& interpreter, JS::GlobalObject& global_object, JS::Value value) \
    {                                                                                                             \
        auto* object = object##_from(interpreter, global_object);                                                 \
        if (!object)                                                                                              \
            return;                                                                                               \
        auto name = value.to_##type(interpreter);                                                                 \
        if (interpreter.exception())                                                                              \
            return;                                                                                               \
        object->set_##name(name);                                                                                 \
    }

#define SIMPLE_STRING_SETTER(Prototype, object, name) \
    _SIMPLE_SETTER(Prototype, object, name, string)

#define THIS_OBJECT_FROM_INTERPRETER(T, Name, name)                                                      \
    static T* name##_from(JS::Interpreter& interpreter, JS::GlobalObject& global_object)                 \
    {                                                                                                    \
        auto* this_object = interpreter.this_value(global_object).to_object(interpreter, global_object); \
        if (!this_object)                                                                                \
            return nullptr;                                                                              \
        if (StringView(#Name) != this_object->class_name()) {                                            \
            interpreter.throw_exception<JS::TypeError>("Not a " #Name " object");                        \
            return nullptr;                                                                              \
        }                                                                                                \
        return static_cast<Name*>(this_object)->name();                                                  \
    }
