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

#include <imgio/image.h>
#include "imageimpl.h"

namespace ImgIO
{

Image::Image()
: mImpl(new Image::Impl)
{}

Image::Image(Image&& aImage)
: mImpl(std::move(aImage.mImpl))
{}

Image::Image(const Image& aImage)
: mImpl(new Impl(*aImage.mImpl))
{}

Image::Image(unsigned int aWidth, unsigned int aHeight, Format aFormat, Image::ChannelDepth aChannelDepth, uint8_t* aData)
: mImpl(new Impl(aWidth, aHeight, aFormat, aChannelDepth, aData))
{}

Image::Image(Impl&& aImpl)
        : mImpl(new Impl(std::move(aImpl)))
{
}

Image::~Image()
{}

bool Image::isValid() const
{
    return mImpl->isValid();
}

Image::Format Image::format() const
{
    return mImpl->format();
}

Image::ChannelDepth Image::channelDepth() const
{
    return mImpl->channelDepth();
}

unsigned int Image::width() const
{
    return mImpl->width();
}

unsigned int Image::height() const
{
    return mImpl->height();
}

const uint8_t* Image::data() const
{
    return mImpl->data();
}

Image& Image::composite(int aX,
                        int aY,
                        const Image& aImage,
                        CompositeOperation aCompositeOperation)
{
    mImpl->composite(aX, aY, *aImage.mImpl, aCompositeOperation);
    return *this;
}

Image Image::cropped(unsigned int aX,
                     unsigned int aY,
                     unsigned int aWidth,
                     unsigned int aHeight) const
{
    return mImpl->cropped(aX, aY, aWidth, aHeight);
}

Image Image::convertedTo(Image::Format aFormat,
                         Image::ChannelDepth aChannelDepth)
{
    return mImpl->convertedTo(aFormat, aChannelDepth);
}

} // namespace ImgIO

// EOF
