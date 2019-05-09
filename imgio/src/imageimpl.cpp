//
// Copyright 2017 Ireneusz Kapica.
//
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the 
// "Software"), to deal in the Software without restriction, including 
// without limitation the rights to use, copy, modify, merge, publish, 
// distribute, sublicense, and/or sell copies of the Software, and to 
// permit persons to whom the Software is furnished to do so, subject to 
// the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include <cstring>
#include "imageimpl.h"

namespace ImgIO
{

Image::Impl::Impl()
: mWidth(0),
  mHeight(0),
  mColorFormat(ColorSpec::Format::kRGBA),
  mColorChannelDepth(ColorSpec::ChannelDepth::k8Bit),
  mData(nullptr),
  mDataSize(0)
{
}

Image::Impl::Impl(Impl&& aImpl)
: mWidth(aImpl.mWidth),
  mHeight(aImpl.mHeight),
  mColorFormat(aImpl.mColorFormat),
  mColorChannelDepth(aImpl.mColorChannelDepth),
  mData(nullptr),
  mDataSize(aImpl.mDataSize)
{
    std::lock_guard<std::mutex> lock(aImpl.mDataMutex);

    mData.swap(aImpl.mData);

    aImpl.mWidth = 0;
    aImpl.mHeight = 0;
    aImpl.mDataSize = 0;
}

Image::Impl::Impl(const Impl& aImpl)
: mWidth(aImpl.mWidth),
  mHeight(aImpl.mHeight),
  mColorFormat(aImpl.mColorFormat),
  mColorChannelDepth(aImpl.mColorChannelDepth),
  mData(nullptr),
  mDataSize(aImpl.mDataSize)
{
    std::lock_guard<std::mutex> lock(aImpl.mDataMutex);

    mData = std::shared_ptr<uint8_t>(new uint8_t[aImpl.mDataSize]);
    std::memcpy(mData.get(), aImpl.mData.get(), aImpl.mDataSize);
}

Image::Impl::Impl(unsigned int aWidth,
                  unsigned int aHeight,
                  ColorSpec::Format aColorFormat,
                  ColorSpec::ChannelDepth aColorChannelDepth,
                  uint8_t* aData)
: mWidth(aWidth),
  mHeight(aHeight),
  mColorFormat(aColorFormat),
  mColorChannelDepth(aColorChannelDepth),
  mData(nullptr),
  mDataSize(0)
{
    size_t pixelSize = static_cast<int>(aColorFormat) * static_cast<int>(aColorChannelDepth);
    size_t lineSize = aWidth * pixelSize;
    size_t dataSize = aHeight * lineSize;

    if (dataSize > 0) {
        mData.reset(aData ? aData : new uint8_t[dataSize]);
        mDataSize = dataSize;
    }
}

bool Image::Impl::isValid() const
{
    return static_cast<bool>(mData);
}

ColorSpec::Format Image::Impl::colorFormat() const
{
    return mColorFormat;
}

ColorSpec::ChannelDepth Image::Impl::colorChannelDepth() const
{
    return mColorChannelDepth;
}

unsigned int Image::Impl::width() const
{
    return mWidth;
}

unsigned int Image::Impl::height() const
{
    return mHeight;
}

const uint8_t* Image::Impl::data() const
{
    return mData.get();
}

uint8_t* Image::Impl::data()
{
    return mData.get();
}

void Image::Impl::composite(int aX,
                            int aY,
                            const Image::Impl& aImpl,
                            CompositeOperation aCompositeOperation)
{
    throw NotImplementedException("Not implemented.s");
}

Image::Impl Image::Impl::cropped(unsigned int aX,
                                 unsigned int aY,
                                 unsigned int aWidth,
                                 unsigned int aHeight) const
{
    std::lock_guard<std::mutex> lock(mDataMutex);

    if ((aX + aWidth) > mWidth)
        aWidth = mWidth - aX;
    if ((aY + aHeight) > mHeight)
        aHeight = mHeight - aY;

    Image::Impl croppedImage(aWidth, aHeight, mColorFormat, mColorChannelDepth);

    size_t pixelSize = static_cast<int>(mColorFormat) * static_cast<int>(mColorChannelDepth);
    size_t srcLineSize = mWidth * pixelSize;
    size_t destLineSize = aWidth * pixelSize;

    if (croppedImage.isValid()) {
        uint8_t* srcDataLine = mData.get() + aY * srcLineSize;
        uint8_t* destDataLine = croppedImage.mData.get();

        for (size_t h = 0; h < aHeight; ++h) {
            std::memcpy(destDataLine, srcDataLine + aX, destLineSize);
            srcDataLine += srcLineSize;
            destDataLine += destLineSize;
        }
    }

    return croppedImage;
}

Image::Impl Image::Impl::convertedTo(ColorSpec::Format aFormat,
                                     ColorSpec::ChannelDepth aChannelDepth) const
{
    if ((mColorFormat == aFormat) && (mColorChannelDepth == aChannelDepth))
        return Image::Impl(*this);

    void (*convertFunc)(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount) = nullptr;

    switch(mColorFormat)
    {
    case ColorSpec::Format::kRGB:
    {
        if (mColorChannelDepth == ColorSpec::ChannelDepth::k8Bit) {
            if ((aFormat == ColorSpec::Format::kRGB) && (aChannelDepth == ColorSpec::ChannelDepth::k16Bit))
                convertFunc = convertRGB8BitToRGB16Bit;
            else if ((aFormat == ColorSpec::Format::kRGBA) && (aChannelDepth == ColorSpec::ChannelDepth::k8Bit))
                convertFunc = convertRGB8BitToRGBA8Bit;
            else if ((aFormat == ColorSpec::Format::kRGBA) && (aChannelDepth == ColorSpec::ChannelDepth::k16Bit))
                convertFunc = convertRGB8BitToRGBA16Bit;
        } else if (mColorChannelDepth == ColorSpec::ChannelDepth::k16Bit) {
            if ((aFormat == ColorSpec::Format::kRGB) && (aChannelDepth == ColorSpec::ChannelDepth::k8Bit))
                convertFunc = convertRGB16BitToRGB8Bit;
            else if ((aFormat == ColorSpec::Format::kRGBA) && (aChannelDepth == ColorSpec::ChannelDepth::k8Bit))
                convertFunc = convertRGB16BitToRGBA8Bit;
            else if ((aFormat == ColorSpec::Format::kRGBA) && (aChannelDepth == ColorSpec::ChannelDepth::k16Bit))
                convertFunc = convertRGB16BitToRGBA16Bit;
        }
    }
    case ColorSpec::Format::kRGBA:
    {
        if (mColorChannelDepth == ColorSpec::ChannelDepth::k8Bit) {
            if ((aFormat == ColorSpec::Format::kRGBA) && (aChannelDepth == ColorSpec::ChannelDepth::k16Bit))
                convertFunc = convertRGBA8BitToRGBA16Bit;
            else if ((aFormat == ColorSpec::Format::kRGB) && (aChannelDepth == ColorSpec::ChannelDepth::k8Bit))
                convertFunc = convertRGBA8BitToRGB8Bit;
            else if ((aFormat == ColorSpec::Format::kRGB) && (aChannelDepth == ColorSpec::ChannelDepth::k16Bit))
                convertFunc = convertRGBA8BitToRGB16Bit;
        } else if (mColorChannelDepth == ColorSpec::ChannelDepth::k16Bit) {
            if ((aFormat == ColorSpec::Format::kRGBA) && (aChannelDepth == ColorSpec::ChannelDepth::k8Bit))
                convertFunc = convertRGBA16BitToRGBA8Bit;
            else if ((aFormat == ColorSpec::Format::kRGB) && (aChannelDepth == ColorSpec::ChannelDepth::k8Bit))
                convertFunc = convertRGBA16BitToRGB8Bit;
            else if ((aFormat == ColorSpec::Format::kRGB) && (aChannelDepth == ColorSpec::ChannelDepth::k16Bit))
                convertFunc = convertRGBA16BitToRGB16Bit;
        }
    }
    default:
        break;
    }

    if (!convertFunc) {
        throw std::logic_error("Not implemented.s");
    }

    Image::Impl image(mWidth, mHeight, aFormat, aChannelDepth);
    size_t pixelsCount = mWidth * mHeight;
    convertFunc(mData.get(), image.mData.get(), pixelsCount);

    return image;
}

Image::Impl& Image::Impl::operator=(const Image::Impl& aImpl)
{
    std::lock_guard<std::mutex> lock(aImpl.mDataMutex);

    mWidth = aImpl.mWidth;
    mHeight = aImpl.mHeight;
    mColorFormat = aImpl.mColorFormat;
    mColorChannelDepth = aImpl.mColorChannelDepth;
    mDataSize = aImpl.mDataSize;
    mData = std::shared_ptr<uint8_t>(new uint8_t[aImpl.mDataSize]);
    std::memcpy(mData.get(), aImpl.mData.get(), aImpl.mDataSize);

    return *this;
}

Image::Impl& Image::Impl::operator=(Image::Impl&& aImpl) noexcept
{
    std::lock_guard<std::mutex> lock(aImpl.mDataMutex);

    mWidth = aImpl.mWidth;
    aImpl.mWidth = 0;

    mHeight = aImpl.mHeight;
    aImpl.mHeight = 0;

    mColorFormat = aImpl.mColorFormat;
    mColorChannelDepth = aImpl.mColorChannelDepth;

    mData.swap(aImpl.mData);

    mDataSize = aImpl.mDataSize;
    aImpl.mDataSize = 0;

    return *this;
}


void Image::Impl::convertRGB8BitToRGB16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 3;
    const size_t destPixelSize = 6;

