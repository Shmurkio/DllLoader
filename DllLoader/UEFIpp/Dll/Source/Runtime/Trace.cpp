#include <Dll/Runtime/Trace.hpp>

namespace Dll::Runtime
{
    static LoadedImageTraceContext TraceContext_{};

    Event<TraceInfo> OnTrace{};

    auto SetTraceImageContext(Foundation::Void* Base, Foundation::Size Size) -> Foundation::Void
    {
        TraceContext_.Base = Base;
        TraceContext_.Size = Size;
        TraceContext_.Image = UEFIpp::Loader::PeImage{ Base };
    }

    auto ClearTraceImageContext() -> Foundation::Void
    {
        TraceContext_ = {};
    }

    auto TraceImageContext() -> const LoadedImageTraceContext&
    {
        return TraceContext_;
    }

    auto Trace(TraceLevel Level, TraceCategory Category, StringView FunctionName, StringView FileName, Foundation::Uint32 LineNumber, StringView Message) -> Foundation::Void
    {
        OnTrace({ Level, Category, FunctionName, FileName, LineNumber, Message });
    }

    auto TraceRip(TraceLevel Level, TraceCategory Category, StringView FunctionName, StringView FileName, Foundation::Uint32 LineNumber, StringView Message, Foundation::Uint64 Rip) -> Foundation::Void
    {
        TraceInfo Info{};
        Info.Level = Level;
        Info.Category = Category;
        Info.FunctionName = FunctionName;
        Info.FileName = FileName;
        Info.LineNumber = LineNumber;
        Info.Message = Message;
        Info.Rip = Rip;

        if (TraceContext_.Base && Rip)
        {
            const auto ImageBase = Foundation::Cast::PointerToAddress<Foundation::Uint64>(TraceContext_.Base);

            Info.ImageBase = ImageBase;
            Info.ImageSize = TraceContext_.Size;

            if (Rip >= ImageBase && Rip < ImageBase + TraceContext_.Size)
            {
                Info.Rva = Rip - ImageBase;

                const auto Section = TraceContext_.Image.SectionByAddress(Rip);

                if (Section)
                {
                    Info.SectionName = Section->Name.View();
                }
            }
        }

        OnTrace(Info);
    }

    auto TraceLevelName(TraceLevel Level) -> StringView
    {
        switch (Level)
        {
        case TraceLevel::Verbose: return "Verbose";
        case TraceLevel::Info: return "Info";
        case TraceLevel::Warning: return "Warning";
        case TraceLevel::Error: return "Error";
        case TraceLevel::Fatal: return "Fatal";
        default: return "Unknown";
        }
    }

    auto TraceCategoryName(TraceCategory Category) -> StringView
    {
        switch (Category)
        {
        case TraceCategory::Loader: return "Loader";
        case TraceCategory::Import: return "Import";
        case TraceCategory::Runtime: return "Runtime";
        case TraceCategory::Host: return "Host";
        case TraceCategory::CRT: return "CRT";
        case TraceCategory::CXX: return "CXX";
        case TraceCategory::MSVC: return "MSVC";
        case TraceCategory::EH: return "EH";
        default: return "Unknown";
        }
    }

    auto TraceImport(TraceLevel Level, StringView FunctionName, StringView FileName, Foundation::Uint32 LineNumber, StringView Message, StringView Module, StringView Symbol, Foundation::Uint64 Address) -> void
    {
        TraceInfo Info{};

        Info.Level = Level;
        Info.Category = TraceCategory::Import;
        Info.FunctionName = FunctionName;
        Info.FileName = FileName;
        Info.LineNumber = LineNumber;
        Info.Message = Message;
        Info.Module = Module;
        Info.Symbol = Symbol;
        Info.Address = Address;

        OnTrace(Info);
    }
}