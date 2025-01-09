#pragma once

#include "StreamWriter.hpp"
#include "StreamReader.hpp"

#include <filesystem>
#include <fstream>

namespace Utopia
{
    class FileStreamWriter : public StreamWriter
    {
    public:
        explicit FileStreamWriter(const std::filesystem::path& path);
        FileStreamWriter(const FileStreamWriter&) = delete;
        FileStreamWriter(FileStreamWriter&&) = delete;
        FileStreamWriter& operator=(const FileStreamWriter&) = delete;
        FileStreamWriter& operator=(FileStreamWriter&&) = delete;

        ~FileStreamWriter() noexcept override;

        [[nodiscard]] bool IsStreamGood() const override { return m_Stream.good(); }
        [[nodiscard]] uint64_t GetStreamPosition() override
        {
            return static_cast<uint64_t>(m_Stream.tellp());
        }
        void SetStreamPosition(uint64_t position) override
        {
            m_Stream.seekp(static_cast<std::streamoff>(position));
        }
        [[nodiscard]] bool WriteData(const char* data, size_t size) override;

    private:
        std::filesystem::path m_Path;
        std::ofstream m_Stream;
    };

    class FileStreamReader : public StreamReader
    {
    public:
        explicit FileStreamReader(const std::filesystem::path& path);
        FileStreamReader(const FileStreamReader&) = delete;
        FileStreamReader(FileStreamReader&&) = delete;
        FileStreamReader& operator=(const FileStreamReader&) = delete;
        FileStreamReader& operator=(FileStreamReader&&) = delete;

        ~FileStreamReader() noexcept override;

        [[nodiscard]] bool IsStreamGood() const override { return m_Stream.good(); }
        [[nodiscard]] uint64_t GetStreamPosition() override
        {
            return static_cast<uint64_t>(m_Stream.tellg());
        }
        void SetStreamPosition(uint64_t position) override
        {
            m_Stream.seekg(static_cast<std::streamoff>(position));
        }
        [[nodiscard]] bool ReadData(char* destination, size_t size) override;

    private:
        std::filesystem::path m_Path;
        std::ifstream m_Stream;
    };

} // namespace Utopia
