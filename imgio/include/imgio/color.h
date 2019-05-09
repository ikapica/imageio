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

#include <memory>

namespace ImgIO
{

/**
 * Color specification class.
 */
class ColorSpec
{
public:
    /**
     * Color channel depth.
     */
    enum class ChannelDepth {
        /**
         * 8 bit color channel.
         */
        k8Bit = 1,
        /**
         * 16 bit color channel.
         */
        k16Bit = 2
    }; // enum class ColorChannelDepth

    /**
     * Color format.
     */
    enum class Format {
        /**
         * Monochromatic (single channel).
         */
        kMonochromatic = 1,

        /**
         * RGB (three channels, red, green and blue).
         */
        kRGB = 3,

        /**
         * RGBA (four channels, red, green, blue and alpha).
         */
        kRGBA = 4,
    }; // enum class ColorFormat

public:
    /**
     * Constructor.
     * @param aFormat Color format.
     * @param aChannelDepth Color channel depth.
     */
    ColorSpec(ColorSpec::Format aFormat, ColorSpec::ChannelDepth aChannelDepth);

    /**
     * Copy constructor.
     * @param rhs Color spec to be copied.
     */
    ColorSpec(const ColorSpec& rhs) = default;

    /**
     * Move constructor.
     * @param rhs Color spec to be moved.
     */
    ColorSpec(ColorSpec&& rhs) = default;

    /**
     * Destructor.
     */
    virtual ~ColorSpec();

    /**
     * Returns color channel depth.
     * @return Color channel depth.
     */
    ColorSpec::ChannelDepth channelDepth() const;

    /**
     * Returns color format.
     * @return Color format.
     */
    ColorSpec::Format format() const;

    /**
     * Assigment operator.
     * @param rhs Color spec to be copied.
     */
    ColorSpec& operator=(const ColorSpec& rhs) = default;

    /**
     * Move assigment operator.
     * @param rhs Color spec to be moved.
     */
    ColorSpec& operator=(ColorSpec&& rhs) = default;
private:
    class Impl;
private:
    std::unique_ptr<Impl> mImpl;
}; // class ColorSpec

/**
 * RGB, 8 bit per channel color specification class.
 */
class ColorRGB8 : public ColorSpec
{
public:
    /**
     * Constructor.
     */
    ColorRGB8()
    : ColorSpec(Format::kRGB, ChannelDepth::k8Bit)
    {}
}; // class ColorRBG8

/**
 * RGBA, 8 bit per channel color specification class.
 */
class ColorRGBA8 : public ColorSpec
{
public:
    /**
     * Constructor.
     */
    ColorRGBA8()
    : ColorSpec(Format::kRGBA, ChannelDepth::k8Bit)
    {}
}; // class ColorRBGA8

/**
 * RGB, 16 bits per channel color specification class.
 */
class ColorRGB16 : public ColorSpec
{
public:
    /**
     * Constructor.
     */
    ColorRGB16()
    : ColorSpec(Format::kRGB, ChannelDepth::k16Bit)
    {}
}; // class ColorRBG16

/**
 * RGBA, 16 bit per channel color specification class.
 */
class ColorRGBA16 : public ColorSpec
{
public:
    /**
     * Constructor.
     */
    ColorRGBA16()
    : ColorSpec(Format::kRGBA, ChannelDepth::k16Bit)
    {}
}; // class ColorRBGA16

}; // namespace ImgIO

#endif // __IMAGEIO_COLOR_H__
