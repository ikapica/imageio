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

#ifndef __IMAGEIO_IMAGEIO_H__
#define __IMAGEIO_IMAGEIO_H__

#include <iostream>
#include <imgio/image.h>

namespace ImgIO {

class ImageIO
{
public:
    enum class ImageFormat {kUnspecified = 0,
                            kRaw = 1,
                            kPng = 2,
                            kJpeg = 3,
                            kGif = 4};
public:
    static Image read(std::istream &aInputDataStream,
                      ImageFormat aInputImageFormat = ImageFormat::kUnspecified,
                      ColorSpec::Format aOutputImageColorformat = ColorSpec::Format::kRGBA,
                      ColorSpec::ChannelDepth aOutputImageChannelDepth = ColorSpec::ChannelDepth::k8Bit);

    static Image read(const uint8_t *aInputData,
                      size_t aLength,
                      ImageFormat aInputImageFormat = ImageFormat::kUnspecified,
                      ColorSpec::Format aOutputImageColorformat = ColorSpec::Format::kRGBA,
                      ColorSpec::ChannelDepth aOutputImageChannelDepth = ColorSpec::ChannelDepth::k8Bit);

    static void write(const Image &aImage,
                      std::ostream &aOutputDataStream,
                      ImageFormat aImageFormat = ImageFormat::kUnspecified);

    static void write(const Image &aImage,
                      uint8_t *aOutputDataBuf,
                      size_t aOutputDataBufLength,
                      ImageFormat aImageFormat = ImageFormat::kUnspecified);
}; // class ImageIO

} // namespace ImgIO

#endif // __IMAGEIO_IMAGEIO_H__
// EOF