    for (size_t i = 0; i < aPixelsCount; ++i, dest += destPixelSize, src += srcPixelSize) {
        reinterpret_cast<uint16_t*>(dest)[0] = static_cast<uint16_t>(src[0]) << 8;
        reinterpret_cast<uint16_t*>(dest)[1] = static_cast<uint16_t>(src[1]) << 8;
        reinterpret_cast<uint16_t*>(dest)[2] = static_cast<uint16_t>(src[2]) << 8;
    }
}

void Image::Impl::convertRGB8BitToRGBA8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 3;
    const size_t destPixelSize = 4;

    for (size_t i = 0; i < aPixelsCount; ++i, dest += destPixelSize, src += srcPixelSize) {
        std::memcpy(dest, src, srcPixelSize);
        dest[3] = static_cast<uint8_t>(1 << 8);
    };
}

void Image::Impl::convertRGB8BitToRGBA16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 3;
    const size_t destPixelSize = 8;

    for (size_t i = 0; i < aPixelsCount; ++i, dest += destPixelSize, src += srcPixelSize) {
        reinterpret_cast<uint16_t*>(dest)[0] = static_cast<uint16_t>(src[0]) << 8;
        reinterpret_cast<uint16_t*>(dest)[1] = static_cast<uint16_t>(src[1]) << 8;
        reinterpret_cast<uint16_t*>(dest)[2] = static_cast<uint16_t>(src[2]) << 8;
        reinterpret_cast<uint16_t*>(dest)[3] = static_cast<uint16_t>(1 << 16);
    }
}

