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

#include "../Macros.h"

#include <LibCore/File.h>
#include <LibJS/Runtime/NativeFunction.h>

namespace Bindings {
namespace LibCore {

// clang-format off
OBJECT_WITH_ARGS(File, const StringView& filename)
    MEMBER(NonnullRefPtr<Core::File>, file)
END

CONSTRUCTOR(File)
    JS_DECLARE_NATIVE_FUNCTION(exists);
    JS_DECLARE_NATIVE_FUNCTION(is_directory);
END

PROTOTYPE(File)
    JS_DECLARE_NATIVE_FUNCTION(is_directory);
    JS_DECLARE_NATIVE_FUNCTION(open);
    JS_DECLARE_NATIVE_FUNCTION(close);
    JS_DECLARE_NATIVE_FUNCTION(read);
    JS_DECLARE_NATIVE_FUNCTION(read_line);
    JS_DECLARE_NATIVE_FUNCTION(read_all);
    JS_DECLARE_NATIVE_FUNCTION(write);
    JS_DECLARE_NATIVE_FUNCTION(seek);
    JS_DECLARE_NATIVE_GETTER(can_read_getter);
    JS_DECLARE_NATIVE_GETTER(can_read_line_getter);
    JS_DECLARE_NATIVE_GETTER(mode_getter);
    JS_DECLARE_NATIVE_GETTER(is_open_getter);
    JS_DECLARE_NATIVE_GETTER(eof_getter);
    JS_DECLARE_NATIVE_GETTER(has_error_getter);
    JS_DECLARE_NATIVE_GETTER(error_getter);
    JS_DECLARE_NATIVE_GETTER(error_string_getter);
    JS_DECLARE_NATIVE_GETTER(filename_getter);
    JS_DECLARE_NATIVE_SETTER(filename_setter);
END
// clang-format on

}
}
