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

#ifndef _IMAGEIMPL_H__
#define _IMAGEIMPL_H__

#include <imgio/image.h>
#include <mutex>

namespace ImgIO
{

class Image::Impl
{
public:
    Impl();
    Impl(Impl&& aImpl);
    Impl(const Impl& aImpl);
    Impl(unsigned int aWidth,
         unsigned int aHeight,
         ColorSpec::Format aColorFormat = ColorSpec::Format::kRGB,
         ColorSpec::ChannelDepth aColorChannelDepth = ColorSpec::ChannelDepth::k8Bit,
         uint8_t* aData = nullptr);

    bool isValid() const;
    ColorSpec::Format colorFormat() const;
    ColorSpec::ChannelDepth colorChannelDepth() const;
    unsigned int width() const;
    unsigned int height() const;
    const uint8_t* data() const;
    uint8_t* data();

    void composite(int aX,
                   int aY,
                   const Image::Impl& aImpl,
                   CompositeOperation aCompositeOperation = Image::CompositeOperation::kCopy);

    Image::Impl cropped(unsigned int aX,
                        unsigned int aY,
                        unsigned int aWidth,
                        unsigned int aHeight) const;

    Image::Impl convertedTo(ColorSpec::Format aFormat,
                            ColorSpec::ChannelDepth aChannelDepth = ColorSpec::ChannelDepth::k8Bit) const;

    Image::Impl& operator=(const Image::Impl& aImpl);
    Image::Impl& operator=(Image::Impl&& aImpl) noexcept ;


    static void convertRGB8BitToRGB16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);
    static void convertRGB8BitToRGBA8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);
    static void convertRGB8BitToRGBA16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);

    static void convertRGB16BitToRGB8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);
    static void convertRGB16BitToRGBA8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);
    static void convertRGB16BitToRGBA16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);

    static void convertRGBA8BitToRGBA16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);
    static void convertRGBA8BitToRGB8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);
    static void convertRGBA8BitToRGB16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);

    static void convertRGBA16BitToRGBA8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);
    static void convertRGBA16BitToRGB8Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);
    static void convertRGBA16BitToRGB16Bit(const uint8_t* aSrc, uint8_t* aDest, size_t aPixelsCount);

private:
    unsigned int mWidth;
    unsigned int mHeight;
    ColorSpec::Format mColorFormat;
    ColorSpec::ChannelDepth mColorChannelDepth;
    std::shared_ptr<uint8_t> mData;
    size_t mDataSize;
    mutable std::mutex mDataMutex;
}; // class Image::Impl

} // namespace ImgIO

#endif // _IMAGEIMPL_H__
// EOF
