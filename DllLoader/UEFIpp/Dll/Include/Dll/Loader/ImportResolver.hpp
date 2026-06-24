#pragma once

#include <UEFIpp/UEFIpp.hpp>

namespace Dll::Loader
{
    enum class ImportKind
    {
        Function,
        Data
    };

    struct ImportSymbol
    {
        StringView Module;
        StringView Name;
        Foundation::Void* Address{};
        ImportKind Kind{ ImportKind::Function };
    };

    enum class ImportResolveFailure
    {
        UnsupportedOrdinal,
        UnresolvedSymbol
    };

    struct ImportResolvedInfo
    {
        StringView Module;
        StringView Name;
        Foundation::Void* Address{};
        ImportKind Kind{};
    };

    struct ImportFailedInfo
    {
        StringView Module;
        StringView Name;
        Foundation::Uint16 Ordinal{};
        Foundation::Bool ByOrdinal{};
        ImportResolveFailure Reason{};
    };

    extern Event<ImportResolvedInfo> OnImportResolved;
    extern Event<ImportFailedInfo> OnImportFailed;

    class ImportResolver
    {
    public:
        ImportResolver() = default;

        auto AddFunction(StringView Module, StringView Name, Foundation::Void* Address) -> void;
        auto AddData(StringView Module, StringView Name, Foundation::Void* Address) -> void;

        [[nodiscard]] auto Resolve(StringView Module, StringView Name, Foundation::Uint16 Ordinal, Foundation::Bool ByOrdinal) const -> Foundation::Void*;

        [[nodiscard]] auto Contains(StringView Module, StringView Name) const -> Foundation::Bool;

        [[nodiscard]] auto SymbolCount() const -> Foundation::Size
        {
            return Symbols_.Size();
        }

        [[nodiscard]] auto Symbols() const -> Span<const ImportSymbol>
        {
            return Symbols_;
        }

    private:
        Vector<ImportSymbol> Symbols_{};
    };
}