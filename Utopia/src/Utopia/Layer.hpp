#pragma once

namespace Utopia {

    class Layer
    {
    public:
        virtual ~Layer() noexcept = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}

        virtual void OnUpdate(float /*ts*/) {}
        virtual void OnUIRender() {}
    };

} // namespace Utopia
