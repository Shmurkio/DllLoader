#include <Dll/Runtime/CrtRuntime.hpp>
#include <Dll/Runtime/HostRuntime.hpp>
#include <cstdarg>

#define LogCall() OnFuncCall(__func__)
#define Unhandled(Reason) OnUnhandledCall({ __func__, Reason })

namespace Dll::Runtime
{
    namespace
    {
        struct RuntimeFile
        {
            Foundation::Int32 Index{};
        };

        RuntimeFile RuntimeFiles[3]
        {
            { 0 }, // stdin
            { 1 }, // stdout
            { 2 }  // stderr
        };
    }

    Event<StringView> OnFuncCall{};
    Event<UnhandledCallInfo> OnUnhandledCall{};

    extern "C" Foundation::Int32 Fltused = 1;

    extern "C" auto RuntimeMalloc(Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        return Memory::Allocator::AllocatePool(Size);
    }

    extern "C" auto RuntimeFree(Foundation::Void* Pointer) -> Foundation::Void
    {
        LogCall();
        if (Pointer)
        {
            Memory::Allocator::FreePool(Pointer);
        }
    }

    extern "C" auto RuntimeCalloc(Foundation::Size Count, Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();

        const auto Total = Count * Size;
        auto* MemoryBlock = RuntimeMalloc(Total);

        if (MemoryBlock)
        {
            RuntimeMemset(MemoryBlock, 0, Total);
        }

        return MemoryBlock;
    }

    extern "C" auto RuntimeRealloc(Foundation::Void* Pointer, Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();

        if (!Pointer)
        {
            return RuntimeMalloc(Size);
        }

        auto* NewBlock = RuntimeMalloc(Size);

        if (!NewBlock)
        {
            return nullptr;
        }

        RuntimeMemcpy(NewBlock, Pointer, Size);
        RuntimeFree(Pointer);

        return NewBlock;
    }

    extern "C" auto RuntimeMemcpy(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        Memory::Copy(Destination, Source, Size);
        return Destination;
    }

    extern "C" auto RuntimeMemmove(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        Memory::Move(Destination, Source, Size);
        return Destination;
    }

    extern "C" auto RuntimeMemset(Foundation::Void* Destination, Foundation::Int32 Value, Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        Memory::Set(Destination, static_cast<Foundation::Uint8>(Value), Size);
        return Destination;
    }

    extern "C" auto RuntimeMemcmp(const Foundation::Void* Left, const Foundation::Void* Right, Foundation::Size Size) -> Foundation::Int32
    {
        LogCall();
        return Memory::Compare(Left, Right, Size);
    }

    extern "C" auto RuntimeStrlen(const Foundation::Char* String) -> Foundation::Size
    {
        LogCall();

        Foundation::Size Length{};

        while (String && String[Length])
        {
            ++Length;
        }

        return Length;
    }

    extern "C" auto RuntimeWcslen(const Foundation::WChar* String) -> Foundation::Size
    {
        LogCall();

        Foundation::Size Length{};

        while (String && String[Length])
        {
            ++Length;
        }

        return Length;
    }

    extern "C" auto RuntimeStrcmp(const Foundation::Char* Left, const Foundation::Char* Right) -> Foundation::Int32
    {
        LogCall();

        while (*Left && *Left == *Right)
        {
            ++Left;
            ++Right;
        }

        return static_cast<unsigned char>(*Left) - static_cast<unsigned char>(*Right);
    }

    extern "C" auto RuntimeStrncmp(const Foundation::Char* Left, const Foundation::Char* Right, Foundation::Size Count) -> Foundation::Int32
    {
        LogCall();

        for (Foundation::Size i = 0; i < Count; ++i)
        {
            if (Left[i] != Right[i] || !Left[i] || !Right[i])
            {
                return static_cast<unsigned char>(Left[i]) - static_cast<unsigned char>(Right[i]);
            }
        }

        return 0;
    }

