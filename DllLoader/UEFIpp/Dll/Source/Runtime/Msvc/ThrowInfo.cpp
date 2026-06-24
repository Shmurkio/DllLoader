#include <Dll/Runtime/Msvc/ThrowInfo.hpp>

namespace Dll::Runtime::Msvc
{
    auto TypeNameFromDescriptor(Foundation::Uint8* Base, Foundation::Uint32 TypeDescriptorRva) -> const Foundation::Char*
    {
        auto* TypeDescriptor = Base + TypeDescriptorRva;
        return Foundation::Cast::Auto<const Foundation::Char*>(TypeDescriptor + 16);
    }
}