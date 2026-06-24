#include <Dll/Runtime/Msvc/UnwindInfo.hpp>
#include <UEFIpp/Loader/PeImage.hpp>

namespace Dll::Runtime::Msvc
{
    auto LookupRuntimeFunction(Foundation::Uint8* Base, Foundation::Uint64 Rip) -> RuntimeFunction*
    {
        const auto Rva = Foundation::Cast::Auto<Foundation::Uint32>(Rip - Foundation::Cast::Auto<Foundation::Uint64>(Base));

        auto Image = UEFIpp::Loader::PeImage{ Base };
        auto Dir = Image.Directory(UEFIpp::Loader::Pe::DirectoryIndex::Exception);

        auto* Functions = Foundation::Cast::Auto<RuntimeFunction*>(Base + Dir.Rva());
        const auto Count = Dir.Size() / sizeof(RuntimeFunction);

        for (Foundation::Size i = 0; i < Count; ++i)
        {
            if (Rva >= Functions[i].BeginAddress && Rva < Functions[i].EndAddress)
            {
                return &Functions[i];
            }
        }

        return nullptr;
    }

    auto HandlerInfoFromUnwindInfo(Foundation::Uint8* UnwindInfo) -> HandlerInfo*
    {
        auto* Header = Foundation::Cast::Auto<UnwindInfoHeader*>(UnwindInfo);
        auto Offset = sizeof(UnwindInfoHeader) + Header->CountOfCodes * sizeof(Foundation::Uint16);
        Offset = (Offset + 3) & ~3;

        return Foundation::Cast::Auto<HandlerInfo*>(UnwindInfo + Offset);
    }
}