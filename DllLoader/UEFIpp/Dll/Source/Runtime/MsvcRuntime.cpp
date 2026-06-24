#include <Dll/Runtime/MsvcRuntime.hpp>
#include <Dll/Runtime/CxxRuntime.hpp>
#include <Dll/Runtime/Msvc/ThrowInfo.hpp>
#include <Dll/Runtime/Msvc/UnwindInfo.hpp>
#include <Dll/Runtime/Msvc/FuncInfo4.hpp>
#include <Dll/Runtime/CrtRuntime.hpp>
#include <Dll/Runtime/Msvc/StdException.hpp>

#include <intrin.h>

#define LogCall() OnFuncCall(__func__)
#define Unhandled(Reason) OnUnhandledCall({ __func__, Reason })

extern "C" [[noreturn]] auto CallCatchAndContinue(Foundation::Void* CatchHandler, Foundation::Void* EstablisherFrame, Foundation::Void* Continuation, Foundation::Uint64 NewRsp) -> Foundation::Void;

namespace Dll::Runtime
{
    struct ActiveException
    {
        Foundation::Void* ExceptionObject{};
        Foundation::Void* RawThrowInfo{};
        Msvc::CatchableTypeArray* ThrowTypes{};

        Foundation::Uint64 FunctionRsp{};
        Foundation::Uint32 FunctionBeginAddress{};

        Msvc::FuncInfo4 FuncInfo{};
        Msvc::MatchResult Match{};
    };

    static Vector<ActiveException> ActiveExceptions{};

    static Foundation::Uint8* CurrentImageBase_{};

    alignas(8) Foundation::Byte TypeInfoVftable[8]{};

    auto SetCurrentImageBase(Foundation::Void* Base) -> Foundation::Void
    {
        CurrentImageBase_ = Foundation::Cast::Auto<Foundation::Uint8*>(Base);
    }

    auto CurrentImageBase() -> Foundation::Uint8*
    {
        return CurrentImageBase_;
    }

    extern "C" auto StdExceptionDestroy(Foundation::Void* Exception) -> Foundation::Void
    {
        LogCall();

        if (!Exception)
        {
            return;
        }

        auto* Data = Foundation::Cast::Auto<Msvc::StdException*>(Exception);

        if (Data->DoFree && Data->Message)
        {
            RuntimeFree(Data->Message);
            Data->Message = nullptr;
        }

        Data->DoFree = false;
    }

    extern "C" auto StdExceptionCopy(Foundation::Void* Destination, const Foundation::Void* Source) -> Foundation::Void
    {
        LogCall();

        if (!Destination || !Source)
        {
            return;
        }

        auto* Dst = Foundation::Cast::Auto<Msvc::StdException*>(Destination);
        auto* Src = Foundation::Cast::Auto<const Msvc::StdException*>(Source);

        Dst->Message = Src->Message;
        Dst->DoFree = false;
    }

    auto StdLengthError(const Foundation::Char*) -> Foundation::Void
    {
        LogCall();
        Unhandled("std::length_error helper is not implemented");
        Terminate();
    }

