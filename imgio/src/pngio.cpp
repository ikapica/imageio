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

#include "pngio.h"
#include <iostream>
#include <png.h>

#include "dataio.h"

#define PNGSIGSIZE 8

namespace ImgIO
{


static void errorHandler(png_structp pngPtr, png_const_charp errorMsg)
{
    throw std::logic_error("PNG decode error: " + std::string(errorMsg));
}

static void pngWarningHandler(png_structp pngPtr, png_const_charp warningMsg)
{
}

static void readDataHandler(png_structp pngPtr, png_bytep data, png_size_t length)
{
    reinterpret_cast<DataReader*>(png_get_io_ptr(pngPtr))->read(reinterpret_cast<uint8_t*>(data), length);
}

static void writeDataHandler(png_structp pngPtr, png_bytep data, png_size_t length)
{
    reinterpret_cast<DataWriter*>(png_get_io_ptr(pngPtr))->write(reinterpret_cast<uint8_t*>(data), length);
}

static void flushDataHandler(png_structp pngPtr)
{
    reinterpret_cast<DataWriter*>(png_get_io_ptr(pngPtr))->flush();
}

static Image readPng(DataReader& aDataReader,
                     Color::Format aOutputImageformat,
                     Color::ChannelDepth aOutputImageChannelDepth)
{
    png_byte pngSig[PNGSIGSIZE];

    if ((aDataReader.read((uint8_t*)pngSig, PNGSIGSIZE) != PNGSIGSIZE) || (png_sig_cmp(pngSig, 0, PNGSIGSIZE) != 0)) {
        throw std::logic_error("Not a PNG");
    }

    std::unique_ptr<png_struct, std::function<void(png_structp)>> pngImage(nullptr,
       [](png_structp pngImagePtr) { png_destroy_read_struct(&pngImagePtr, nullptr, nullptr); });
    {
        png_structp pngImagePtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, errorHandler, pngWarningHandler);
        if (!pngImagePtr) {
            throw std::logic_error("PNG decoder internal error");
        }
        pngImage.reset(pngImagePtr);
    }

    std::unique_ptr<png_info, std::function<void(png_infop)>> pngImageInfo(nullptr,
                                                                           [](png_infop pngImageInfoPtr) { png_destroy_read_struct(nullptr, &pngImageInfoPtr, nullptr); });
    {
        png_infop pngImageInfoPtr = png_create_info_struct(pngImage.get());
        if (!pngImageInfoPtr) {
            throw std::logic_error("Couldn't initialize png info struct");
        }
        pngImageInfo.reset(pngImageInfoPtr);
    }

    png_set_read_fn(pngImage.get(),
                    reinterpret_cast<png_voidp>(&aDataReader),
                    readDataHandler);

    png_set_sig_bytes(pngImage.get(), PNGSIGSIZE);
    png_read_info(pngImage.get(), pngImageInfo.get());

    png_uint_32 pngImageWidth =  png_get_image_width(pngImage.get(), pngImageInfo.get());
    png_uint_32 pngImageHeight = png_get_image_height(pngImage.get(), pngImageInfo.get());
    png_uint_32 pngImageChannelDepth = png_get_bit_depth(pngImage.get(), pngImageInfo.get());
    png_uint_32 pngImageChannels = png_get_channels(pngImage.get(), pngImageInfo.get());
    png_uint_32 pngImageFormat = png_get_color_type(pngImage.get(), pngImageInfo.get());

    switch (pngImageFormat) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(pngImage.get());
            pngImageChannels = 3;
            break;
        case PNG_COLOR_TYPE_GRAY:
            if (pngImageChannelDepth < 8)
                png_set_expand_gray_1_2_4_to_8(pngImage.get());
            pngImageChannelDepth = 8;
            break;
    }

    // Convert transparent color to alpha channel
    if (png_get_valid(pngImage.get(), pngImageInfo.get(), PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(pngImage.get());
        pngImageChannels += 1;
    }

    if ((pngImageChannelDepth == 16) && (aOutputImageChannelDepth == Color::ChannelDepth::k8Bit)) {
        png_set_strip_16(pngImage.get());
    }

    if ((aOutputImageformat == Color::Format::kRGBA) && (pngImageFormat == PNG_COLOR_TYPE_RGB))
    {
        png_set_add_alpha(pngImage.get(), 255, PNG_FILLER_AFTER);
        pngImageFormat = PNG_COLOR_TYPE_RGBA;
        pngImageChannels += 1;
    }

    if ((aOutputImageformat == Color::Format::kRGB) && (pngImageFormat == PNG_COLOR_TYPE_RGBA))
    {
        png_set_strip_alpha(pngImage.get());
        pngImageFormat = PNG_COLOR_TYPE_RGB;
        pngImageChannels -= 1;
    }

    png_read_update_info(pngImage.get(), pngImageInfo.get());

    std::unique_ptr<png_bytep> rowPtrs(new png_bytep[pngImageHeight]);
    std::unique_ptr<uint8_t> data(new uint8_t[pngImageWidth * pngImageHeight * pngImageChannelDepth * pngImageChannels / 8]);
    const unsigned int dataLineSize = pngImageWidth * pngImageChannelDepth * pngImageChannels / 8;

    for (size_t i = 0; i < pngImageHeight; ++i) {
        rowPtrs.get()[i] = static_cast<png_bytep>(data.get()) + i * dataLineSize;
    }

    png_read_image(pngImage.get(), rowPtrs.get());

    return Image(pngImageWidth,
                 pngImageHeight,
                 aOutputImageformat,
                 aOutputImageChannelDepth,
                 data.release());
}

