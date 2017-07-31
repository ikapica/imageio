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

#ifndef _IMAGEIO_IMAGE_H__
#define _IMAGEIO_IMAGE_H__

#include <string>

namespace ImgIO
{

class Image
{
public:
    enum class ChannelDepth {
        kUnspecified = 0,
        k8Bit = 1,
        k16Bit = 2
    }; // enum class ChannelDepth

    enum class Format {
        kUnspecified = 0,
        kMonochromatic = 1,
        kRGB = 3,
        kRGBA = 4,
    }; // enum class Format

    enum class CompositeOperation
    {
        kCopy,
    };

public:
    Image();
    Image(Image&& aImage);
    Image(const Image& aImage);
    Image(unsigned int aWidth,
          unsigned int aHeight,
          Image::Format aFormat = Image::Format::kRGB,
          Image::ChannelDepth aChannelDepth = Image::ChannelDepth::k8Bit,
          uint8_t* aData = nullptr);
    ~Image();

    bool isValid() const;
    Image::Format format() const;
    Image::ChannelDepth channelDepth() const;
    unsigned int width() const;
    unsigned int height() const;
    const uint8_t* data() const;


    Image& composite(int aX,
                     int aY,
                     const Image& aImage,
                     CompositeOperation aCompositeOperation = Image::CompositeOperation::kCopy);

    Image cropped(unsigned int aX,
                  unsigned int aY,
                  unsigned int aWidth,
                  unsigned int aHeight) const;

    Image convertedTo(Image::Format aFormat,
                      Image::ChannelDepth aChannelDepth = Image::ChannelDepth::k8Bit);
private:
    class Impl;
private:
    Image(Image::Impl&& aImpl);
private:
    std::unique_ptr<Impl> mImpl;
}; // class Image

} // namespace ImgIO

#endif // _IMAGEIO_IMAGE_H__
// EOF