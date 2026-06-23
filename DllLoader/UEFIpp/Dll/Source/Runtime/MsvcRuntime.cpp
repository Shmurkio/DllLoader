#include <Dll/Runtime/MsvcRuntime.hpp>
#include <Dll/Runtime/CxxRuntime.hpp>

#define LogCall() \
    OnFuncCall(__func__)

namespace Dll::Runtime
{
    alignas(8) Foundation::Byte TypeInfoVftable[8]{};

    extern "C" auto __std_exception_destroy(Foundation::Void*) -> Foundation::Void
    {
        LogCall();
    }

    extern "C" auto __std_exception_copy(Foundation::Void*, const Foundation::Void*) -> Foundation::Void
    {
        LogCall();
    }

    extern "C" auto CxxThrowException(Foundation::Void*, Foundation::Void*) -> Foundation::Void
    {
        LogCall();
        Terminate();
    }

    auto StdLengthError(const Foundation::Char*) -> Foundation::Void
    {
        LogCall();
        Terminate();
    }

    auto StdBadFunctionCall() -> Foundation::Void
    {
        LogCall();
        Terminate();
    }

	extern "C" int Fltused = 1;

	extern "C" auto RuntimeCeilf(Foundation::Float Value) -> Foundation::Float
	{
		LogCall();
        const auto Integer = static_cast<int>(Value);

        if (static_cast<float>(Integer) == Value)
        {
            return Value;
        }

        if (Value > 0.0f)
        {
            return static_cast<float>(Integer + 1);
        }

        return static_cast<float>(Integer);
	}

    auto StdTypeInfoCompare(const Foundation::Void* Left, const Foundation::Void* Right) -> Foundation::Int32
    {
		LogCall();

        if (Left < Right)
        {
            return -1;
        }

        if (Left > Right)
        {
            return 1;
        }

        return 0;
    }
}