#include "pch.h"
#include "resource.h"

#pragma comment(lib, "Comctl32.lib")

_COM_SMARTPTR_TYPEDEF(IWICImagingFactory, __uuidof(IWICImagingFactory));
_COM_SMARTPTR_TYPEDEF(IWICComponentInfo, __uuidof(IWICComponentInfo));
_COM_SMARTPTR_TYPEDEF(IWICBitmapCodecInfo, __uuidof(IWICBitmapCodecInfo));
_COM_SMARTPTR_TYPEDEF(IWICColorContext, __uuidof(IWICColorContext));
_COM_SMARTPTR_TYPEDEF(IWICFormatConverterInfo, __uuidof(IWICFormatConverterInfo));
_COM_SMARTPTR_TYPEDEF(IWICPixelFormatInfo, __uuidof(IWICPixelFormatInfo));
_COM_SMARTPTR_TYPEDEF(IWICPixelFormatInfo2, __uuidof(IWICPixelFormatInfo2));
_COM_SMARTPTR_TYPEDEF(IWICBitmapDecoderInfo, __uuidof(IWICBitmapDecoderInfo));
_COM_SMARTPTR_TYPEDEF(IWICBitmapEncoderInfo, __uuidof(IWICBitmapEncoderInfo));
_COM_SMARTPTR_TYPEDEF(IWICMetadataHandlerInfo, __uuidof(IWICMetadataHandlerInfo));
_COM_SMARTPTR_TYPEDEF(IWICMetadataReaderInfo, __uuidof(IWICMetadataReaderInfo));
_COM_SMARTPTR_TYPEDEF(IWICMetadataWriterInfo, __uuidof(IWICMetadataWriterInfo));

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

struct component_info
{
    std::wstring name;
    WICComponentType componentType;
    std::map<std::wstring, std::wstring> properties;
};

HINSTANCE g_hInst;
std::vector<component_info> g_currentlist;

_Check_return_
std::wstring guidToString(REFGUID guid)
{
    WCHAR buf[64]{};
    int len = StringFromGUID2(guid, buf, ARRAYSIZE(buf));
    return len > 0 ? buf : std::wstring{};
}

_Check_return_
std::wstring hrToString(HRESULT hr)
{
    WCHAR buf[32]{};
    swprintf(buf, ARRAYSIZE(buf), L"error:0x%08X", hr);
    return buf;
}

_Check_return_
std::wstring boolToString(BOOL value)
{
    return value ? L"true"s : L"false"s;
}

_Check_return_
std::wstring bytesToString(const BYTE* bytes, UINT size)
{
    std::wstringstream strstream;
    strstream.flags(std::ios_base::showbase | std::ios_base::hex | std::ios_base::showpoint);

    for (UINT i = 0; i < size; i++)
    {
        if (i > 0)
            strstream << L",";
        strstream << bytes[i];
    }

    return strstream.str();
}

