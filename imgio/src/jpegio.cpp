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

#include "jpegio.h"
#include <jpeglib.h>

#include "dataio.h"

namespace ImgIO
{

struct my_source_mgr {
    struct jpeg_source_mgr pub;
    std::istream* is;
    JOCTET*       buffer;
};

class JpegSourceManager : private jpeg_source_mgr {
public:
    JpegSourceManager(j_decompress_ptr aDecompressInfo, DataReader &aDataReader)
            : mDataReader(aDataReader), mBuffer(new JOCTET[1025]), mBufferSize(1024) {
        init_source = initSource;
        fill_input_buffer = fillInputBuffer;
        skip_input_data = skipInputData;
        resync_to_restart = jpeg_resync_to_restart; /* use default method */
        term_source = terminateSource;
        bytes_in_buffer = 0;
        next_input_byte = 0;

        aDecompressInfo->src = static_cast<struct jpeg_source_mgr *>(this);
    }

    void resizeBuffer(size_t aSize)
    {
        mBuffer.reset(new JOCTET[aSize > 2 ? aSize : 2]);
        mBufferSize = (aSize > 2 ? aSize : 2);
    }
private:
    static void initSource(j_decompress_ptr aDecompressInfo) {
    }

    static boolean fillInputBuffer(j_decompress_ptr aDecompressInfo) {
        JpegSourceManager* src = reinterpret_cast<JpegSourceManager*>(aDecompressInfo->src);

        size_t bytes = src->mDataReader.read(src->mBuffer.get(), src->mBufferSize);
        if (bytes == 0) {
            /* Insert a fake EOI marker */
            src->mBuffer.get()[0] = (JOCTET) 0xFF;
            src->mBuffer.get()[1] = (JOCTET) JPEG_EOI;
            bytes = 2;
        }
        src->next_input_byte = src->mBuffer.get();
        src->bytes_in_buffer = bytes;

        return TRUE;
    }

    static void skipInputData(j_decompress_ptr aDecompressInfo, long aNumBytes) {
        JpegSourceManager* src = reinterpret_cast<JpegSourceManager*>(aDecompressInfo->src);

        if (aNumBytes > 0) {
            while (aNumBytes > src->bytes_in_buffer) {
                aNumBytes -= src->bytes_in_buffer;
                fillInputBuffer(aDecompressInfo);
            }
            src->next_input_byte += aNumBytes;
            src->bytes_in_buffer -= aNumBytes;
        }
    }

    static void terminateSource(j_decompress_ptr aDecompressInfo) {
        JpegSourceManager* src = reinterpret_cast<JpegSourceManager*>(aDecompressInfo->src);
        src->mDataReader.clearErrors();
        src->mDataReader.seekPos(src->mDataReader.tellPos() - (std::streampos)src->bytes_in_buffer);
    }
private:
    DataReader& mDataReader;
    std::unique_ptr<JOCTET> mBuffer;
    size_t mBufferSize;
}; // class JpegSourceManager

class JpegDestinationManager : private jpeg_destination_mgr {
public:
    JpegDestinationManager(j_compress_ptr aCompressInfo, DataWriter& aDataWriter, size_t aBufferSize)
    : mDataWriter(aDataWriter), mBuffer(new JOCTET[aBufferSize]), mBufferSize(aBufferSize)
    {
        init_destination = init;
        empty_output_buffer = write;
        term_destination = terminate;

        aCompressInfo->dest = static_cast<struct jpeg_destination_mgr *>(this);
    }

private:
    static void init(j_compress_ptr compressInfo) {
        JpegDestinationManager* destMgr = reinterpret_cast<JpegDestinationManager*>(compressInfo->dest);
//        dest->buffer = (JOCTET*)(*compressInfo->mem->alloc_small) ((j_common_ptr)compressInfo, JPOOL_IMAGE, JPEG_BUF_SIZE * sizeof(JOCTET));
        destMgr->next_output_byte = destMgr->mBuffer.get();
        destMgr->free_in_buffer = destMgr->mBufferSize;
    }

    static boolean write(j_compress_ptr copressInfo) {
        JpegDestinationManager* destMgr = reinterpret_cast<JpegDestinationManager*>(copressInfo->dest);

        destMgr->mDataWriter.write(static_cast<uint8_t*>(destMgr->mBuffer.get()), destMgr->mBufferSize);
//        if (destMgr->os->fail()) {
//            LOG_FATAL("Couldn't write entire jpeg buffer to stream.");
//        }
        destMgr->next_output_byte = destMgr->mBuffer.get();
        destMgr->free_in_buffer = destMgr->mBufferSize;

        return true;
    }

