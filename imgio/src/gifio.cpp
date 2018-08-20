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

#include "gifio.h"
#include <iostream>
#include <gif_lib.h>

#include "dataio.h"

namespace ImgIO
{

    static Image readGif(DataReader& aDataReader,
                         ColorSpec::Format aOutputImageformat,
                         ColorSpec::ChannelDepth aOutputImageChannelDepth)
    {
        // TODO: Implement
        return {};
    }

    static void writeGif(DataWriter& aDataWriter,
                         Image& aImage)
    {
        // TODO: Implement
    }

    Image GifIO::read(std::istream& aPngDataStream,
                      ColorSpec::Format aOutputImageformat,
                      ColorSpec::ChannelDepth aOutputImageChannelDepth)
    {
        StreamReader streamReader(aPngDataStream);
        return readGif(streamReader,
                       aOutputImageformat,
                       aOutputImageChannelDepth);
    }

    Image GifIO::read(const uint8_t* aData,
                      size_t aLength,
                      ColorSpec::Format aOutputImageformat,
                      ColorSpec::ChannelDepth aOutputImageChannelDepth)
    {
        MemoryReader memoryReader(aData, aLength);
        return readGif(memoryReader,
                       aOutputImageformat,
                       aOutputImageChannelDepth);
    }

    void GifIO::write(Image& aImage, std::ostream& aPngDataStream)
    {
        StreamWriter streamWriter(aPngDataStream);
        writeGif(streamWriter, aImage);
    }

    void GifIO::write(Image& aImage, uint8_t* aData, size_t aLength)
    {
        MemoryWriter streamWriter(aData, aLength);
        writeGif(streamWriter, aImage);
    }

} // namespace ImgIO
// EOF
