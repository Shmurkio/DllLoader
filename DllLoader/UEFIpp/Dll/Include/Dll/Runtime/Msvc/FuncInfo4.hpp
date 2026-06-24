#pragma once

#include <UEFIpp/UEFIpp.hpp>
#include <Dll/Runtime/Msvc/ThrowInfo.hpp>

namespace Dll::Runtime::Msvc
{
    struct FuncInfoHeader
    {
        Foundation::Uint8 Value;

        [[nodiscard]] auto IsCatch() const -> Foundation::Bool { return (Value & (1 << 0)) != 0; }
        [[nodiscard]] auto IsSeparated() const -> Foundation::Bool { return (Value & (1 << 1)) != 0; }
        [[nodiscard]] auto HasBBT() const -> Foundation::Bool { return (Value & (1 << 2)) != 0; }
        [[nodiscard]] auto HasUnwindMap() const -> Foundation::Bool { return (Value & (1 << 3)) != 0; }
        [[nodiscard]] auto HasTryBlockMap() const -> Foundation::Bool { return (Value & (1 << 4)) != 0; }
        [[nodiscard]] auto HasEHs() const -> Foundation::Bool { return (Value & (1 << 5)) != 0; }
        [[nodiscard]] auto HasNoExcept() const -> Foundation::Bool { return (Value & (1 << 6)) != 0; }
    };

    struct FuncInfo4
    {
        FuncInfoHeader Header{};
        Foundation::Uint32 BbtFlags{};
        Foundation::Int32 DispUnwindMap{};
        Foundation::Int32 DispTryBlockMap{};
        Foundation::Int32 DispIPtoStateMap{};
        Foundation::Uint32 DispFrame{};
    };

    struct TryBlockMapEntry4
    {
        Foundation::Int32 TryLow{};
        Foundation::Int32 TryHigh{};
        Foundation::Int32 CatchHigh{};
        Foundation::Int32 DispHandlerArray{};
    };

    struct HandlerTypeHeader
    {
        Foundation::Uint8 Value{};

        [[nodiscard]] auto HasAdjectives() const -> Foundation::Bool { return (Value & (1 << 0)) != 0; }
        [[nodiscard]] auto HasDispType() const -> Foundation::Bool { return (Value & (1 << 1)) != 0; }
        [[nodiscard]] auto HasDispCatchObj() const -> Foundation::Bool { return (Value & (1 << 2)) != 0; }
        [[nodiscard]] auto ContIsRva() const -> Foundation::Bool { return (Value & (1 << 3)) != 0; }
        [[nodiscard]] auto ContAddrCount() const -> Foundation::Uint8 { return (Value >> 4) & 0x03; }
    };

    struct HandlerType4
    {
        HandlerTypeHeader Header{};
        Foundation::Uint32 Adjectives{};
        Foundation::Int32 DispType{};
        Foundation::Uint32 DispCatchObj{};
        Foundation::Int32 DispOfHandler{};
        Foundation::Uint64 ContinuationAddress[2]{};
    };

    struct MatchResult
    {
        Foundation::Bool Matched{};
        HandlerType4 Handler{};
        CatchableType Catchable{};
    };

    auto ReadInt(Foundation::Uint8*& Buffer) -> Foundation::Int32;
    auto ReadUnsigned(Foundation::Uint8*& Buffer) -> Foundation::Uint32;

    auto DecodeFuncInfo4(Foundation::Uint8* Buffer) -> FuncInfo4;

    auto FindMatchingHandler(Foundation::Uint8* Base, Foundation::Uint32 FunctionStartRva, const FuncInfo4& FuncInfo, const CatchableTypeArray* ThrowTypes, Foundation::Int32 SkipHandlerRva = -1) -> MatchResult;
}