    extern "C" auto RuntimeWcscmp(const Foundation::WChar* Left, const Foundation::WChar* Right) -> Foundation::Int32
    {
        LogCall();

        while (*Left && *Left == *Right)
        {
            ++Left;
            ++Right;
        }

        return *Left - *Right;
    }

    extern "C" auto RuntimeWcsncmp(const Foundation::WChar* Left, const Foundation::WChar* Right, Foundation::Size Count) -> Foundation::Int32
    {
        LogCall();

        for (Foundation::Size i = 0; i < Count; ++i)
        {
            if (Left[i] != Right[i] || !Left[i] || !Right[i])
            {
                return Left[i] - Right[i];
            }
        }

        return 0;
    }

    extern "C" auto RuntimeStrcpy(Foundation::Char* Destination, const Foundation::Char* Source) -> Foundation::Char*
    {
        LogCall();

        auto* Result = Destination;

        while ((*Destination++ = *Source++) != 0) {}

        return Result;
    }

    extern "C" auto RuntimeStrncpy(Foundation::Char* Destination, const Foundation::Char* Source, Foundation::Size Count) -> Foundation::Char*
    {
        LogCall();

        Foundation::Size i{};

        for (; i < Count && Source[i]; ++i)
        {
            Destination[i] = Source[i];
        }

        for (; i < Count; ++i)
        {
            Destination[i] = 0;
        }

        return Destination;
    }

    extern "C" auto RuntimeWcscpy(Foundation::WChar* Destination, const Foundation::WChar* Source) -> Foundation::WChar*
    {
        LogCall();

        auto* Result = Destination;

        while ((*Destination++ = *Source++) != 0) {}

        return Result;
    }

    extern "C" auto RuntimeWcsncpy(Foundation::WChar* Destination, const Foundation::WChar* Source, Foundation::Size Count) -> Foundation::WChar*
    {
        LogCall();

        Foundation::Size i{};

        for (; i < Count && Source[i]; ++i)
        {
            Destination[i] = Source[i];
        }

        for (; i < Count; ++i)
        {
            Destination[i] = 0;
        }

        return Destination;
    }

    extern "C" auto RuntimeCeilf(Foundation::Float Value) -> Foundation::Float
    {
        LogCall();

        const auto Integer = static_cast<Foundation::Int32>(Value);

        if (static_cast<Foundation::Float>(Integer) == Value)
        {
            return Value;
        }

        if (Value > 0.0f)
        {
            return static_cast<Foundation::Float>(Integer + 1);
        }

        return static_cast<Foundation::Float>(Integer);
    }

    extern "C" auto RuntimeAcrtIobFunc(Foundation::Int32 Index) -> Foundation::Void*
    {
        LogCall();

        if (Index < 0 || Index >= 3)
        {
            return nullptr;
        }

        return &RuntimeFiles[Index];
    }

    extern "C" auto RuntimePutchar(Foundation::Int32 Character) -> Foundation::Int32
    {
        //LogCall();
        auto* Host = GetHostContext();

        if (Character == '\n')
        {
            Foundation::WChar NewLine[]{ L'\r', L'\n', 0 };
            Host->SystemTable->ConsoleOut->OutputString(Host->SystemTable->ConsoleOut, NewLine);
            return Character;
        }

        Foundation::WChar Text[2]{ static_cast<Foundation::WChar>(Character), 0 };
        Host->SystemTable->ConsoleOut->OutputString(Host->SystemTable->ConsoleOut, Text);

        return Character;
    }

    extern "C" auto RuntimePuts(const Foundation::Char* String) -> Foundation::Int32
    {
        LogCall();

        if (!String)
        {
            return -1;
        }

        Foundation::Int32 Count{};

        while (*String)
        {
            RuntimePutchar(*String++);
            ++Count;
        }

        RuntimePutchar('\n');

        return Count;
    }

