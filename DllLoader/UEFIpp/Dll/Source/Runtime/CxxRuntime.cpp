#include <Dll/Runtime/CxxRuntime.hpp>
#include <Dll/Runtime/Trace.hpp>

namespace Dll::Runtime
{
    alignas(8) Foundation::Byte StdNoThrowObject[1]{};

    extern "C" auto Terminate() -> Foundation::Void
    {
        DLL_CXX_TRACE(__func__);
        DLL_TRACE_FATAL(::Dll::Runtime::TraceCategory::CXX, "Terminate called");

        for (;;)
        {
            UEFI::Context::BootServices().Stall(1000000);
        }
    }

    extern "C" auto CxxNew(Foundation::Size Size) -> Foundation::Void*
    {
        DLL_CXX_TRACE(__func__);
        return Memory::Allocator::AllocatePool(Size);
    }

    extern "C" auto CxxDelete(Foundation::Void* Pointer) -> Foundation::Void
    {
        DLL_CXX_TRACE(__func__);

        if (Pointer)
        {
            Memory::Allocator::FreePool(Pointer);
        }
    }

    extern "C" auto CxxNewArray(Foundation::Size Size) -> Foundation::Void*
    {
        DLL_CXX_TRACE(__func__);
        return CxxNew(Size);
    }

    extern "C" auto CxxDeleteArray(Foundation::Void* Pointer) -> Foundation::Void
    {
        DLL_CXX_TRACE(__func__);
        CxxDelete(Pointer);
    }

    extern "C" auto CxxNewNoThrow(Foundation::Size Size, const Foundation::Void*) -> Foundation::Void*
    {
        DLL_CXX_TRACE(__func__);
        return CxxNew(Size);
    }

    extern "C" auto CxxNewArrayNoThrow(Foundation::Size Size, const Foundation::Void*) -> Foundation::Void*
    {
        DLL_CXX_TRACE(__func__);
        return CxxNewArray(Size);
    }

    extern "C" auto CxxDeleteSized(Foundation::Void* Pointer, Foundation::Size) -> Foundation::Void
    {
        DLL_CXX_TRACE(__func__);
        CxxDelete(Pointer);
    }

    extern "C" auto CxxDeleteArraySized(Foundation::Void* Pointer, Foundation::Size) -> Foundation::Void
    {
        DLL_CXX_TRACE(__func__);
        CxxDeleteArray(Pointer);
    }
}