#define GET_STRING_PROPERTY(pWICComponentInfo, name, map) \
{ \
    WCHAR buf[512]{}; \
    UINT bufLength{}; \
    hr = (pWICComponentInfo)->Get ## name(ARRAYSIZE(buf), buf, &bufLength); \
    map.emplace(TEXT( # name), SUCCEEDED(hr) ? std::wstring{ buf, (bufLength > 0 ? bufLength - 1 : 0) } : hrToString(hr)); \
}

#define GET_BOOLEAN_PROPERTY(pWICComponentInfo, name, map) \
{ \
    BOOL value{}; \
    hr = (pWICComponentInfo)-> ## name(&value); \
    map.emplace(TEXT( # name), SUCCEEDED(hr) ? boolToString(value) : hrToString(hr)); \
}

_Check_return_
std::wstring getWellKnownContainerName(REFGUID guid)
{
    return
        (guid == GUID_ContainerFormatBmp) ? L"Bmp"s :
        (guid == GUID_ContainerFormatPng) ? L"Png"s :
        (guid == GUID_ContainerFormatIco) ? L"Ico"s :
        (guid == GUID_ContainerFormatJpeg) ? L"Jpeg"s :
        (guid == GUID_ContainerFormatTiff) ? L"Tiff"s :
        (guid == GUID_ContainerFormatGif) ? L"Gif"s :
        (guid == GUID_ContainerFormatWmp) ? L"Wmp"s :
        (guid == GUID_ContainerFormatDds) ? L"Dds"s :
        (guid == GUID_ContainerFormatAdng) ? L"Adng"s :
        (guid == GUID_ContainerFormatHeif) ? L"Heif"s :
        (guid == GUID_ContainerFormatWebp) ? L"Webp"s :
        (guid == GUID_ContainerFormatRaw) ? L"Raw"s :
        guidToString(guid);

}

_Check_return_
std::wstring getWellKnownPixelFormatName(REFWICPixelFormatGUID guid)
{
    return
        (guid == GUID_WICPixelFormatDontCare) ? L"DontCare"s :
        (guid == GUID_WICPixelFormat1bppIndexed) ? L"1bppIndexed"s :
        (guid == GUID_WICPixelFormat2bppIndexed) ? L"2bppIndexed"s :
        (guid == GUID_WICPixelFormat4bppIndexed) ? L"4bppIndexed"s :
        (guid == GUID_WICPixelFormat8bppIndexed) ? L"8bppIndexed"s :
        (guid == GUID_WICPixelFormatBlackWhite) ? L"BlackWhite"s :
        (guid == GUID_WICPixelFormat2bppGray) ? L"2bppGray"s :
        (guid == GUID_WICPixelFormat4bppGray) ? L"4bppGray"s :
        (guid == GUID_WICPixelFormat8bppGray) ? L"8bppGray"s :
        (guid == GUID_WICPixelFormat8bppAlpha) ? L"8bppAlpha"s :
        (guid == GUID_WICPixelFormat16bppBGR555) ? L"16bppBGR555"s :
        (guid == GUID_WICPixelFormat16bppBGR565) ? L"16bppBGR565"s :
        (guid == GUID_WICPixelFormat16bppBGRA5551) ? L"16bppBGRA5551"s :
        (guid == GUID_WICPixelFormat16bppGray) ? L"16bppGray"s :
        (guid == GUID_WICPixelFormat24bppBGR) ? L"24bppBGR"s :
        (guid == GUID_WICPixelFormat24bppRGB) ? L"24bppRGB"s :
        (guid == GUID_WICPixelFormat32bppBGR) ? L"32bppBGR"s :
        (guid == GUID_WICPixelFormat32bppBGRA) ? L"32bppBGRA"s :
        (guid == GUID_WICPixelFormat32bppPBGRA) ? L"32bppPBGRA"s :
        (guid == GUID_WICPixelFormat32bppGrayFloat) ? L"32bppGrayFloat"s :
        //(guid == GUID_WICPixelFormat32bppRGB) ? L"32bppRGB"s :
        (guid == GUID_WICPixelFormat32bppRGBA) ? L"32bppRGBA"s :
        (guid == GUID_WICPixelFormat32bppPRGBA) ? L"32bppPRGBA"s :
        (guid == GUID_WICPixelFormat48bppRGB) ? L"48bppRGB"s :
        (guid == GUID_WICPixelFormat48bppBGR) ? L"48bppBGR"s :
        //(guid == GUID_WICPixelFormat64bppRGB) ? L"64bppRGB"s :
        (guid == GUID_WICPixelFormat64bppRGBA) ? L"64bppRGBA"s :
        (guid == GUID_WICPixelFormat64bppBGRA) ? L"64bppBGRA"s :
        (guid == GUID_WICPixelFormat64bppPRGBA) ? L"64bppPRGBA"s :
        (guid == GUID_WICPixelFormat64bppPBGRA) ? L"64bppPBGRA"s :
        (guid == GUID_WICPixelFormat16bppGrayFixedPoint) ? L"16bppGrayFixedPoint"s :
        (guid == GUID_WICPixelFormat32bppBGR101010) ? L"32bppBGR101010"s :
        (guid == GUID_WICPixelFormat48bppRGBFixedPoint) ? L"48bppRGBFixedPoint"s :
        (guid == GUID_WICPixelFormat48bppBGRFixedPoint) ? L"48bppBGRFixedPoint"s :
        (guid == GUID_WICPixelFormat96bppRGBFixedPoint) ? L"96bppRGBFixedPoint"s :
        //(guid == GUID_WICPixelFormat96bppRGBFloat) ? L"96bppRGBFloat"s :
        (guid == GUID_WICPixelFormat128bppRGBAFloat) ? L"128bppRGBAFloat"s :
        (guid == GUID_WICPixelFormat128bppPRGBAFloat) ? L"128bppPRGBAFloat"s :
        (guid == GUID_WICPixelFormat128bppRGBFloat) ? L"128bppRGBFloat"s :
        (guid == GUID_WICPixelFormat32bppCMYK) ? L"32bppCMYK"s :
        (guid == GUID_WICPixelFormat64bppRGBAFixedPoint) ? L"64bppRGBAFixedPoint"s :
        (guid == GUID_WICPixelFormat64bppBGRAFixedPoint) ? L"64bppBGRAFixedPoint"s :
        (guid == GUID_WICPixelFormat64bppRGBFixedPoint) ? L"64bppRGBFixedPoint"s :
        (guid == GUID_WICPixelFormat128bppRGBAFixedPoint) ? L"128bppRGBAFixedPoint"s :
        (guid == GUID_WICPixelFormat128bppRGBFixedPoint) ? L"128bppRGBFixedPoint"s :
        (guid == GUID_WICPixelFormat64bppRGBAHalf) ? L"64bppRGBAHalf"s :
        //(guid == GUID_WICPixelFormat64bppPRGBAHalf) ? L"64bppPRGBAHalf"s :
        (guid == GUID_WICPixelFormat64bppRGBHalf) ? L"64bppRGBHalf"s :
        (guid == GUID_WICPixelFormat48bppRGBHalf) ? L"48bppRGBHalf"s :
        (guid == GUID_WICPixelFormat32bppRGBE) ? L"32bppRGBE"s :
        (guid == GUID_WICPixelFormat16bppGrayHalf) ? L"16bppGrayHalf"s :
        (guid == GUID_WICPixelFormat32bppGrayFixedPoint) ? L"32bppGrayFixedPoint"s :
        (guid == GUID_WICPixelFormat32bppRGBA1010102) ? L"32bppRGBA1010102"s :
        (guid == GUID_WICPixelFormat32bppRGBA1010102XR) ? L"32bppRGBA1010102XR"s :
        (guid == GUID_WICPixelFormat32bppR10G10B10A2) ? L"32bppR10G10B10A2"s :
        (guid == GUID_WICPixelFormat32bppR10G10B10A2HDR10) ? L"32bppR10G10B10A2HDR10"s :
        (guid == GUID_WICPixelFormat64bppCMYK) ? L"64bppCMYK"s :
        (guid == GUID_WICPixelFormat24bpp3Channels) ? L"24bpp3Channels"s :
        (guid == GUID_WICPixelFormat32bpp4Channels) ? L"32bpp4Channels"s :
        (guid == GUID_WICPixelFormat40bpp5Channels) ? L"40bpp5Channels"s :
        (guid == GUID_WICPixelFormat48bpp6Channels) ? L"48bpp6Channels"s :
        (guid == GUID_WICPixelFormat56bpp7Channels) ? L"56bpp7Channels"s :
        (guid == GUID_WICPixelFormat64bpp8Channels) ? L"64bpp8Channels"s :
        (guid == GUID_WICPixelFormat48bpp3Channels) ? L"48bpp3Channels"s :
        (guid == GUID_WICPixelFormat64bpp4Channels) ? L"64bpp4Channels"s :
        (guid == GUID_WICPixelFormat80bpp5Channels) ? L"80bpp5Channels"s :
        (guid == GUID_WICPixelFormat96bpp6Channels) ? L"96bpp6Channels"s :
        (guid == GUID_WICPixelFormat112bpp7Channels) ? L"112bpp7Channels"s :
        (guid == GUID_WICPixelFormat128bpp8Channels) ? L"128bpp8Channels"s :
        (guid == GUID_WICPixelFormat40bppCMYKAlpha) ? L"40bppCMYKAlpha"s :
        (guid == GUID_WICPixelFormat80bppCMYKAlpha) ? L"80bppCMYKAlpha"s :
        (guid == GUID_WICPixelFormat32bpp3ChannelsAlpha) ? L"32bpp3ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat40bpp4ChannelsAlpha) ? L"40bpp4ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat48bpp5ChannelsAlpha) ? L"48bpp5ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat56bpp6ChannelsAlpha) ? L"56bpp6ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat64bpp7ChannelsAlpha) ? L"64bpp7ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat72bpp8ChannelsAlpha) ? L"72bpp8ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat64bpp3ChannelsAlpha) ? L"64bpp3ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat80bpp4ChannelsAlpha) ? L"80bpp4ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat96bpp5ChannelsAlpha) ? L"96bpp5ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat112bpp6ChannelsAlpha) ? L"112bpp6ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat128bpp7ChannelsAlpha) ? L"128bpp7ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat144bpp8ChannelsAlpha) ? L"144bpp8ChannelsAlpha"s :
        (guid == GUID_WICPixelFormat8bppY) ? L"8bppY"s :
        (guid == GUID_WICPixelFormat8bppCb) ? L"8bppCb"s :
        (guid == GUID_WICPixelFormat8bppCr) ? L"8bppCr"s :
        (guid == GUID_WICPixelFormat16bppCbCr) ? L"16bppCbCr"s :
        (guid == GUID_WICPixelFormat16bppYQuantizedDctCoefficients) ? L"16bppYQuantizedDctCoefficients"s :
        (guid == GUID_WICPixelFormat16bppCbQuantizedDctCoefficients) ? L"16bppCbQuantizedDctCoefficients"s :
        (guid == GUID_WICPixelFormat16bppCrQuantizedDctCoefficients) ? L"16bppCrQuantizedDctCoefficients"s :
        guidToString(guid);
}

