#pragma once

#include <UEFIpp/UEFIpp.hpp>

namespace Dll::Runtime
{
    extern alignas(8) Foundation::Byte TypeInfoVftable[8];

    extern "C"
    {
        auto __std_exception_destroy(Foundation::Void*) -> Foundation::Void;
        auto __std_exception_copy(Foundation::Void*, const Foundation::Void*) -> Foundation::Void;
        auto CxxThrowException(Foundation::Void*, Foundation::Void*) -> Foundation::Void;
        extern int Fltused;
        auto RuntimeCeilf(Foundation::Float Value) -> Foundation::Float;
    }

    auto StdLengthError(const Foundation::Char*) -> Foundation::Void;
    auto StdBadFunctionCall() -> Foundation::Void;
	auto StdTypeInfoCompare(const Foundation::Void* Left, const Foundation::Void* Right) -> Foundation::Int32;
}