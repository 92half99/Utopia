#pragma once

#include "StreamWriter.hpp"
#include "StreamReader.hpp"

#include "Utopia/Core/Assert.hpp"

#include <filesystem>
#include <fstream>
#include <cstring>

namespace Utopia
{
    class BufferStreamWriter : public StreamWriter
    {
    public:
        BufferStreamWriter(Buffer targetBuffer, uint64_t position = 0);
        BufferStreamWriter(const BufferStreamWriter&) = delete;
        BufferStreamWriter(BufferStreamWriter&&) = delete;
        BufferStreamWriter& operator=(const BufferStreamWriter&) = delete;
        BufferStreamWriter& operator=(BufferStreamWriter&&) = delete;

        ~BufferStreamWriter() noexcept override = default;

        [[nodiscard]] bool IsStreamGood() const override { return static_cast<bool>(m_TargetBuffer); }
        [[nodiscard]] uint64_t GetStreamPosition() override { return m_BufferPosition; }
        void SetStreamPosition(uint64_t position) override { m_BufferPosition = position; }
        [[nodiscard]] bool WriteData(const char* data, size_t size) override;

        [[nodiscard]] Buffer GetBuffer() const { return Buffer(m_TargetBuffer, m_BufferPosition); }

    private:
        Buffer m_TargetBuffer;
        uint64_t m_BufferPosition = 0;
    };

    class BufferStreamReader : public StreamReader
    {
    public:
        BufferStreamReader(Buffer targetBuffer, uint64_t position = 0);
        BufferStreamReader(const BufferStreamReader&) = delete;
        BufferStreamReader(BufferStreamReader&&) = delete;
        BufferStreamReader& operator=(const BufferStreamReader&) = delete;
        BufferStreamReader& operator=(BufferStreamReader&&) = delete;

        ~BufferStreamReader() noexcept override = default;

        [[nodiscard]] bool IsStreamGood() const override { return static_cast<bool>(m_TargetBuffer); }
        [[nodiscard]] uint64_t GetStreamPosition() override { return m_BufferPosition; }
        void SetStreamPosition(uint64_t position) override { m_BufferPosition = position; }
        [[nodiscard]] bool ReadData(char* destination, size_t size) override;

        [[nodiscard]] Buffer GetBuffer() const { return Buffer(m_TargetBuffer, m_BufferPosition); }

    private:
        Buffer m_TargetBuffer;
        uint64_t m_BufferPosition = 0;
    };

} // namespace Utopia