    auto StdBadFunctionCall() -> Foundation::Void
    {
        LogCall();
        Unhandled("std::bad_function_call helper is not implemented");
        Terminate();
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

    extern "C" auto CxxFrameHandler4(Foundation::Void* ExceptionRecord, Foundation::Void* EstablisherFrame, Foundation::Void* ContextRecord, Foundation::Void* DispatcherContext) -> Foundation::Int32
    {
        LogCall();
        Unhandled("__CxxFrameHandler4 direct dispatch is not implemented");
        Terminate();
        return 0;
    }

    extern "C" auto CxxThrowExceptionImpl(Foundation::Void* ExceptionObject, Foundation::Void* RawThrowInfo, Foundation::Uint64 ThrowRip, Foundation::Uint64 FunctionRsp) -> Foundation::Void
    {
        LogCall();

        auto* Base = CurrentImageBase();

        if (!Base)
        {
            Unhandled("CurrentImageBase is null");
            Terminate();
        }

        Foundation::Int32 SkipHandlerRva = -1;
        Foundation::Bool IsRethrow = false;

        if (!ExceptionObject || !RawThrowInfo)
        {
            if (ActiveExceptions.Empty())
            {
                Unhandled("Rethrow without active exception");
                Terminate();
            }

            const auto& Active = ActiveExceptions.Back();

            ExceptionObject = Active.ExceptionObject;
            RawThrowInfo = Active.RawThrowInfo;
            SkipHandlerRva = Active.Match.Handler.DispOfHandler;
            IsRethrow = true;
        }

        auto* Info = Foundation::Cast::Auto<Msvc::ThrowInfo*>(RawThrowInfo);

        auto* ThrowTypes = Foundation::Cast::Auto<Msvc::CatchableTypeArray*>(Base + Info->PCatchableTypeArray);

        Msvc::FuncInfo4 FuncInfo{};
        Foundation::Uint32 FunctionBeginAddress{};
        Foundation::Uint64 HandlerFunctionRsp = FunctionRsp;

        if (IsRethrow)
        {
            const auto& Active = ActiveExceptions.Back();
            FuncInfo = Active.FuncInfo;
            FunctionBeginAddress = Active.FunctionBeginAddress;
            HandlerFunctionRsp = Active.FunctionRsp;
        }
        else
        {
            auto* Function = Msvc::LookupRuntimeFunction(Base, ThrowRip);

            if (!Function)
            {
                Unhandled("No runtime function found for throw RIP");
                Terminate();
            }

            auto* UnwindInfo = Base + Function->UnwindInfoAddress;
            auto* Handler = Msvc::HandlerInfoFromUnwindInfo(UnwindInfo);

            FuncInfo = Msvc::DecodeFuncInfo4(Base + Handler->HandlerDataRva);
            FunctionBeginAddress = Function->BeginAddress;
        }

        auto Match = Msvc::FindMatchingHandler(Base, FunctionBeginAddress, FuncInfo, ThrowTypes, SkipHandlerRva);

        if (!Match.Matched)
        {
            Unhandled("No matching catch handler found");
            Terminate();
        }

        auto* CatchObject = Foundation::Cast::Auto<Foundation::Uint8*>(HandlerFunctionRsp) + Match.Handler.DispCatchObj;
        auto* SourceObject = Foundation::Cast::Auto<Foundation::Uint8*>(ExceptionObject) + Match.Catchable.ThisDisplacementMdisp;

        const auto IsReferenceCatch = (Match.Handler.Adjectives & 0x8) != 0;

        if (IsReferenceCatch)
        {
            *Foundation::Cast::Auto<Foundation::Void**>(CatchObject) = SourceObject;
        }
        else if (Match.Catchable.CopyFunctionRva != 0 && Match.Catchable.CopyFunctionRva != -1)
        {
            using CopyFn = auto (*)( Foundation::Void* Destination, Foundation::Void* Source)->Foundation::Void*;
            auto Copy = Foundation::Cast::Auto<CopyFn>(Base + Match.Catchable.CopyFunctionRva);
            Copy(CatchObject, SourceObject);
        }
        else
        {
            Memory::Copy(CatchObject, SourceObject, Match.Catchable.SizeOrOffset);
        }

        using CatchFn = auto (*)(Foundation::Void* ExceptionRecord, Foundation::Void* EstablisherFrame)->Foundation::Void;
        auto CatchHandler = Foundation::Cast::Auto<CatchFn>(Base + Match.Handler.DispOfHandler);

        auto* Continuation = Foundation::Cast::Auto<Foundation::Void*>(Base + Match.Handler.ContinuationAddress[0]);

        ActiveExceptions.PushBack({ ExceptionObject, RawThrowInfo, ThrowTypes, HandlerFunctionRsp, FunctionBeginAddress, FuncInfo, Match });

        CallCatchAndContinue(Foundation::Cast::Auto<Foundation::Void*>(CatchHandler), Foundation::Cast::Auto<Foundation::Void*>(HandlerFunctionRsp), Continuation, HandlerFunctionRsp);

        Unhandled("CallCatchAndContinue returned unexpectedly");
        Terminate();
    }
}