#pragma once

#include <UEFIpp/UEFIpp.hpp>

extern "C" auto CxxThrowExceptionShim(Foundation::Void* ExceptionObject, Foundation::Void* RawThrowInfo) -> Foundation::Void;

namespace Dll::Runtime
{
    auto SetCurrentImageBase(Foundation::Void* Base) -> Foundation::Void;
    auto CurrentImageBase() -> Foundation::Uint8*;

    extern alignas(8) Foundation::Byte TypeInfoVftable[8];

    extern "C"
    {
        auto StdExceptionDestroy(Foundation::Void* Exception) -> Foundation::Void;
        auto StdExceptionCopy(Foundation::Void* Destination, const Foundation::Void* Source) -> Foundation::Void;

        extern "C" auto CxxThrowExceptionImpl(Foundation::Void* ExceptionObject, Foundation::Void* RawThrowInfo, Foundation::Uint64 ThrowRip, Foundation::Uint64 FunctionRsp) -> Foundation::Void;

        auto CxxFrameHandler4(Foundation::Void* ExceptionRecord, Foundation::Void* EstablisherFrame, Foundation::Void* ContextRecord, Foundation::Void* DispatcherContext) -> Foundation::Int32;
    }

    auto StdLengthError(const Foundation::Char* Message) -> Foundation::Void;
    auto StdBadFunctionCall() -> Foundation::Void;
    auto StdTypeInfoCompare(const Foundation::Void* Left, const Foundation::Void* Right) -> Foundation::Int32;
}