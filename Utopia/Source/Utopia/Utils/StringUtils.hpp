#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace Utopia::Utils {
    std::vector<std::string> SplitString(std::string_view string, std::string_view delimiters);
    std::vector<std::string> SplitString(std::string_view string, char delimiter);

} // namespace Utopia::Utils