_Check_return_
std::wstring findModulePath(const void* address)
{
    auto deleter = [](HANDLE handle) { CloseHandle(handle); };
    std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(deleter)> hSnap{
        CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId()), deleter };

    MODULEENTRY32 me{ sizeof(MODULEENTRY32) };
    if (Module32First(hSnap.get(), &me))
    {
        do
        {
            if (me.modBaseAddr <= address && address <= me.modBaseAddr + me.modBaseSize)
            {
#if _M_IX86
                std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(deleter)> hFile{
                    CreateFile(me.szExePath, 0/*QUERY*/, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr), deleter
                };

                WCHAR path[MAX_PATH*2]{};
                GetFinalPathNameByHandle(hFile.get(), path, ARRAYSIZE(path), FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
                return path;
#else
                return me.szExePath;
#endif
            }
        } while (Module32Next(hSnap.get(), &me));
    }

    return {};
}

_Check_return_
component_info getCommonComponentInfo(IWICComponentInfo* pWICComponentInfo)
{
    HRESULT hr;

    WICComponentType componentType{};
    hr = pWICComponentInfo->GetComponentType(&componentType);

    WCHAR friendlyName[512]{};
    UINT friendlyNameLength{};
    hr = pWICComponentInfo->GetFriendlyName(ARRAYSIZE(friendlyName), friendlyName, &friendlyNameLength);

    component_info info{ friendlyName, componentType };

    GET_STRING_PROPERTY(pWICComponentInfo, Author, info.properties);
    {
        CLSID clsid{};
        hr = pWICComponentInfo->GetCLSID(&clsid);
        info.properties.emplace(L"CLSID"s, SUCCEEDED(hr) ? guidToString(clsid) : hrToString(hr));
    }
    {
        DWORD signingStatus;
        hr = pWICComponentInfo->GetSigningStatus(&signingStatus);
        info.properties.emplace(L"SigningStatus"s, SUCCEEDED(hr) ? std::to_wstring(signingStatus) : hrToString(hr));
    }
    GET_STRING_PROPERTY(pWICComponentInfo, SpecVersion, info.properties);
    GET_STRING_PROPERTY(pWICComponentInfo, Version, info.properties);
    {
        GUID vendorGUID{};
        hr = pWICComponentInfo->GetVendorGUID(&vendorGUID);
        info.properties.emplace(L"VendorGUID"s, SUCCEEDED(hr) ? guidToString(vendorGUID) : hrToString(hr));
    }

    return info;
}

