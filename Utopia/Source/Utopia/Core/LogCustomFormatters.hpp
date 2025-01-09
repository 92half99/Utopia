#pragma once

#include <glm/glm.hpp>
#include <spdlog/fmt/fmt.h>

namespace fmt {

    // -------------------------
    // glm::vec2
    // -------------------------
    template<>
    struct formatter<glm::vec2>
    {
        char presentation = 'f';

        constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
        {
            auto it = ctx.begin();
            const auto end = ctx.end();
            if (it != end && (*it == 'f' || *it == 'e'))
                presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format for glm::vec2");

            return it;
        }

        template <typename FormatContext>
        auto format(const glm::vec2& vec, FormatContext& ctx) const -> decltype(ctx.out())
        {
            if (presentation == 'f')
                return fmt::format_to(ctx.out(), "({:.3f}, {:.3f})", vec.x, vec.y);
            else
                return fmt::format_to(ctx.out(), "({:.3e}, {:.3e})", vec.x, vec.y);
        }
    };

    // -------------------------
    // glm::vec3
    // -------------------------
    template<>
    struct formatter<glm::vec3>
    {
        char presentation = 'f';

        constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
        {
            auto it = ctx.begin();
            const auto end = ctx.end();
            if (it != end && (*it == 'f' || *it == 'e'))
                presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format for glm::vec3");

            return it;
        }

        template <typename FormatContext>
        auto format(const glm::vec3& vec, FormatContext& ctx) const -> decltype(ctx.out())
        {
            if (presentation == 'f')
                return fmt::format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f})", vec.x, vec.y, vec.z);
            else
                return fmt::format_to(ctx.out(), "({:.3e}, {:.3e}, {:.3e})", vec.x, vec.y, vec.z);
        }
    };

    // -------------------------
    // glm::vec4
    // -------------------------
    template<>
    struct formatter<glm::vec4>
    {
        char presentation = 'f';

        constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
        {
            auto it = ctx.begin();
            const auto end = ctx.end();
            if (it != end && (*it == 'f' || *it == 'e'))
                presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format for glm::vec4");

            return it;
        }

        template <typename FormatContext>
        auto format(const glm::vec4& vec, FormatContext& ctx) const -> decltype(ctx.out())
        {
            if (presentation == 'f')
            {
                return fmt::format_to(ctx.out(),
                    "({:.3f}, {:.3f}, {:.3f}, {:.3f})",
                    vec.x, vec.y, vec.z, vec.w
                );
            }
            else
            {
                return fmt::format_to(ctx.out(),
                    "({:.3e}, {:.3e}, {:.3e}, {:.3e})",
                    vec.x, vec.y, vec.z, vec.w
                );
            }
        }
    };

} // namespace fmt
