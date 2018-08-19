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

#ifndef _DATAIO_H__
#define _DATAIO_H__

#include <cstring>
#include <iostream>

namespace ImgIO
{

class DataReader
{
public:
    DataReader() {}
    virtual ~DataReader() {}

    virtual size_t read(uint8_t* data, size_t length) = 0;
    virtual size_t tellPos() const = 0;
    virtual void seekPos(ssize_t aPosDiff) = 0;
    virtual void clearErrors() = 0;
}; // class DataReader

class StreamReader : public DataReader
{
public:
    StreamReader(std::istream& aStream)
    : mStream(aStream)
    {}

    size_t read(uint8_t* aData, size_t aLength)
    {
        mStream.read(reinterpret_cast<char*>(aData), aLength);
        return mStream.gcount();
    }

    size_t tellPos() const
    {
        return mStream.tellg();
    }

    void seekPos(ssize_t aPosDiff)
    {
        mStream.seekg(aPosDiff);
    }

    void clearErrors()
    {
        mStream.clear();
    }
private:
    std::istream& mStream;
}; // class StreamReader

class MemoryReader : public DataReader
{
public:
    MemoryReader(const uint8_t* aData, size_t aLength)
    : mData(aData), mReadPos(0), mLength(aLength)
    {}

    size_t read(uint8_t* aData, size_t aLength)
    {
        if ((mReadPos + aLength) > mLength) {
            aLength = mLength - mReadPos;
        }

        std::memcpy(aData, mData + mReadPos, aLength);
        mReadPos += aLength;

        return aLength;
    }

    size_t tellPos() const
    {
        return mReadPos;
    }

    void seekPos(ssize_t aPosDiff)
    {
        if ((static_cast<ssize_t>(mReadPos) + aPosDiff) < 0)
            mReadPos = 0;
        else if ((mReadPos + aPosDiff) > mLength)
            mReadPos = mLength;
        else
            mReadPos += aPosDiff;
    }

    void clearErrors()
    {
    }

private:
    const uint8_t* mData;
    size_t mReadPos;
    size_t mLength;
}; // class MemoryReader

class DataWriter
{
public:
    DataWriter() {}
    virtual ~DataWriter() {}

    virtual size_t write(const uint8_t* data, size_t length) = 0;
    virtual void flush() = 0;
}; // class DataWriter

class StreamWriter : public DataWriter
{
public:
    StreamWriter(std::ostream& aStream)
            : mStream(aStream)
    {}

    size_t write(const uint8_t* aData, size_t aLength)
    {
        mStream.write(reinterpret_cast<const char*>(aData), aLength);
        return aLength;
    }

    void flush()
    {
        mStream.flush();
    }
private:
    std::ostream& mStream;
}; // class StreamWriter

class MemoryWriter : public DataWriter
{
public:
    MemoryWriter(uint8_t* aData, size_t aLength)
            : mData(aData), mLength(aLength)
    {}

    size_t write(const uint8_t* aData, size_t aLength)
    {
        if (aLength > mLength) {
            aLength = mLength;
        }

        std::memcpy(mData, aData, aLength);
        mData += aLength;
        mLength -= aLength;

        return aLength;
    }

    void flush()
    {
    }
private:
    uint8_t* mData;
    size_t mLength;
}; // class MemoryWriter

}; // namespace ImgIO

#endif // _DATAIO_H__
// EOF