_Check_return_
component_info getPixelFormatInfo(IWICComponentInfo* pWICComponentInfo)
{
    HRESULT hr;
    component_info info = getCommonComponentInfo(pWICComponentInfo);

    IWICPixelFormatInfoPtr pWICPixelFormatInfo;
    hr = pWICComponentInfo->QueryInterface(&pWICPixelFormatInfo);

    GUID formatGUID{};
    hr = pWICPixelFormatInfo->GetFormatGUID(&formatGUID);

    IWICColorContextPtr pWICColorContext;
    hr = pWICPixelFormatInfo->GetColorContext(&pWICColorContext);
    if (SUCCEEDED(hr))
    {
        WICColorContextType Type{};
        hr = pWICColorContext->GetType(&Type);

        auto toString = [](WICColorContextType type) { return
            type == WICColorContextUninitialized ? L"Uninitialized"s :
            type == WICColorContextProfile ? L"Profile"s :
            type == WICColorContextExifColorSpace ? L"ExifColorSpace"s :
            L"Unknown"s;
        };

        info.properties.emplace(L"ColorContext.Type"s, SUCCEEDED(hr) ? toString(Type) : hrToString(hr));

        UINT ProfileBytes{};
        hr = pWICColorContext->GetProfileBytes(0, nullptr, &ProfileBytes);
        info.properties.emplace(L"ColorContext.ProfileBytes"s, SUCCEEDED(hr) ? std::to_wstring(ProfileBytes) : hrToString(hr));

        if (Type == WICColorContextExifColorSpace) {
            UINT ExifColorSpace{};
            hr = pWICColorContext->GetExifColorSpace(&ExifColorSpace);
            info.properties.emplace(L"ColorContext.ExifColorSpace"s, SUCCEEDED(hr) ? std::to_wstring(ExifColorSpace) : hrToString(hr));
        }
    }
    UINT BitsPerPixel{};
    hr = pWICPixelFormatInfo->GetBitsPerPixel(&BitsPerPixel);
    info.properties.emplace(L"BitsPerPixel"s, SUCCEEDED(hr) ? std::to_wstring(BitsPerPixel) : hrToString(hr));

    UINT ChannelCount{};
    hr = pWICPixelFormatInfo->GetChannelCount(&ChannelCount);
    info.properties.emplace(L"ChannelCount"s, SUCCEEDED(hr) ? std::to_wstring(ChannelCount) : hrToString(hr));
    std::wstring tmp;
    for (UINT i = 0; i < ChannelCount; i++)
    {
        BYTE MaskBuffer[64]{};
        UINT MaskBufferLength{};
        hr = pWICPixelFormatInfo->GetChannelMask(i, ARRAYSIZE(MaskBuffer), MaskBuffer, &MaskBufferLength);
        tmp += L"["s + bytesToString(MaskBuffer, MaskBufferLength).append(L"],");
    }
    info.properties.emplace(L"ChannelMask"s, SUCCEEDED(hr) ? tmp : hrToString(hr));

    IWICPixelFormatInfo2Ptr pWICPixelFormatInfo2;
    if (SUCCEEDED(pWICComponentInfo->QueryInterface(&pWICPixelFormatInfo2)))
    {
        GET_BOOLEAN_PROPERTY(pWICPixelFormatInfo2, SupportsTransparency, info.properties);

        WICPixelFormatNumericRepresentation NumericRepresentation{};
        hr = pWICPixelFormatInfo2->GetNumericRepresentation(&NumericRepresentation);

        auto name = (NumericRepresentation == WICPixelFormatNumericRepresentationUnspecified) ? L"Unspecified"s :
            (NumericRepresentation == WICPixelFormatNumericRepresentationIndexed) ? L"Indexed"s :
            (NumericRepresentation == WICPixelFormatNumericRepresentationUnsignedInteger) ? L"UnsignedInteger"s :
            (NumericRepresentation == WICPixelFormatNumericRepresentationSignedInteger) ? L"SignedInteger"s :
            (NumericRepresentation == WICPixelFormatNumericRepresentationFixed) ? L"Fixed"s :
            (NumericRepresentation == WICPixelFormatNumericRepresentationFloat) ? L"Float"s :
            L"Unkown";
        info.properties.emplace(L"NumericRepresentation"s, SUCCEEDED(hr) ? name : hrToString(hr));
    }

    return info;
}
_Check_return_
component_info getConverterInfo(IWICComponentInfo* pWICComponentInfo)
{
    HRESULT hr;
    component_info info = getCommonComponentInfo(pWICComponentInfo);

    IWICFormatConverterInfoPtr pWICFormatConverterInfo;
    hr = pWICComponentInfo->QueryInterface(&pWICFormatConverterInfo);
    {
        WICPixelFormatGUID guids[512]{};
        UINT guidsLength;
        hr = pWICFormatConverterInfo->GetPixelFormats(ARRAYSIZE(guids), guids, &guidsLength);
        std::wstring tmp;
        for (UINT i = 0; i < guidsLength; i++)
        {
            tmp.append(getWellKnownPixelFormatName(guids[i])).append(L",");
        }
        info.properties.emplace(L"PixelFormats"s, SUCCEEDED(hr) ? tmp : hrToString(hr));
    }
    {
        IWICFormatConverter* instance{};
        hr = pWICFormatConverterInfo->CreateInstance(&instance);
        if (instance) {
            info.properties.emplace(L"Path"s, findModulePath((*(void**)(instance))));
            instance->Release();
        }
    }
    return info;
}

