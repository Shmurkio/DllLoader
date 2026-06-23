#include <Dll/Runtime/CxxRuntime.hpp>

#define LogCall() \
    OnFuncCall(__func__)

namespace Dll::Runtime
{
	Event<StringView> OnFuncCall{};

    alignas(8) Foundation::Byte StdNoThrowObject[1]{};

    extern "C" auto Terminate() -> Foundation::Void
    {
        LogCall();

        for (;;)
        {
            __debugbreak();
        }
    }

    extern "C" auto CxxNew(Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        return malloc(Size);
    }

    extern "C" auto CxxDelete(Foundation::Void* Pointer) -> Foundation::Void
    {
        LogCall();
        free(Pointer);
    }

    extern "C" auto CxxNewArray(Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        return malloc(Size);
    }

    extern "C" auto CxxDeleteArray(Foundation::Void* Pointer) -> Foundation::Void
    {
        LogCall();
        free(Pointer);
    }

    extern "C" auto CxxNewNoThrow(Foundation::Size Size, const Foundation::Void*) -> Foundation::Void*
    {
        LogCall();
        return malloc(Size);
    }

    extern "C" auto CxxNewArrayNoThrow(Foundation::Size Size, const Foundation::Void*) -> Foundation::Void*
    {
        LogCall();
        return malloc(Size);
    }

    extern "C" auto CxxDeleteSized(Foundation::Void* Pointer, Foundation::Size) -> Foundation::Void
    {
        LogCall();
        free(Pointer);
    }

    extern "C" auto CxxDeleteArraySized(Foundation::Void* Pointer, Foundation::Size) -> Foundation::Void
    {
        LogCall();
        free(Pointer);
    }
}