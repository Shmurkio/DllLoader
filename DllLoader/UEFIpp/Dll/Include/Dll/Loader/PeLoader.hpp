#pragma once

#include <UEFIpp/UEFIpp.hpp>
#include <Dll/Loader/ImportResolver.hpp>

namespace Dll::Loader
{
    class PeLoader
    {
    public:
        PeLoader() = default;
        ~PeLoader();

    public:
        auto Load(const Foundation::Void* FileBuffer, const ImportResolver& Resolver) -> Foundation::Bool;

        auto Load(Span<const Foundation::Byte> FileBuffer, const ImportResolver& Resolver) -> Foundation::Bool
        {
            return Load(FileBuffer.Data(), Resolver);
        }

        auto Load(const Vector<Foundation::Byte>& FileBuffer, const ImportResolver& Resolver) -> Foundation::Bool
        {
            return Load(Span<const Foundation::Byte>{ FileBuffer }, Resolver);
        }

        auto Unload() -> void;

    public:
        [[nodiscard]] auto Image() const -> UEFIpp::Loader::PeImage;

        [[nodiscard]] auto Base() const -> Foundation::Void*;

        [[nodiscard]] auto Size() const -> Foundation::Size
        {
            return Size_;
        }

        [[nodiscard]] auto Export(StringView Name) const -> Foundation::Void*;

        [[nodiscard]] auto Loaded() const -> Foundation::Bool
        {
            return Base_ != nullptr;
        }

    public:
        PeLoader(const PeLoader&) = delete;
        auto operator=(const PeLoader&) -> PeLoader & = delete;

        PeLoader(PeLoader&& Other) noexcept;
        auto operator=(PeLoader&& Other) noexcept -> PeLoader&;

    private:
        Foundation::Uint8* Base_{};
        Foundation::Size Size_{};
    };
}