_Check_return_
component_info getCodecInfo(IWICComponentInfo* pWICComponentInfo)
{
    HRESULT hr;
    component_info info = getCommonComponentInfo(pWICComponentInfo);

    IWICBitmapCodecInfoPtr pWICBitmapCodecInfo;
    hr = pWICComponentInfo->QueryInterface(&pWICBitmapCodecInfo);

    {
        GUID ContainerFormat{};
        hr = pWICBitmapCodecInfo->GetContainerFormat(&ContainerFormat);
        info.properties.emplace(L"ContainerFormat"s, SUCCEEDED(hr) ? getWellKnownContainerName(ContainerFormat) : hrToString(hr));
    }
    {
        WICPixelFormatGUID guids[512]{};
        UINT guidsLength;
        hr = pWICBitmapCodecInfo->GetPixelFormats(ARRAYSIZE(guids), guids, &guidsLength);
        std::wstring tmp;
        for (UINT i = 0; i < guidsLength; i++)
        {
            tmp.append(getWellKnownPixelFormatName(guids[i])).append(L",");
        }
        info.properties.emplace(L"PixelFormats"s, SUCCEEDED(hr) ? tmp : hrToString(hr));
    }

    GET_STRING_PROPERTY(pWICBitmapCodecInfo, ColorManagementVersion, info.properties);
    GET_STRING_PROPERTY(pWICBitmapCodecInfo, DeviceManufacturer, info.properties);
    GET_STRING_PROPERTY(pWICBitmapCodecInfo, DeviceModels, info.properties);
    GET_STRING_PROPERTY(pWICBitmapCodecInfo, MimeTypes, info.properties);
    GET_STRING_PROPERTY(pWICBitmapCodecInfo, FileExtensions, info.properties);
    GET_BOOLEAN_PROPERTY(pWICBitmapCodecInfo, DoesSupportAnimation, info.properties);
    GET_BOOLEAN_PROPERTY(pWICBitmapCodecInfo, DoesSupportChromakey, info.properties);
    GET_BOOLEAN_PROPERTY(pWICBitmapCodecInfo, DoesSupportLossless, info.properties);
    GET_BOOLEAN_PROPERTY(pWICBitmapCodecInfo, DoesSupportMultiframe, info.properties);

    return info;
}

