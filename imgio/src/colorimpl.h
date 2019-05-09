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

#ifndef __IMAGEIO_COLORIMPL_H__
#define __IMAGEIO_COLORIMPL_H__

#include <imgio/color.h>

namespace ImgIO
{

/**
 * Color specification class implementation.
 */
class ColorSpec::Impl
{
public:
    /**
     * Constructor.
     * @param aFormat Color format.
     * @param aChannelDepth Color channel depth.
     */
    Impl(ColorSpec::Format aFormat, ColorSpec::ChannelDepth aChannelDepth)
    : mFormat(aFormat), mChannelDepth(aChannelDepth)
    {}

    /**
     * Copy constructor.
     * @param rhs Color spec to be copied.
     */
    Impl(const Impl& rhs) = default;

    /**
     * Move constructor.
     * @param rhs Color spec to be moved.
     */
    Impl(Impl&& rhs) = default;

    /**
     * Destructor.
     */
    virtual ~Impl() = default;

    /**
     * Returns color channel depth.
     * @return Color channel depth.
     */
    ColorSpec::ChannelDepth channelDepth() const
    {
        return mChannelDepth;
    }

    /**
     * Returns color format.
     * @return Color format.
     */
    ColorSpec::Format format() const
    {
        return mFormat;
    }

    /**
     * Assigment operator.
     * @param rhs Color spec to be copied.
     */
    Impl& operator=(const Impl& rhs) = default;

    /**
     * Move assigment operator.
     * @param rhs Color spec to be moved.
     */
    Impl& operator=(Impl&& rhs) = default;
private:
    ColorSpec::Format mFormat;
    ColorSpec::ChannelDepth mChannelDepth;
}; // class ColorSpec

}; // namespace ImgIO

#endif // __IMAGEIO_COLORIMPL_H__
