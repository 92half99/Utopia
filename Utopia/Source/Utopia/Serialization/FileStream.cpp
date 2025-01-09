#include "FileStream.hpp"

namespace Utopia
{
    FileStreamWriter::FileStreamWriter(const std::filesystem::path& path)
        : m_Path(path)
        , m_Stream(path, std::ios::out | std::ios::binary)
    {
    }

    FileStreamWriter::~FileStreamWriter() noexcept
    {
        if (m_Stream.is_open())
            m_Stream.close();
    }

    bool FileStreamWriter::WriteData(const char* data, size_t size)
    {
        m_Stream.write(data, static_cast<std::streamsize>(size));
        if (m_Stream.fail())
        {
            return false;
        }
        return true;
    }

    FileStreamReader::FileStreamReader(const std::filesystem::path& path)
        : m_Path(path)
        , m_Stream(path, std::ios::in | std::ios::binary)
    {
    }

    FileStreamReader::~FileStreamReader() noexcept
    {
        if (m_Stream.is_open())
            m_Stream.close();
    }

    bool FileStreamReader::ReadData(char* destination, size_t size)
    {
        m_Stream.read(destination, static_cast<std::streamsize>(size));
        if (m_Stream.fail())
        {
            return false;
        }
        return true;
    }

} // namespace Utopia
