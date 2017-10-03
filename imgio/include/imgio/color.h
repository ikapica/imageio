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

class Color
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
    Color() = default;
    virtual ~Color() = default;

    virtual Color::ChannelDepth channelDepth() const = 0;
    virtual Color::Format format() const = 0;
}; // class Color

class ColorRGB8 : public Color
{
public:
}; // class ColorRBG8

class ColorRGBA8 : public Color
{
public:
}; // class ColorRBGA8

class ColorRGB16 : public Color
{
public:
}; // class ColorRBG16

class ColorRGBA16 : public Color
{
public:
}; // class ColorRBGA16

}; // namespace ImgIO

#endif // __IMAGEIO_COLOR_H__
