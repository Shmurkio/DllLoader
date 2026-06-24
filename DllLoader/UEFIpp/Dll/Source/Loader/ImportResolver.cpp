#include <Dll/Loader/ImportResolver.hpp>
#include <Dll/Runtime/Trace.hpp>

namespace Dll::Loader
{
    auto ImportResolver::AddFunction(StringView Module, StringView Name, Foundation::Void* Address) -> void
    {
        DLL_IMPORT_TRACE_VERBOSE("Registered function import", Module, Name, Address);
        Symbols_.PushBack({ Module, Name, Address, ImportKind::Function });
    }

    auto ImportResolver::AddData(StringView Module, StringView Name, Foundation::Void* Address) -> void
    {
        DLL_IMPORT_TRACE_VERBOSE("Registered data import", Module, Name, Address);
        Symbols_.PushBack({ Module, Name, Address, ImportKind::Data });
    }

    auto ImportResolver::Resolve(StringView Module, StringView Name, Foundation::Uint16 Ordinal, Foundation::Bool ByOrdinal) const -> Foundation::Void*
    {
        if (ByOrdinal)
        {
            DLL_IMPORT_TRACE_ERROR("Import failed: ordinal imports are not supported", Module, Name, Ordinal);
            return nullptr;
        }

        for (const auto& Symbol : Symbols_)
        {
            if (Symbol.Module == Module && Symbol.Name == Name)
            {
                DLL_IMPORT_TRACE_INFO("Resolved import", Module, Name, Symbol.Address);
                return Symbol.Address;
            }
        }

        DLL_IMPORT_TRACE_ERROR("Import failed: unresolved symbol", Module, Name, 0);
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