static void writePng(DataWriter& aDataWriter,
                     const Image& aImage)
{
    std::unique_ptr<png_struct, std::function<void(png_structp)>> pngImage(nullptr,
                                                                           [](png_structp pngImagePtr) { png_destroy_write_struct(&pngImagePtr, nullptr); });
    {
        png_structp pngImagePtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, errorHandler, pngWarningHandler);
        if (!pngImagePtr) {
            throw std::logic_error("PNG decoder internal error");
        }
        pngImage.reset(pngImagePtr);
    }

    std::unique_ptr<png_info, std::function<void(png_infop)>> pngImageInfo(nullptr,
                                                                           [](png_infop pngImageInfoPtr) { png_destroy_write_struct(nullptr, &pngImageInfoPtr); });
    {
        png_infop pngImageInfoPtr = png_create_info_struct(pngImage.get());
        if (!pngImageInfoPtr) {
            throw std::logic_error("Couldn't initialize png info struct");
        }
        pngImageInfo.reset(pngImageInfoPtr);
    }

    png_set_write_fn(pngImage.get(),
                    reinterpret_cast<png_voidp>(&aDataWriter),
                    writeDataHandler,
                    flushDataHandler);

    int pngBitDepth = 0;
    int pngColorType = 0;

    switch(aImage.colorChannelDepth())
    {
        case Color::ChannelDepth::k8Bit:
            pngBitDepth = 8;
            break;
        case Color::ChannelDepth::k16Bit:
            pngBitDepth = 16;
            break;
        default:
            throw std::logic_error("Unsupported bit depth");
            break;
    }

    switch(aImage.colorFormat())
    {
        case Color::Format::kRGB:
            pngColorType = PNG_COLOR_TYPE_RGB;
            break;
        case Color::Format::kRGBA:
            pngColorType = PNG_COLOR_TYPE_RGBA;
            break;
        default:
            throw std::logic_error("Unsupported image colorFormat");
            break;
    }

    png_set_IHDR(pngImage.get(),
                 pngImageInfo.get(),
                 aImage.width(),
                 aImage.height(),
                 pngBitDepth,
                 pngColorType,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    png_text softwareText;
    softwareText.compression = PNG_TEXT_COMPRESSION_NONE;
    softwareText.key = const_cast<png_charp>("Software");
    softwareText.text = const_cast<png_charp>("imgio");
    png_set_text(pngImage.get(),
                 pngImageInfo.get(),
                 &softwareText, 1);

    png_write_info(pngImage.get(), pngImageInfo.get());

    const uint8_t* row = aImage.data();
    size_t rowLength = aImage.width() * static_cast<int>(aImage.colorFormat()) * static_cast<int>(aImage.colorChannelDepth());
    for (int y = 0 ; y < aImage.height() ; ++y) {
        png_write_row(pngImage.get(), row);
        row += rowLength;
    }

    png_write_end(pngImage.get(), NULL);
}

Image PngIO::read(std::istream& aPngDataStream,
                  Color::Format aOutputImageformat,
                  Color::ChannelDepth aOutputImageChannelDepth)
{
    StreamReader streamReader(aPngDataStream);
    return readPng(streamReader,
                   aOutputImageformat,
                   aOutputImageChannelDepth);
}

Image PngIO::read(const uint8_t* aData,
                  size_t aLength,
                  Color::Format aOutputImageformat,
                  Color::ChannelDepth aOutputImageChannelDepth)
{
    MemoryReader memoryReader(aData, aLength);
    return readPng(memoryReader,
                   aOutputImageformat,
                   aOutputImageChannelDepth);
}

void PngIO::write(const Image& aImage, std::ostream& aPngDataStream)
{
    StreamWriter streamWriter(aPngDataStream);
    writePng(streamWriter, aImage);
}

void PngIO::write(const Image& aImage, uint8_t* aData, size_t aLength)
{
    MemoryWriter streamWriter(aData, aLength);
    writePng(streamWriter, aImage);
}

} // namespace ImgIO
// EOF