    static void terminate(j_compress_ptr cinfo) {
        JpegDestinationManager* destMgr = (JpegDestinationManager*) cinfo->dest;
        size_t remainingDataSize = destMgr->mBufferSize - destMgr->free_in_buffer;

        if (remainingDataSize > 0) {
            destMgr->mDataWriter.write(static_cast<uint8_t*>(destMgr->mBuffer.get()), remainingDataSize);
//            if (destMgr->os->fail()) {
//                LOG_FATAL("Couldn't write remaining jpeg data to stream.");
//            }
        }
        destMgr->mDataWriter.flush();
    }

private:
    DataWriter& mDataWriter;
    std::unique_ptr<JOCTET> mBuffer;
    size_t mBufferSize;
};

static Image readJpeg(DataReader& aDataReader,
                     Image::Format aOutputImageformat,
                     Image::ChannelDepth aOutputImageChannelDepth)
{
    std::function<void(struct jpeg_decompress_struct*)> decompressInfoAutoCleanupFunction =
            [](struct jpeg_decompress_struct* aDecompressInfo) {
                jpeg_destroy_decompress(aDecompressInfo);
            };
    std::unique_ptr<struct jpeg_decompress_struct, std::function<void(struct jpeg_decompress_struct*)>> decompressInfoAutoCleanup(nullptr,
                                                                                                                                  decompressInfoAutoCleanupFunction);
    struct jpeg_decompress_struct decompressInfo;
    struct jpeg_error_mgr errorManager;

    decompressInfo.err = jpeg_std_error(&errorManager);
    jpeg_create_decompress(&decompressInfo);
    decompressInfoAutoCleanup.reset(&decompressInfo);

    JpegSourceManager sourceManager(&decompressInfo, aDataReader);

    if (jpeg_read_header(&decompressInfo, TRUE) != JPEG_HEADER_OK) {
        throw std::logic_error("Failed to read JPEG header.");
    }

    if (decompressInfo.num_components != 3) {
        throw std::logic_error("Unsupported number of color components: " + std::to_string(decompressInfo.num_components));
    }

    jpeg_start_decompress(&decompressInfo);

    size_t rowSize = decompressInfo.output_width * static_cast<int>(Image::Format::kRGB) * static_cast<int>(Image::ChannelDepth::k8Bit);
    size_t dataSize = decompressInfo.output_height * rowSize;

    std::unique_ptr<uint8_t> data(new uint8_t[dataSize]);
    std::unique_ptr<uint8_t*> dataRows(new uint8_t*[decompressInfo.output_height]);

    for (size_t y = 0; y < decompressInfo.output_height; y++) {
        dataRows.get()[y] = data.get() + y * rowSize;
    }

    sourceManager.resizeBuffer(rowSize);
    while (decompressInfo.output_scanline != decompressInfo.output_height) {
        jpeg_read_scanlines(&decompressInfo,
                            dataRows.get() + decompressInfo.output_scanline,
                            decompressInfo.output_height - decompressInfo.output_scanline);
    }
    jpeg_finish_decompress(&decompressInfo);

    return Image(decompressInfo.output_width,
                 decompressInfo.output_height,
                 Image::Format::kRGB,
                 Image::ChannelDepth::k8Bit,
                 data.release());
}

static void writeJpeg(DataWriter& aDataWriter,
                     const Image& aImage)
{
    int quality = 75;
    if ((aImage.format() != Image::Format::kRGB) || (aImage.channelDepth() != Image::ChannelDepth::k8Bit)) {
        throw std::logic_error("Expected RGB (8bit per channel) image");
    }

    struct jpeg_compress_struct compressInfo;
    struct jpeg_error_mgr errorManager;

    compressInfo.err = jpeg_std_error(&errorManager);
    jpeg_create_compress(&compressInfo);

    compressInfo.image_width = aImage.width(); 	/* image width and height, in pixels */
    compressInfo.image_height = aImage.height();

    compressInfo.input_components = 3;
    compressInfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&compressInfo);
    jpeg_set_quality(&compressInfo, quality, true);


    size_t rowLength = aImage.width() * static_cast<int>(aImage.format()) * static_cast<int>(aImage.channelDepth());

    JpegDestinationManager destinationManager(&compressInfo, aDataWriter, rowLength);

    jpeg_start_compress(&compressInfo, true);

    std::unique_ptr<const uint8_t*> rows(new const uint8_t*[aImage.height()]);

    const uint8_t* row = aImage.data();
    for (size_t y = 0; y < aImage.height(); ++y)
    {
        rows.get()[y] = row;
        row += rowLength;
    }

    jpeg_write_scanlines(&compressInfo, const_cast<JSAMPARRAY>(rows.get()), aImage.height());

    jpeg_finish_compress(&compressInfo);
    jpeg_destroy_compress(&compressInfo);
}

Image JpegIO::read(std::istream& aPngDataStream,
                  Image::Format aOutputImageformat,
                  Image::ChannelDepth aOutputImageChannelDepth)
{
    StreamReader streamReader(aPngDataStream);
    return readJpeg(streamReader,
                   aOutputImageformat,
                   aOutputImageChannelDepth);
}

Image JpegIO::read(const uint8_t* aData,
                  size_t aLength,
                  Image::Format aOutputImageformat,
                  Image::ChannelDepth aOutputImageChannelDepth)
{
    MemoryReader memoryReader(aData, aLength);
    return readJpeg(memoryReader,
                   aOutputImageformat,
                   aOutputImageChannelDepth);
}

void JpegIO::write(const Image& aImage, std::ostream& aPngDataStream)
{
    StreamWriter streamWriter(aPngDataStream);
    writeJpeg(streamWriter, aImage);
}

void JpegIO::write(const Image& aImage, uint8_t* aData, size_t aLength)
{
    MemoryWriter streamWriter(aData, aLength);
    writeJpeg(streamWriter, aImage);
}

} // namespace ImgIO
// EOF