void Image::Impl::convertRGB16BitToRGB8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 6;
    const size_t destPixelSize = 3;

    for (size_t i = 0; i < aPixelsCount; ++i, dest += destPixelSize, src += srcPixelSize) {
        dest[0] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[0] >> 8);
        dest[1] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[1] >> 8);
        dest[2] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[2] >> 8);
    }
}

void Image::Impl::convertRGB16BitToRGBA8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 6;
    const size_t destPixelSize = 4;

    for (size_t i = 0; i < aPixelsCount; ++i, dest += destPixelSize, src += srcPixelSize) {
        dest[0] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[0] >> 8);
        dest[1] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[1] >> 8);
        dest[2] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[2] >> 8);
        dest[3] = static_cast<uint8_t>(1 << 8);
    }
}

void Image::Impl::convertRGB16BitToRGBA16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 6;
    const size_t destPixelSize = 8;

    for (size_t i = 0; i < aPixelsCount; ++i, dest += destPixelSize, src += srcPixelSize) {
        std::memcpy(dest, src, srcPixelSize);
        reinterpret_cast<uint16_t*>(dest)[3] = static_cast<uint16_t>(1 << 16);
    };
}

void Image::Impl::convertRGBA8BitToRGBA16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 4;
    const size_t destPixelSize = 8;

    for (size_t i = 0; i < aPixelsCount; ++i, dest += destPixelSize, src += srcPixelSize) {
        reinterpret_cast<uint16_t*>(dest)[0] = static_cast<uint16_t>(src[0]) << 8;
        reinterpret_cast<uint16_t*>(dest)[1] = static_cast<uint16_t>(src[1]) << 8;
        reinterpret_cast<uint16_t*>(dest)[2] = static_cast<uint16_t>(src[2]) << 8;
        reinterpret_cast<uint16_t*>(dest)[3] = static_cast<uint16_t>(src[3]) << 8;
    }
}

