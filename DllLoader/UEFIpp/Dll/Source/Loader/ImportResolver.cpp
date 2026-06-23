#include <Dll/Loader/ImportResolver.hpp>

namespace Dll::Loader
{
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
            Stream::Out::Console
                << "Unsupported ordinal import: " << Module << " Ordinal=" << Ordinal << Stream::Endl;

            return nullptr;
        }

        for (const auto& Symbol : Symbols_)
        {
            if (Symbol.Module == Module && Symbol.Name == Name)
            {
                return Symbol.Address;
            }
        }

        Stream::Out::Console
            << "Unresolved import: " << Module << "!" << Name << Stream::Endl;

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