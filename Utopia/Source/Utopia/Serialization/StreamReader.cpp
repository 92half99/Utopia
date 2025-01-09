#include "StreamReader.hpp"

namespace Utopia
{
	bool StreamReader::ReadBuffer(Buffer& buffer, uint32_t size)
	{
		buffer.Size = size;
		if (size == 0)
		{
			if (!ReadData(reinterpret_cast<char*>(&buffer.Size), sizeof(uint32_t)))
				return false;
		}

		buffer.Allocate(buffer.Size);
		return ReadData(reinterpret_cast<char*>(buffer.Data), buffer.Size);
	}

	bool StreamReader::ReadString(std::string& string)
	{
		size_t size = 0;
		if (!ReadData(reinterpret_cast<char*>(&size), sizeof(size_t)))
			return false;

		string.resize(size);
		return ReadData(reinterpret_cast<char*>(string.data()), sizeof(char) * size);
	}

} // namespace Utopia
