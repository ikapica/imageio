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

#ifndef __IMAGEIO_COLOR_H__
#define __IMAGEIO_COLOR_H__

namespace ImgIO
{

class ColorSpec
{
public:
    enum class ChannelDepth {
        kUnspecified = 0,
        k8Bit = 1,
        k16Bit = 2
    }; // enum class ColorChannelDepth

    enum class Format {
        kUnspecified = 0,
        kMonochromatic = 1,
        kRGB = 3,
        kRGBA = 4,
    }; // enum class ColorFormat

public:
    ColorSpec(ColorSpec::Format aFormat, ColorSpec::ChannelDepth aChannelDepth)
    : mFormat(aFormat), mChannelDepth(aChannelDepth)
    {
    }

    virtual ~ColorSpec() = default;

    ColorSpec::ChannelDepth channelDepth() const
    {
        return mChannelDepth;
    }

    ColorSpec::Format format() const
    {
        return mFormat;
    }
private:
    Format mFormat;
    ChannelDepth mChannelDepth;
}; // class ColorSpec

class ColorRGB8 : public ColorSpec
{
public:
    ColorRGB8()
    : ColorSpec(Format::kRGB, ChannelDepth::k8Bit)
    {}
}; // class ColorRBG8

class ColorRGBA8 : public ColorSpec
{
public:
    ColorRGBA8()
    : ColorSpec(Format::kRGBA, ChannelDepth::k8Bit)
    {}
}; // class ColorRBGA8

class ColorRGB16 : public ColorSpec
{
public:
    ColorRGB16()
    : ColorSpec(Format::kRGB, ChannelDepth::k16Bit)
    {}
}; // class ColorRBG16

class ColorRGBA16 : public ColorSpec
{
public:
    ColorRGBA16()
    : ColorSpec(Format::kRGBA, ChannelDepth::k16Bit)
    {}
}; // class ColorRBGA16

}; // namespace ImgIO

#endif // __IMAGEIO_COLOR_H__
