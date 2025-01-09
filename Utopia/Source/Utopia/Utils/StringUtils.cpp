#include "StringUtils.hpp"

namespace Utopia::Utils {

    std::vector<std::string> SplitString(std::string_view string, std::string_view delimiters)
    {
        std::vector<std::string> result;
        result.reserve(8); // (Optional) Pre-allocate if you expect some typical size

        std::size_t first = 0;
        while (first <= string.size())
        {
            const auto second = string.find_first_of(delimiters, first);
            if (first != second)
            {
                // `std::string_view::npos` is (size_t)-1, so subtract is safe as long as second >= first
                auto length = (second == std::string_view::npos)
                    ? string.size() - first
                    : second - first;

                result.emplace_back(string.substr(first, length));
            }

            if (second == std::string_view::npos)
                break;

            first = second + 1;
        }

        return result;
    }

    std::vector<std::string> SplitString(std::string_view string, char delimiter)
    {
        return SplitString(string, std::string_view(&delimiter, 1));
    }

} // namespace Utopia::Utils
