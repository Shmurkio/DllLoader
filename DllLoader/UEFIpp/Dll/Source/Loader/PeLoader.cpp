#include <Dll/Loader/PeLoader.hpp>

namespace
{
    struct BaseRelocationBlock
    {
        Foundation::Uint32 PageRva;
        Foundation::Uint32 BlockSize;
    };

    inline constexpr auto RelocationAbsolute = Foundation::Uint16{ 0 };
    inline constexpr auto RelocationHighLow = Foundation::Uint16{ 3 };
    inline constexpr auto RelocationDir64 = Foundation::Uint16{ 10 };

    [[nodiscard]] auto RawPointerFromRva(const UEFIpp::Loader::PeImage& RawImage, Foundation::Uint32 Rva) -> const Foundation::Void*
    {
        if (Rva == 0)
        {
            return nullptr;
        }

        const auto RawBase = Foundation::Cast::Auto<const Foundation::Uint8*>(RawImage.Base());

        if (Rva < RawImage.NtHeaders64()->OptionalHeader.SizeOfHeaders)
        {
            return RawBase + Rva;
        }

        for (const auto& Section : RawImage.Sections())
        {
            const auto Start = Section.Rva;
            const auto End = Start + Foundation::Utility::Max(Section.VirtualSize, Section.RawSize);

            if (Rva >= Start && Rva < End)
            {
                const auto Offset = Rva - Start;

                if (Offset >= Section.RawSize)
                {
                    return nullptr;
                }

                return RawBase + Section.RawOffset + Offset;
            }
        }

        return nullptr;
    }

    auto ApplyRelocations(Foundation::Uint8* Base, const UEFIpp::Loader::PeImage& Image, Foundation::Uint64 PreferredBase) -> Foundation::Bool
    {
        const auto ActualBase = Foundation::Cast::Auto<Foundation::Uint64>(Base);

        const auto Delta = ActualBase - PreferredBase;

        if (Delta == 0)
        {
            return true;
        }

        const auto Directory = Image.Directory(UEFIpp::Loader::Pe::DirectoryIndex::BaseRelocation);

        if (Directory.Empty())
        {
            return true;
        }

        auto* Block = Foundation::Cast::Auto<BaseRelocationBlock*>(Base + Directory.Rva());

        const auto* End = Base + Directory.Rva() + Directory.Size();

        while (Foundation::Cast::Auto<Foundation::Uint8*>(Block) < End && Block->BlockSize >= sizeof(BaseRelocationBlock))
        {
            const auto EntryCount = (Block->BlockSize - sizeof(BaseRelocationBlock)) / sizeof(Foundation::Uint16);

            auto* Entries = Foundation::Cast::Auto<Foundation::Uint16*>(Foundation::Cast::Auto<Foundation::Uint8*>(Block) + sizeof(BaseRelocationBlock));

            for (Foundation::Uint32 i = 0; i < EntryCount; ++i)
            {
                const auto Entry = Entries[i];
                const auto Type = Foundation::Cast::Auto<Foundation::Uint16>(Entry >> 12);

                const auto Offset = Foundation::Cast::Auto<Foundation::Uint16>(Entry & 0x0FFF);

                auto* PatchAddress = Base + Block->PageRva + Offset;

                switch (Type)
                {
                case RelocationAbsolute:
                {
                    break;
                }
                case RelocationDir64:
                {
                    auto* Value = Foundation::Cast::Auto<Foundation::Uint64*>(PatchAddress);
                    *Value += Delta;
                    break;
                }

                case RelocationHighLow:
                {
                    auto* Value = Foundation::Cast::Auto<Foundation::Uint32*>(PatchAddress);
                    *Value += Foundation::Cast::Auto<Foundation::Uint32>(Delta);
                    break;
                }

                default:
                {
                    return false;
                }
                }
            }

            Block = Foundation::Cast::Auto<BaseRelocationBlock*>(Foundation::Cast::Auto<Foundation::Uint8*>(Block) + Block->BlockSize);
        }

        return true;
    }

    auto ResolveImports(Foundation::Uint8* Base, const UEFIpp::Loader::PeImage& Image, const Dll::Loader::ImportResolver& Resolver) -> Foundation::Bool
    {
        for (const auto& Import : Image.Imports())
        {
            auto* Address = Resolver.Resolve(Import.Module.View(), Import.Name.View(), Import.Ordinal, Import.ImportByOrdinal);

            if (!Address)
            {
                return false;
            }

            const auto IatRva = Foundation::Cast::Auto<Foundation::Uint32>(Import.IatAddress - Image.BaseAddress());

            if (Image.Is64())
            {
                auto* Iat = Foundation::Cast::Auto<Foundation::Uint64*>(Base + IatRva);
                *Iat = Foundation::Cast::Auto<Foundation::Uint64>(Address);
            }
            else
            {
                auto* Iat = Foundation::Cast::Auto<Foundation::Uint32*>(Base + IatRva);
                *Iat = Foundation::Cast::Auto<Foundation::Uint32>(Foundation::Cast::Auto<Foundation::Uint64>(Address));
            }
        }

        return true;
    }
}