_Check_return_
component_info getDecoderInfo(IWICComponentInfo* pWICComponentInfo)
{
    HRESULT hr;
    component_info info = getCodecInfo(pWICComponentInfo);

    IWICBitmapDecoderInfoPtr pWICBitmapCodecInfo;
    hr = pWICComponentInfo->QueryInterface(&pWICBitmapCodecInfo);

    {
        UINT PatternCount;
        UINT sizePatterns;
        hr = pWICBitmapCodecInfo->GetPatterns(0, nullptr, &PatternCount, &sizePatterns);
        auto buf{ std::make_unique<BYTE[]>(sizePatterns) };
        WICBitmapPattern* Patterns{ reinterpret_cast<WICBitmapPattern*>(buf.get()) };
        hr = pWICBitmapCodecInfo->GetPatterns(sizePatterns, Patterns, &PatternCount, &sizePatterns);
        for (UINT i = 0; i < PatternCount; i++)
        {
            auto tmp{ L"Position: "s + std::to_wstring(Patterns[i].Position.QuadPart) +
                L",EndOfStream: "s + boolToString(Patterns[i].EndOfStream) +
                L",Pattern: ["s + bytesToString(Patterns[i].Pattern, Patterns[i].Length) + L"]"s +
                L",Mask: ["s + bytesToString(Patterns[i].Mask, Patterns[i].Length) + L"]"s };
            info.properties.emplace(L"Patterns"s + std::to_wstring(i), SUCCEEDED(hr) ? tmp : hrToString(hr));
        }
    }
    {
        IWICBitmapDecoder* instance{};
        hr = pWICBitmapCodecInfo->CreateInstance(&instance);
        if (instance) {
            info.properties.emplace(L"Path"s, findModulePath((*(void**)(instance))));
            instance->Release();
        }
    }
    return info;
}

_Check_return_
component_info getEncoderInfo(IWICComponentInfo* pWICComponentInfo)
{
    HRESULT hr;
    component_info info = getCodecInfo(pWICComponentInfo);
    IWICBitmapEncoderInfoPtr pWICBitmapEncoderInfo;
    hr = pWICComponentInfo->QueryInterface(&pWICBitmapEncoderInfo);

    {
        IWICBitmapEncoder* instance{};
        hr = pWICBitmapEncoderInfo->CreateInstance(&instance);
        if (instance) {
            info.properties.emplace(L"Path"s, findModulePath((*(void**)(instance))));
            instance->Release();
        }
    }
    return info;
}

_Check_return_
component_info getMetadataHandler(IWICComponentInfo* pWICComponentInfo)
{
    HRESULT hr;
    component_info info = getCommonComponentInfo(pWICComponentInfo);

    IWICMetadataHandlerInfoPtr pWICMetadataHandlerInfo;
    hr = pWICComponentInfo->QueryInterface(&pWICMetadataHandlerInfo);

    GUID MetadataFormat{};
    hr = pWICMetadataHandlerInfo->GetMetadataFormat(&MetadataFormat);
    info.properties.emplace(L"MetadataFormat"s, SUCCEEDED(hr) ? guidToString(MetadataFormat) : hrToString(hr));

    WICPixelFormatGUID guids[512]{};
    UINT guidsLength;
    hr = pWICMetadataHandlerInfo->GetContainerFormats(ARRAYSIZE(guids), guids, &guidsLength);
    std::wstring tmp;
    for (UINT i = 0; i < guidsLength; i++)
    {
        tmp.append(getWellKnownContainerName(guids[i])).append(L",");
    }
    info.properties.emplace(L"ContainerFormats"s, SUCCEEDED(hr) ? tmp : hrToString(hr));

    GET_STRING_PROPERTY(pWICMetadataHandlerInfo, DeviceManufacturer, info.properties);
    GET_STRING_PROPERTY(pWICMetadataHandlerInfo, DeviceModels, info.properties);

    GET_BOOLEAN_PROPERTY(pWICMetadataHandlerInfo, DoesRequireFullStream, info.properties);
    GET_BOOLEAN_PROPERTY(pWICMetadataHandlerInfo, DoesSupportPadding, info.properties);
    GET_BOOLEAN_PROPERTY(pWICMetadataHandlerInfo, DoesRequireFixedSize, info.properties);

    return info;
}