void Image::Impl::convertRGBA8BitToRGB8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 4;
    const size_t destPixelSize = 3;

    for (size_t i = 0; i < (aPixelsCount - 1); ++i, dest += destPixelSize, src += srcPixelSize) {
        std::memcpy(dest, src, srcPixelSize);
    }
    std::memcpy(dest, src, destPixelSize);
}

void Image::Impl::convertRGBA8BitToRGB16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 4;
    const size_t destPixelSize = 6;

    for (size_t i = 0; i < aPixelsCount; ++i, dest += destPixelSize, src += srcPixelSize) {
        reinterpret_cast<uint16_t*>(dest)[0] = static_cast<uint16_t>(src[0]) << 8;
        reinterpret_cast<uint16_t*>(dest)[1] = static_cast<uint16_t>(src[1]) << 8;
        reinterpret_cast<uint16_t*>(dest)[2] = static_cast<uint16_t>(src[2]) << 8;
    }
}

void Image::Impl::convertRGBA16BitToRGBA8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 8;
    const size_t destPixelSize = 4;

    for (size_t i = 0; i < aPixelsCount; ++i, dest += destPixelSize, src += srcPixelSize) {
        dest[0] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[0] >> 8);
        dest[1] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[1] >> 8);
        dest[2] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[2] >> 8);
        dest[3] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[3] >> 8);
    }
}

void Image::Impl::convertRGBA16BitToRGB8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 8;
    const size_t destPixelSize = 3;

    for (size_t i = 0; i < aPixelsCount; ++i, dest += destPixelSize, src += srcPixelSize) {
        dest[0] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[0] >> 8);
        dest[1] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[1] >> 8);
        dest[2] = static_cast<uint8_t>(reinterpret_cast<const uint16_t*>(src)[2] >> 8);
    }
}

void Image::Impl::convertRGBA16BitToRGB16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount)
{
    const uint8_t* src = aSrc;
    uint8_t* dest = aDest;
    const size_t srcPixelSize = 8;
    const size_t destPixelSize = 6;

    for (size_t i = 0; i < (aPixelsCount - 1); ++i, dest += destPixelSize, src += srcPixelSize) {
        std::memcpy(dest, src, srcPixelSize);
    }
    std::memcpy(dest, src, destPixelSize);
}

} // namespace ImgIO

// EOF