    namespace
    {
        auto RuntimePrintUnsigned(Foundation::Uint64 Value, Foundation::Uint32 Base, Foundation::Bool Uppercase) -> Foundation::Int32
        {
            const char* Digits = Uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

            char Buffer[32]{};
            auto* Out = Buffer + sizeof(Buffer);

            *--Out = 0;

            do
            {
                *--Out = Digits[Value % Base];
                Value /= Base;
            } while (Value);

            Foundation::Int32 Count{};

            while (*Out)
            {
                RuntimePutchar(*Out++);
                ++Count;
            }

            return Count;
        }
    }

    extern "C" auto RuntimeVprintf(const Foundation::Char* Format, va_list Args) -> Foundation::Int32
    {
		LogCall();

        if (!Format)
        {
            return -1;
        }

        Foundation::Int32 Count{};

        while (*Format)
        {
            if (*Format != '%')
            {
                RuntimePutchar(*Format++);
                ++Count;
                continue;
            }

            ++Format;

            if (*Format == '%')
            {
                RuntimePutchar('%');
                ++Format;
                ++Count;
                continue;
            }

            switch (*Format++)
            {
            case 'c':
            {
                RuntimePutchar(va_arg(Args, int));
                ++Count;
                break;
            }

            case 's':
            {
                auto* String = va_arg(Args, const Foundation::Char*);

                if (!String)
                {
                    String = "(null)";
                }

                while (*String)
                {
                    RuntimePutchar(*String++);
                    ++Count;
                }

                break;
            }

            case 'd':
            case 'i':
            {
                const auto Value = va_arg(Args, int);

                if (Value < 0)
                {
                    RuntimePutchar('-');
                    ++Count;
                    Count += RuntimePrintUnsigned(static_cast<Foundation::Uint64>(-Value), 10, false);
                }
                else
                {
                    Count += RuntimePrintUnsigned(static_cast<Foundation::Uint64>(Value), 10, false);
                }

                break;
            }

            case 'u':
            {
                Count += RuntimePrintUnsigned(va_arg(Args, unsigned int), 10, false);
                break;
            }

            case 'x':
            {
                Count += RuntimePrintUnsigned(va_arg(Args, unsigned int), 16, false);
                break;
            }

            case 'X':
            {
                Count += RuntimePrintUnsigned(va_arg(Args, unsigned int), 16, true);
                break;
            }

            case 'p':
            {
                auto Value = reinterpret_cast<Foundation::Uint64>(va_arg(Args, Foundation::Void*));
                RuntimePutchar('0');
                RuntimePutchar('x');
                Count += 2;
                Count += RuntimePrintUnsigned(Value, 16, false);
                break;
            }

            default:
            {
                RuntimePutchar('%');
                RuntimePutchar(Format[-1]);
                Count += 2;
                break;
            }
            }
        }

        return Count;
    }

    extern "C" auto RuntimePrintf(const Foundation::Char* Format, ...) -> Foundation::Int32
    {
        LogCall();

        va_list Args;
        va_start(Args, Format);

        const auto Result = RuntimeVprintf(Format, Args);

        va_end(Args);

        return Result;
    }

    extern "C" auto RuntimeVfprintf(Foundation::Void* File, const Foundation::Char* Format, va_list Args) -> Foundation::Int32
    {
		LogCall();

		if (!File)
		{
			return -1;
		}

		auto* RtFile = Foundation::Cast::Auto<RuntimeFile*>(File);

        switch (RtFile->Index)
        {
        case 1:
        case 2: return RuntimeVprintf(Format, Args);
        case 0:
        default: return -1;
        }
    }

    extern "C" auto RuntimeStdioCommonVfprintf(Foundation::Uint64 Options, Foundation::Void* Stream, const Foundation::Char* Format, Foundation::Void* Locale, va_list Args) -> Foundation::Int32
    {
        LogCall();
		return RuntimeVfprintf(Stream, Format, Args);
    }
}