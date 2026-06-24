#include <Dll/Loader/ImportResolver.hpp>

namespace Dll::Loader
{
    Event<ImportResolvedInfo> OnImportResolved{};
    Event<ImportFailedInfo> OnImportFailed{};

    auto ImportResolver::AddFunction(StringView Module, StringView Name, Foundation::Void* Address) -> void
    {
        Symbols_.PushBack({ Module, Name, Address, ImportKind::Function });
    }

    auto ImportResolver::AddData(StringView Module, StringView Name, Foundation::Void* Address) -> void
    {
        Symbols_.PushBack({ Module, Name, Address, ImportKind::Data });
    }

    auto ImportResolver::Resolve(StringView Module, StringView Name, Foundation::Uint16 Ordinal, Foundation::Bool ByOrdinal) const -> Foundation::Void*
    {
        if (ByOrdinal)
        {
            OnImportFailed({ Module, Name, Ordinal, ByOrdinal, ImportResolveFailure::UnsupportedOrdinal });
            return nullptr;
        }

        for (const auto& Symbol : Symbols_)
        {
            if (Symbol.Module == Module && Symbol.Name == Name)
            {
				OnImportResolved({ Module, Name, Symbol.Address, Symbol.Kind });
                return Symbol.Address;
            }
        }

		OnImportFailed({ Module, Name, Ordinal, ByOrdinal, ImportResolveFailure::UnresolvedSymbol });

        return nullptr;
    }

    auto ImportResolver::Contains(StringView Module, StringView Name) const -> Foundation::Bool
    {
        for (const auto& Symbol : Symbols_)
        {
            if (Symbol.Module == Module && Symbol.Name == Name)
            {
                return true;
            }
        }

        return false;
    }
}