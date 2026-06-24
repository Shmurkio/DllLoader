#include <Dll/Runtime/Msvc/FuncInfo4.hpp>

namespace Dll::Runtime::Msvc
{
    static constexpr Foundation::Int8 NegLengthTable[16]
    {
        -1, -2, -1, -3,
        -1, -2, -1, -4,
        -1, -2, -1, -3,
        -1, -2, -1, -5,
    };

    static constexpr Foundation::Uint8 ShiftTable[16]
    {
        25, 18, 25, 11,
        25, 18, 25, 4,
        25, 18, 25, 11,
        25, 18, 25, 0,
    };

    auto ReadInt(Foundation::Uint8*& Buffer) -> Foundation::Int32
    {
        const auto Value = *Foundation::Cast::Auto<Foundation::Int32*>(Buffer);
        Buffer += sizeof(Foundation::Int32);
        return Value;
    }

    auto ReadUnsigned(Foundation::Uint8*& Buffer) -> Foundation::Uint32
    {
        const auto LengthBits = *Buffer & 0x0F;
        const auto NegLength = NegLengthTable[LengthBits];
        const auto Shift = ShiftTable[LengthBits];

        auto Result = *Foundation::Cast::Auto<Foundation::Uint32*>(Buffer - NegLength - 4);

        Result >>= Shift;
        Buffer -= NegLength;

        return Result;
    }

    auto DecodeFuncInfo4(Foundation::Uint8* Buffer) -> FuncInfo4
    {
        FuncInfo4 Info{};

        Info.Header.Value = *Buffer;
        ++Buffer;

        if (Info.Header.HasBBT())
        {
            Info.BbtFlags = ReadUnsigned(Buffer);
        }

        if (Info.Header.HasUnwindMap())
        {
            Info.DispUnwindMap = ReadInt(Buffer);
        }

        if (Info.Header.HasTryBlockMap())
        {
            Info.DispTryBlockMap = ReadInt(Buffer);
        }

        Info.DispIPtoStateMap = ReadInt(Buffer);

        if (Info.Header.IsCatch())
        {
            Info.DispFrame = ReadUnsigned(Buffer);
        }

        return Info;
    }

    static auto TypeNamesEqual(const Foundation::Char* A, const Foundation::Char* B) -> Foundation::Bool
    {
        if (!A || !B)
        {
            return false;
        }

        while (*A && *B)
        {
            if (*A != *B)
            {
                return false;
            }

            ++A;
            ++B;
        }

        return *A == *B;
    }

    static auto DecodeHandler(Foundation::Uint8*& Buffer, Foundation::Uint32 FunctionStartRva) -> HandlerType4
    {
        HandlerType4 Handler{};

        Handler.Header.Value = *Buffer;
        ++Buffer;

        if (Handler.Header.HasAdjectives())
        {
            Handler.Adjectives = ReadUnsigned(Buffer);
        }

        if (Handler.Header.HasDispType())
        {
            Handler.DispType = ReadInt(Buffer);
        }

        if (Handler.Header.HasDispCatchObj())
        {
            Handler.DispCatchObj = ReadUnsigned(Buffer);
        }

        Handler.DispOfHandler = ReadInt(Buffer);

        const auto ContinuationCount = Handler.Header.ContAddrCount();

        for (Foundation::Uint8 i = 0; i < ContinuationCount && i < 2; ++i)
        {
            if (Handler.Header.ContIsRva())
            {
                Handler.ContinuationAddress[i] = Foundation::Cast::Auto<Foundation::Uint64>(ReadInt(Buffer));
            }
            else
            {
                Handler.ContinuationAddress[i] = FunctionStartRva + ReadUnsigned(Buffer);
            }
        }

        return Handler;
    }

    static auto FindMatchingHandlerInTryBlock(Foundation::Uint8* Base, Foundation::Uint32 FunctionStartRva,const TryBlockMapEntry4& TryBlock, const CatchableTypeArray* ThrowTypes, Foundation::Int32 SkipHandlerRva = -1) -> MatchResult
    {
        auto* Buffer = Base + TryBlock.DispHandlerArray;
        const auto HandlerCount = ReadUnsigned(Buffer);

        for (Foundation::Uint32 i = 0; i < HandlerCount; ++i)
        {
            auto Handler = DecodeHandler(Buffer, FunctionStartRva);

            if (Handler.DispOfHandler == SkipHandlerRva)
            {
                continue;
            }

            // catch(...)
            if (!Handler.DispType)
            {
                return { true, Handler, {} };
            }

            const auto* CatchTypeName = TypeNameFromDescriptor(Base, Handler.DispType);

            for (Foundation::Int32 t = 0; t < ThrowTypes->Count; ++t)
            {
                auto* Catchable = Foundation::Cast::Auto<CatchableType*>(Base + ThrowTypes->Types[t]);

                const auto* ThrowTypeName = TypeNameFromDescriptor(Base, Catchable->TypeDescriptorRva);

                if (TypeNamesEqual(CatchTypeName, ThrowTypeName))
                {
                    return { true, Handler, *Catchable };
                }
            }
        }

        return {};
    }

    auto FindMatchingHandler(Foundation::Uint8* Base, Foundation::Uint32 FunctionStartRva, const FuncInfo4& FuncInfo, const CatchableTypeArray* ThrowTypes, Foundation::Int32 SkipHandlerRva) -> MatchResult
    {
        if (!FuncInfo.Header.HasTryBlockMap())
        {
            return {};
        }

        auto* Buffer = Base + FuncInfo.DispTryBlockMap;
        const auto TryBlockCount = ReadUnsigned(Buffer);

        for (Foundation::Uint32 i = 0; i < TryBlockCount; ++i)
        {
            TryBlockMapEntry4 TryBlock{};

            TryBlock.TryLow = ReadUnsigned(Buffer);
            TryBlock.TryHigh = ReadUnsigned(Buffer);
            TryBlock.CatchHigh = ReadUnsigned(Buffer);
            TryBlock.DispHandlerArray = ReadInt(Buffer);

            auto Match = FindMatchingHandlerInTryBlock(Base, FunctionStartRva, TryBlock, ThrowTypes, SkipHandlerRva);

            if (Match.Matched)
            {
                return Match;
            }
        }

        return {};
    }
}