namespace Dll::Loader
{
    Event<LoadEventInfo> OnLoadEvent{};
    Event<LoadFailedInfo> OnLoadFailed{};

    PeLoader::~PeLoader()
    {
        Unload();
    }

    PeLoader::PeLoader(PeLoader&& Other) noexcept : Base_(Other.Base_), Size_(Other.Size_)
    {
        Other.Base_ = nullptr;
        Other.Size_ = 0;
    }

    auto PeLoader::operator=(PeLoader&& Other) noexcept -> PeLoader&
    {
        if (this != &Other)
        {
            Unload();

            Base_ = Other.Base_;
            Size_ = Other.Size_;

            Other.Base_ = nullptr;
            Other.Size_ = 0;
        }

        return *this;
    }

    auto PeLoader::Load(const Foundation::Void* FileBuffer, const ImportResolver& Resolver) -> Foundation::Bool
    {
        Unload();

        OnLoadEvent({ LoadStage::Begin, nullptr, 0 });

        if (!FileBuffer)
        {
            OnLoadFailed({ LoadStage::Begin, "FileBuffer is null" });
            return false;
        }

        UEFIpp::Loader::PeImage RawImage{ FileBuffer };

        if (!RawImage.IsValid() || !RawImage.Is64())
        {
            OnLoadFailed({ LoadStage::Begin, "Invalid or non-x64 PE image" });
            return false;
        }

        const auto* Nt = RawImage.NtHeaders64();
        const auto ImageSize = Nt->OptionalHeader.SizeOfImage;
        const auto HeaderSize = Nt->OptionalHeader.SizeOfHeaders;
        const auto PreferredBase = Nt->OptionalHeader.ImageBase;

        if (ImageSize == 0 || HeaderSize == 0)
        {
            OnLoadFailed({ LoadStage::Begin, "Invalid image/header size" });
            return false;
        }

        OnLoadEvent({ LoadStage::AllocateImage, nullptr, ImageSize });

        Base_ = new Foundation::Uint8[ImageSize];

        if (!Base_)
        {
            OnLoadFailed({ LoadStage::AllocateImage, "Image allocation failed" });
            return false;
        }

        Size_ = ImageSize;

        Memory::Zero(Base_, Size_);
        Memory::Copy(Base_, FileBuffer, HeaderSize);

        OnLoadEvent({ LoadStage::CopyHeaders, Base_, HeaderSize });

        for (const auto& Section : RawImage.Sections())
        {
            if (Section.RawSize == 0)
            {
                continue;
            }

            const auto* Source = RawPointerFromRva(RawImage, Section.Rva);

            if (!Source)
            {
                OnLoadFailed({ LoadStage::CopySections, "Failed to resolve raw section data" });
                Unload();
                return false;
            }

            auto* Destination = Base_ + Section.Rva;
            const auto CopySize = Foundation::Utility::Min(
                Section.RawSize,
                Section.VirtualSize ? Section.VirtualSize : Section.RawSize);

            Memory::Copy(Destination, Source, CopySize);
        }

        OnLoadEvent({ LoadStage::CopySections, Base_, Size_ });

        UEFIpp::Loader::PeImage MappedImage{ Base_ };

        if (!MappedImage.IsValid() || !MappedImage.Is64())
        {
            OnLoadFailed({ LoadStage::CopySections, "Mapped image is invalid" });
            Unload();
            return false;
        }

        OnLoadEvent({ LoadStage::ApplyRelocations, Base_, Size_ });

        if (!ApplyRelocations(Base_, MappedImage, PreferredBase))
        {
            OnLoadFailed({ LoadStage::ApplyRelocations, "Failed to apply relocations" });
            Unload();
            return false;
        }

        OnLoadEvent({ LoadStage::ResolveImports, Base_, Size_ });

        if (!ResolveImports(Base_, MappedImage, Resolver))
        {
            OnLoadFailed({ LoadStage::ResolveImports, "Failed to resolve imports" });
            Unload();
            return false;
        }

        OnLoadEvent({ LoadStage::Finished, Base_, Size_ });

        return true;
    }

    auto PeLoader::Unload() -> void
    {
        if (Base_)
        {
            OnLoadEvent({ LoadStage::Unload, Base_, Size_ });

            delete[] Base_;
            Base_ = nullptr;
        }

        Size_ = 0;
    }

    auto PeLoader::Image() const -> UEFIpp::Loader::PeImage
    {
        return UEFIpp::Loader::PeImage{ Base_ };
    }

    auto PeLoader::Base() const -> Foundation::Void*
    {
        return Base_;
    }

    auto PeLoader::Export(StringView Name) const -> Foundation::Void*
    {
        if (!Base_)
        {
            return nullptr;
        }

        const auto MappedImage = Image();
        const auto Export = MappedImage.Export(Name);

        if (!Export || Export->Forwarded)
        {
            return nullptr;
        }

        return Foundation::Cast::Auto<Foundation::Void*>(Base_ + Export->Rva);
    }
}