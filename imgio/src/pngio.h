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

#ifndef _PNGIO_H__
#define _PNGIO_H__

#include <imgio/image.h>

namespace ImgIO
{

class PngIO
{
public:
    static Image read(std::istream& aPngDataStream,
                      ColorSpec::Format aOutputImageformat = ColorSpec::Format::kRGBA,
                      ColorSpec::ChannelDepth aOutputImageChannelDepth = ColorSpec::ChannelDepth::k8Bit);
    static Image read(const uint8_t* aData,
                      size_t aLength,
                      ColorSpec::Format aOutputImageformat = ColorSpec::Format::kRGBA,
                      ColorSpec::ChannelDepth aOutputImageChannelDepth = ColorSpec::ChannelDepth::k8Bit);
    static void write(Image& aImage,
                      std::ostream& aPngDataStream);
    static void write(Image& aImage,
                      uint8_t* aData,
                      size_t aLength);
}; // class PngIO

} // namespace ImgIO

#endif // _PNGIO_H__
// EOF
