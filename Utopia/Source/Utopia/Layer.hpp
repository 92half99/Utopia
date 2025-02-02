#pragma once

namespace Utopia {

    class Layer
    {
    public:
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}

        virtual void OnUpdate(float /*ts*/) {}
        virtual void OnRender() {}
        virtual void OnUIRender() {}
    };

} // namespace Utopia
