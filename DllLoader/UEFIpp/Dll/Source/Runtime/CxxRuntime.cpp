#include <Dll/Runtime/CxxRuntime.hpp>

#define LogCall() OnFuncCall(__func__)
#define Unhandled(Reason) OnUnhandledCall({ __func__, Reason })

namespace Dll::Runtime
{
    alignas(8) Foundation::Byte StdNoThrowObject[1]{};

    extern "C" auto Terminate() -> Foundation::Void
    {
        LogCall();

        Stream::Out::Serial
            << "Terminate called" << Stream::Endl;

        for (;;)
        {
            UEFI::Context::BootServices().Stall(1000000);
        }
    }

    extern "C" auto CxxNew(Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        return Memory::Allocator::AllocatePool(Size);
    }

    extern "C" auto CxxDelete(Foundation::Void* Pointer) -> Foundation::Void
    {
        LogCall();

        if (Pointer)
        {
            Memory::Allocator::FreePool(Pointer);
        }
    }

    extern "C" auto CxxNewArray(Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        return CxxNew(Size);
    }

    extern "C" auto CxxDeleteArray(Foundation::Void* Pointer) -> Foundation::Void
    {
        LogCall();
        CxxDelete(Pointer);
    }

    extern "C" auto CxxNewNoThrow(Foundation::Size Size, const Foundation::Void*) -> Foundation::Void*
    {
        LogCall();
        return CxxNew(Size);
    }

    extern "C" auto CxxNewArrayNoThrow(Foundation::Size Size, const Foundation::Void*) -> Foundation::Void*
    {
        LogCall();
        return CxxNewArray(Size);
    }

    extern "C" auto CxxDeleteSized(Foundation::Void* Pointer, Foundation::Size) -> Foundation::Void
    {
        LogCall();
        CxxDelete(Pointer);
    }

    extern "C" auto CxxDeleteArraySized(Foundation::Void* Pointer, Foundation::Size) -> Foundation::Void
    {
        LogCall();
        CxxDeleteArray(Pointer);
    }
}