component_info getMetadataReaderInfo(IWICComponentInfo* pWICComponentInfo)
{
    HRESULT hr;
    component_info info = getMetadataHandler(pWICComponentInfo);
    IWICMetadataReaderInfoPtr pWICMetadataReaderInfo;
    hr = pWICComponentInfo->QueryInterface(&pWICMetadataReaderInfo);

    {
        IWICMetadataReader* instance{};
        hr = pWICMetadataReaderInfo->CreateInstance(&instance);
        if (instance) {
            info.properties.emplace(L"Path"s, findModulePath((*(void**)(instance))));
            instance->Release();
        }
    }
    return info;
}

component_info getMetadataWriterInfo(IWICComponentInfo* pWICComponentInfo)
{
    HRESULT hr;
    component_info info = getMetadataHandler(pWICComponentInfo);
    IWICMetadataWriterInfoPtr pWICMetadataWriterInfo;
    hr = pWICComponentInfo->QueryInterface(&pWICMetadataWriterInfo);

    {
        IWICMetadataWriter* instance{};
        hr = pWICMetadataWriterInfo->CreateInstance(&instance);
        if (instance) {
            info.properties.emplace(L"Path"s, findModulePath((*(void**)(instance))));
            instance->Release();
        }
    }
    return info;
}

static constexpr struct { WICComponentType type; LPCWSTR name; } g_table[] =
{
    {WICDecoder, L"Decoder"},
    {WICEncoder, L"Encoder"},
    {WICPixelFormatConverter, L"PixelFormatConverter"},
    {WICMetadataReader,L"MetadataReader"},
    {WICMetadataWriter, L"MetadataWriter"},
    {WICPixelFormat, L"PixelFormat"},
    {WICAllComponents, L"All" },
};

LPCWSTR getComponentName(WICComponentType type)
{
    for (auto& itr : g_table)
    {
        if (type == itr.type)
            return itr.name;
    }
    return nullptr;
}

static constexpr struct { LPCWSTR text; int cx; int fmt; LPCWSTR(*lamda)(const component_info&); } g_listview_columns[] = {
    {L"Name", 200, LVCFMT_LEFT, [](const component_info& info) { return info.name.c_str(); }},
    {L"Type",  80, LVCFMT_LEFT, [](const component_info& info) { return getComponentName(info.componentType); } },
};

HRESULT getComponentInfo(WICComponentType requiredType, std::vector<component_info>& list)
{
    HRESULT hr;
    IWICImagingFactoryPtr pWICImagingFactory;
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWICImagingFactory));
    if (FAILED(hr)) return hr;

    IEnumUnknownPtr pEnumUnknownPtr;
    hr = pWICImagingFactory->CreateComponentEnumerator(requiredType, WICComponentEnumerateRefresh, &pEnumUnknownPtr);
    if (FAILED(hr)) return hr;
    IUnknownPtr pUnknownPtr;
    while (S_OK == (pEnumUnknownPtr->Next(1, &pUnknownPtr, nullptr)))
    {
        IWICComponentInfoPtr pWICComponentInfo;
        hr = pUnknownPtr->QueryInterface(&pWICComponentInfo);
        if (FAILED(hr)) continue;

        WICComponentType componentType{};
        hr = pWICComponentInfo->GetComponentType(&componentType);
        if (FAILED(hr)) continue;

        auto info =
            componentType == WICDecoder ? getDecoderInfo(pWICComponentInfo) :
            componentType == WICEncoder ? getEncoderInfo(pWICComponentInfo) :
            componentType == WICPixelFormatConverter ? getConverterInfo(pWICComponentInfo) :
            componentType == WICMetadataReader ? getMetadataReaderInfo(pWICComponentInfo) :
            componentType == WICMetadataWriter ? getMetadataWriterInfo(pWICComponentInfo) :
            componentType == WICPixelFormat ? getPixelFormatInfo(pWICComponentInfo) :
            component_info{}; // TODO
        list.emplace_back(info);
        if (info.name.empty())
            abort();
    }
    return hr;
}

std::wstring getComponentDetail(const component_info& info)
{
    std::wstringstream strstream;
    for (auto& itr : info.properties)
    {
        strstream << itr.first << L": " << itr.second << L"\r\n";
    }
    return strstream.str();
}

