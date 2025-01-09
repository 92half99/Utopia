#pragma once

#include <random>
#include <limits>
#include <glm/glm.hpp>

namespace Utopia {

    class Random
    {
    public:
        static void Init() noexcept
        {
            s_RandomEngine.seed(std::random_device{}());
        }

        [[nodiscard]] static auto UInt() noexcept -> std::uint32_t
        {
            return s_IntDistribution(s_RandomEngine);
        }

        [[nodiscard]] static auto UInt(std::uint32_t min, std::uint32_t max) noexcept -> std::uint32_t
        {
            std::uniform_int_distribution<std::uint32_t> dist(min, max);
            return dist(s_RandomEngine);
        }

        [[nodiscard]] static auto Float() noexcept -> float
        {
            return s_FloatDistribution(s_RandomEngine);
        }

        [[nodiscard]] static auto Vec3() noexcept -> glm::vec3
        {
            return { Float(), Float(), Float() };
        }

        [[nodiscard]] static auto Vec3(float min, float max) noexcept -> glm::vec3
        {
            const float range = max - min;
            return { Float() * range + min,
                     Float() * range + min,
                     Float() * range + min };
        }

        [[nodiscard]] static auto InUnitSphere() noexcept -> glm::vec3
        {
            return glm::normalize(Vec3(-1.0f, 1.0f));
        }

    private:
        inline static thread_local std::mt19937 s_RandomEngine{};

        inline static std::uniform_int_distribution<std::uint32_t>
            s_IntDistribution{ 0, std::numeric_limits<std::uint32_t>::max() };

        inline static std::uniform_real_distribution<float>
            s_FloatDistribution{ 0.0f, 1.0f };
    };

} // namespace Utopia
