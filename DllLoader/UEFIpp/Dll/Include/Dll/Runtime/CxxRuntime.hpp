#pragma once

#include <UEFIpp/UEFIpp.hpp>

namespace Dll::Runtime
{
    extern Event<StringView> OnFuncCall;

    extern alignas(8) Foundation::Byte StdNoThrowObject[1];

    extern "C"
    {
        auto Terminate() -> Foundation::Void;

        auto CxxNew(Foundation::Size Size) -> Foundation::Void*;
        auto CxxDelete(Foundation::Void* Pointer) -> Foundation::Void;

        auto CxxNewArray(Foundation::Size Size) -> Foundation::Void*;
        auto CxxDeleteArray(Foundation::Void* Pointer) -> Foundation::Void;

        auto CxxNewNoThrow(Foundation::Size Size, const Foundation::Void* NoThrow) -> Foundation::Void*;
        auto CxxNewArrayNoThrow(Foundation::Size Size, const Foundation::Void* NoThrow) -> Foundation::Void*;

        auto CxxDeleteSized(Foundation::Void* Pointer, Foundation::Size Size) -> Foundation::Void;
        auto CxxDeleteArraySized(Foundation::Void* Pointer, Foundation::Size Size) -> Foundation::Void;
    }
}