#pragma once

#include <UEFIpp/UEFIpp.hpp>
#include <UEFIpp/Loader/PeImage.hpp>

namespace Dll::Runtime
{
    enum class TraceLevel : Foundation::Uint8
    {
        Verbose,
        Info,
        Warning,
        Error,
        Fatal
    };

    enum class TraceCategory : Foundation::Uint8
    {
        Loader,
        Import,
        Runtime,
        Host,
        CRT,
        CXX,
        MSVC,
        EH
    };

    struct LoadedImageTraceContext
    {
        Foundation::Void* Base{};
        Foundation::Size Size{};
        UEFIpp::Loader::PeImage Image{};
    };

    struct TraceInfo
    {
        TraceLevel Level{};
        TraceCategory Category{};

        StringView FunctionName{};
        StringView FileName{};
        Foundation::Uint32 LineNumber{};

        StringView Message{};

        Foundation::Uint64 Rip{};
        Foundation::Uint64 ImageBase{};
        Foundation::Uint64 Rva{};
        Foundation::Size ImageSize{};
        StringView SectionName{};

        StringView Module{};
        StringView Symbol{};

        Foundation::Uint64 Address{};
    };

    extern Event<TraceInfo> OnTrace;

    auto SetTraceImageContext(Foundation::Void* Base, Foundation::Size Size) -> Foundation::Void;
    auto ClearTraceImageContext() -> Foundation::Void;
    auto TraceImageContext() -> const LoadedImageTraceContext&;

    auto Trace(TraceLevel Level, TraceCategory Category, StringView FunctionName, StringView FileName, Foundation::Uint32 LineNumber, StringView Message) -> Foundation::Void;

    auto TraceRip(TraceLevel Level, TraceCategory Category, StringView FunctionName, StringView FileName, Foundation::Uint32 LineNumber, StringView Message, Foundation::Uint64 Rip) -> Foundation::Void;

    auto TraceLevelName(TraceLevel Level) -> StringView;
    auto TraceCategoryName(TraceCategory Category) -> StringView;
    auto TraceImport(TraceLevel Level, StringView FunctionName, StringView FileName, Foundation::Uint32 LineNumber, StringView Message, StringView Module, StringView Symbol, Foundation::Uint64 Address) -> Foundation::Void;
}

#define DLL_TRACE(Level, Category, Message) \
    ::Dll::Runtime::Trace(Level, Category, __func__, __FILE__, __LINE__, Message)

#define DLL_TRACE_RIP(Level, Category, Message, Rip) \
    ::Dll::Runtime::TraceRip(Level, Category, __func__, __FILE__, __LINE__, Message, Rip)

#define DLL_TRACE_VERBOSE(Category, Message) \
    DLL_TRACE(::Dll::Runtime::TraceLevel::Verbose, Category, Message)

#define DLL_TRACE_INFO(Category, Message) \
    DLL_TRACE(::Dll::Runtime::TraceLevel::Info, Category, Message)

#define DLL_TRACE_WARNING(Category, Message) \
    DLL_TRACE(::Dll::Runtime::TraceLevel::Warning, Category, Message)

#define DLL_TRACE_ERROR(Category, Message) \
    DLL_TRACE(::Dll::Runtime::TraceLevel::Error, Category, Message)

#define DLL_TRACE_FATAL(Category, Message) \
    DLL_TRACE(::Dll::Runtime::TraceLevel::Fatal, Category, Message)

#define DLL_EH_TRACE(Message) \
    DLL_TRACE_INFO(::Dll::Runtime::TraceCategory::EH, Message)

#define DLL_EH_TRACE_RIP(Message, Rip) \
    DLL_TRACE_RIP(::Dll::Runtime::TraceLevel::Info, ::Dll::Runtime::TraceCategory::EH, Message, Rip)

#define DLL_RUNTIME_TRACE(Message) \
    DLL_TRACE_INFO(::Dll::Runtime::TraceCategory::Runtime, Message)

#define DLL_HOST_TRACE(Message) \
    DLL_TRACE_INFO(::Dll::Runtime::TraceCategory::Host, Message)

#define DLL_CRT_TRACE(Message) \
    DLL_TRACE_VERBOSE(::Dll::Runtime::TraceCategory::CRT, Message)

#define DLL_CXX_TRACE(Message) \
    DLL_TRACE_VERBOSE(::Dll::Runtime::TraceCategory::CXX, Message)

#define DLL_MSVC_TRACE(Message) \
    DLL_TRACE_VERBOSE(::Dll::Runtime::TraceCategory::MSVC, Message)

#define DLL_IMPORT_TRACE(Level, Message, Module, Symbol, Address) \
    ::Dll::Runtime::TraceImport(Level, __func__, __FILE__, __LINE__, Message, Module, Symbol, Foundation::Cast::Auto<Foundation::Uint64>(Address))

#define DLL_IMPORT_TRACE_VERBOSE(Message, Module, Symbol, Address) \
    DLL_IMPORT_TRACE(::Dll::Runtime::TraceLevel::Verbose, Message, Module, Symbol, Address)

#define DLL_IMPORT_TRACE_INFO(Message, Module, Symbol, Address) \
    DLL_IMPORT_TRACE(::Dll::Runtime::TraceLevel::Info, Message, Module, Symbol, Address)

#define DLL_IMPORT_TRACE_WARNING(Message, Module, Symbol, Address) \
    DLL_IMPORT_TRACE(::Dll::Runtime::TraceLevel::Warning, Message, Module, Symbol, Address)

#define DLL_IMPORT_TRACE_ERROR(Message, Module, Symbol, Address) \
    DLL_IMPORT_TRACE(::Dll::Runtime::TraceLevel::Error, Message, Module, Symbol, Address)