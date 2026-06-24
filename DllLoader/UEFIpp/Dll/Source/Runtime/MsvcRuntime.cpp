#include <Dll/Runtime/MsvcRuntime.hpp>
#include <Dll/Runtime/CxxRuntime.hpp>
#include <Dll/Runtime/Msvc/ThrowInfo.hpp>
#include <Dll/Runtime/Msvc/UnwindInfo.hpp>
#include <Dll/Runtime/Msvc/FuncInfo4.hpp>
#include <Dll/Runtime/CrtRuntime.hpp>
#include <Dll/Runtime/Msvc/StdException.hpp>
#include <Dll/Runtime/Trace.hpp>
#include <intrin.h>

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
		DLL_MSVC_TRACE(__func__);

        if (!Exception)
        {
            DLL_TRACE_WARNING(::Dll::Runtime::TraceCategory::MSVC, "StdExceptionDestroy called with null exception");
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
		DLL_MSVC_TRACE(__func__);

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
        DLL_MSVC_TRACE(__func__);
        DLL_TRACE_FATAL(::Dll::Runtime::TraceCategory::MSVC, "std::length_error is not implemented");
        Terminate();
    }

    auto StdBadFunctionCall() -> Foundation::Void
    {
        DLL_MSVC_TRACE(__func__);
        DLL_TRACE_FATAL(::Dll::Runtime::TraceCategory::MSVC, "std::bad_function_call is not implemented");
        Terminate();
    }

    auto StdTypeInfoCompare(const Foundation::Void* Left, const Foundation::Void* Right) -> Foundation::Int32
    {
        DLL_MSVC_TRACE(__func__);

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
		DLL_MSVC_TRACE(__func__);
		DLL_TRACE_FATAL(::Dll::Runtime::TraceCategory::MSVC, "_CxxFrameHandler4 is not implemented");
        Terminate();
        return 0;
    }

    extern "C" auto CxxThrowExceptionImpl(Foundation::Void* ExceptionObject, Foundation::Void* RawThrowInfo, Foundation::Uint64 ThrowRip, Foundation::Uint64 FunctionRsp) -> Foundation::Void
    {
		DLL_MSVC_TRACE(__func__);
        DLL_EH_TRACE_RIP("Enter", ThrowRip);

        auto* Base = CurrentImageBase();

        if (!Base)
        {
            DLL_EH_TRACE_RIP("CurrentImageBase is null", ThrowRip);
            DLL_TRACE_FATAL(::Dll::Runtime::TraceCategory::MSVC, "CurrentImageBase is null");
            Terminate();
        }

        DLL_EH_TRACE_RIP("Image base resolved", ThrowRip);

        Foundation::Int32 SkipHandlerRva = -1;
        Foundation::Bool IsRethrow = false;

        if (!ExceptionObject || !RawThrowInfo)
        {
            DLL_EH_TRACE_RIP("Rethrow path", ThrowRip);

            if (ActiveExceptions.Empty())
            {
                DLL_EH_TRACE_RIP("Rethrow without active exception", ThrowRip);
                DLL_TRACE_FATAL(::Dll::Runtime::TraceCategory::MSVC, "Rethrow without active exception");
                Terminate();
            }

            const auto& Active = ActiveExceptions.Back();

            ExceptionObject = Active.ExceptionObject;
            RawThrowInfo = Active.RawThrowInfo;
            SkipHandlerRva = Active.Match.Handler.DispOfHandler;
            IsRethrow = true;

            DLL_EH_TRACE_RIP("Active exception restored", ThrowRip);
        }

        DLL_EH_TRACE_RIP("Throw object/info ready", ThrowRip);

        auto* Info = Foundation::Cast::Auto<Msvc::ThrowInfo*>(RawThrowInfo);
        auto* ThrowTypes = Foundation::Cast::Auto<Msvc::CatchableTypeArray*>(Base + Info->PCatchableTypeArray);

        DLL_EH_TRACE_RIP("Throw metadata decoded", ThrowRip);

        Msvc::FuncInfo4 FuncInfo{};
        Foundation::Uint32 FunctionBeginAddress{};
        Foundation::Uint64 HandlerFunctionRsp = FunctionRsp;

        if (IsRethrow)
        {
            DLL_EH_TRACE_RIP("Using active exception function metadata", ThrowRip);

            const auto& Active = ActiveExceptions.Back();
            FuncInfo = Active.FuncInfo;
            FunctionBeginAddress = Active.FunctionBeginAddress;
            HandlerFunctionRsp = Active.FunctionRsp;

            DLL_EH_TRACE_RIP("Active metadata ready", ThrowRip);
        }
        else
        {
            DLL_EH_TRACE_RIP("Looking up runtime function", ThrowRip);

            auto* Function = Msvc::LookupRuntimeFunction(Base, ThrowRip);

            if (!Function)
            {
                DLL_EH_TRACE_RIP("No runtime function found", ThrowRip);
                DLL_TRACE_FATAL(::Dll::Runtime::TraceCategory::MSVC, "No runtime function found for throw RIP");
                Terminate();
            }

            DLL_EH_TRACE_RIP("Runtime function found", ThrowRip);

            auto* UnwindInfo = Base + Function->UnwindInfoAddress;
            DLL_EH_TRACE_RIP("Unwind info resolved", ThrowRip);

            auto* Handler = Msvc::HandlerInfoFromUnwindInfo(UnwindInfo);
            DLL_EH_TRACE_RIP("Handler info resolved", ThrowRip);

            FuncInfo = Msvc::DecodeFuncInfo4(Base + Handler->HandlerDataRva);
            FunctionBeginAddress = Function->BeginAddress;

            DLL_EH_TRACE_RIP("FuncInfo4 decoded", ThrowRip);
        }

        DLL_EH_TRACE_RIP("Finding matching handler", ThrowRip);

        auto Match = Msvc::FindMatchingHandler(Base, FunctionBeginAddress, FuncInfo, ThrowTypes, SkipHandlerRva);

        if (!Match.Matched)
        {
            DLL_EH_TRACE_RIP("No matching handler found", ThrowRip);
            DLL_TRACE_FATAL(::Dll::Runtime::TraceCategory::MSVC, "No matching catch handler found");
            Terminate();
        }

        DLL_EH_TRACE_RIP("Matching handler found", ThrowRip);

        auto* CatchObject = Foundation::Cast::Auto<Foundation::Uint8*>(HandlerFunctionRsp) + Match.Handler.DispCatchObj;
        auto* SourceObject = Foundation::Cast::Auto<Foundation::Uint8*>(ExceptionObject) + Match.Catchable.ThisDisplacementMdisp;

        DLL_EH_TRACE_RIP("Catch/source object computed", ThrowRip);

        const auto IsReferenceCatch = (Match.Handler.Adjectives & 0x8) != 0;

        if (IsReferenceCatch)
        {
            DLL_EH_TRACE_RIP("Preparing reference catch", ThrowRip);
            *Foundation::Cast::Auto<Foundation::Void**>(CatchObject) = SourceObject;
            DLL_EH_TRACE_RIP("Reference catch prepared", ThrowRip);
        }
        else if (Match.Catchable.CopyFunctionRva != 0 && Match.Catchable.CopyFunctionRva != -1)
        {
            DLL_EH_TRACE_RIP("Calling catch object copy function", ThrowRip);

            using CopyFn = auto (*)(Foundation::Void* Destination, Foundation::Void* Source)->Foundation::Void*;
            auto Copy = Foundation::Cast::Auto<CopyFn>(Base + Match.Catchable.CopyFunctionRva);
            Copy(CatchObject, SourceObject);

            DLL_EH_TRACE_RIP("Catch object copy completed", ThrowRip);
        }
        else
        {
            DLL_EH_TRACE_RIP("Raw-copying catch object", ThrowRip);

            Memory::Copy(CatchObject, SourceObject, Match.Catchable.SizeOrOffset);

            DLL_EH_TRACE_RIP("Raw catch object copy completed", ThrowRip);
        }

        DLL_EH_TRACE_RIP("Resolving catch handler", ThrowRip);

        using CatchFn = auto (*)(Foundation::Void* ExceptionRecord, Foundation::Void* EstablisherFrame)->Foundation::Void;

        auto CatchHandler = Foundation::Cast::Auto<CatchFn>(Base + Match.Handler.DispOfHandler);

        auto* Continuation = Foundation::Cast::Auto<Foundation::Void*>(Base + Match.Handler.ContinuationAddress[0]);

        DLL_EH_TRACE_RIP("Catch handler and continuation resolved", ThrowRip);

        ActiveExceptions.PushBack({ ExceptionObject, RawThrowInfo, ThrowTypes, HandlerFunctionRsp, FunctionBeginAddress, FuncInfo, Match });

        DLL_EH_TRACE_RIP("Active exception pushed", ThrowRip);
        DLL_EH_TRACE_RIP("Entering catch trampoline", ThrowRip);

        CallCatchAndContinue(Foundation::Cast::Auto<Foundation::Void*>(CatchHandler), Foundation::Cast::Auto<Foundation::Void*>(HandlerFunctionRsp), Continuation, HandlerFunctionRsp);

        DLL_EH_TRACE_RIP("CallCatchAndContinue returned unexpectedly", ThrowRip);
        DLL_TRACE_FATAL(::Dll::Runtime::TraceCategory::MSVC, "CallCatchAndContinue returned unexpectedly");
        Terminate();
    }
}