void errorMsgBox(HRESULT hr, HWND hwnd)
{
    _com_error error(hr);
    MessageBox(hwnd, error.ErrorMessage(), L"wic_checker", MB_ICONERROR);
}

BOOL onInitDialog(HWND hwnd, HWND /*hwndFocus*/, LPARAM /*lParam*/)
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        errorMsgBox(hr, hwnd);
        return FALSE;
    }

    for (UINT i = 0; i < ARRAYSIZE(g_table); i++)
    {
        ComboBox_InsertString(GetDlgItem(hwnd, IDC_COMBO1), i, g_table[i].name);
    }

    if (FAILED(hr))
    {
        errorMsgBox(hr, hwnd);
        return FALSE;
    }

    LVCOLUMN lvc{
        /*.mask =*/ LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
    };
    for (int i = 0; i < ARRAYSIZE(g_listview_columns); i++)
    {
        lvc.pszText = const_cast<LPWSTR>(g_listview_columns[i].text);
        lvc.cx = g_listview_columns[i].cx;
        lvc.fmt = g_listview_columns[i].fmt;
        lvc.iSubItem = i;

        if (ListView_InsertColumn(GetDlgItem(hwnd, IDC_LIST1), i, &lvc) == -1)
        {
            return FALSE;
        }
    }

    ComboBox_SetCurSel(GetDlgItem(hwnd, IDC_COMBO1), 0);
    SNDMSG(hwnd, WM_COMMAND, MAKEWPARAM(IDC_COMBO1, CBN_SELCHANGE), (LPARAM)GetDlgItem(hwnd, IDC_COMBO1));

    return TRUE;
}

void onCombobox1Selchange(HWND hwnd)
{
    auto index{ ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_COMBO1)) };
    ListView_DeleteAllItems(GetDlgItem(hwnd, IDC_LIST1));
    std::vector<component_info> list;
    if (index < ARRAYSIZE(g_table) && SUCCEEDED(getComponentInfo(g_table[index].type, list)))
    {
        SetWindowRedraw(GetDlgItem(hwnd, IDC_LIST1), FALSE);

        for (int iRow = 0; iRow < list.size(); iRow++)
        {
            auto& info{ list[iRow] };
            LVITEM item{};
            item.mask = LVIF_TEXT;
            item.iItem = iRow;
            for (int iCol = 0; iCol < ARRAYSIZE(g_listview_columns); iCol++)
            {
                item.iSubItem = iCol;
                item.pszText = const_cast<LPWSTR>(g_listview_columns[iCol].lamda(info));
                if (iCol == 0)
                    ListView_InsertItem(GetDlgItem(hwnd, IDC_LIST1), &item);
                else
                    ListView_SetItem(GetDlgItem(hwnd, IDC_LIST1), &item);
            }
        }
        SetWindowRedraw(GetDlgItem(hwnd, IDC_LIST1), TRUE);
        //ListView_SetItemState(GetDlgItem(hwnd, IDC_LIST1), 0, LVIS_SELECTED, LVIS_SELECTED);
        g_currentlist = std::move(list);
    }
    Edit_SetText(GetDlgItem(hwnd, IDC_EDIT1), L"");
}

void onCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    if (id == IDOK || id == IDCANCEL)
    {
        EndDialog(hwnd, id);
        return;
    }
    else if (id == IDC_COMBO1)
    {
        switch (codeNotify)
        {
        case CBN_SELCHANGE:
        {
            onCombobox1Selchange(hwnd);
        }
        break;
        default:
            break;
        }
        return;
    }
}

void onNotify(HWND hwnd, int /*id*/, NMHDR* pnmhdr)
{
    if (pnmhdr->idFrom == IDC_LIST1)
    {
        switch (pnmhdr->code)
        {
        case LVN_ITEMCHANGED:
        {
            NMLISTVIEW* nmlv = (NMLISTVIEW*)pnmhdr;
            if (nmlv->uChanged == LVIF_STATE && nmlv->uNewState & LVIS_SELECTED &&
                nmlv->iItem < g_currentlist.size())
            {
                Edit_SetText(GetDlgItem(hwnd, IDC_EDIT1),
                    getComponentDetail(g_currentlist[nmlv->iItem]).c_str());
            }
            break;
        }
        }
    }
}


INT_PTR CALLBACK dlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, &onInitDialog);
    case WM_COMMAND:
        HANDLE_WM_COMMAND(hDlg, wParam, lParam, &onCommand);
        break;
    case WM_NOTIFY:
        HANDLE_WM_NOTIFY(hDlg, wParam, lParam, &onNotify);
        break;
    }
    return (INT_PTR)FALSE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    InitCommonControls();

    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), nullptr, &dlgProc);

    return 0;
}
