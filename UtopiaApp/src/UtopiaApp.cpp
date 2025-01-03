#include "Utopia/Application.hpp"
#include "Utopia/EntryPoint.hpp"
#include "Utopia/Image.hpp"

class ExampleLayer : public Utopia::Layer
{
public:
    void OnUIRender() override
    {
        ImGui::Begin("Hello");
        ImGui::Button("Button");
        ImGui::End();

        ImGui::ShowDemoWindow();
    }
};

Utopia::Application* Utopia::CreateApplication(int /*argc*/, char** /*argv*/)
{
    Utopia::ApplicationSpecification spec;
    spec.Name = "Utopia Example";

    auto* app = new Utopia::Application(spec);

    app->PushLayer<ExampleLayer>();

    app->SetMenubarCallback([app]()
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                {
                    app->Close();
                }
                ImGui::EndMenu();
            }
        });

    return app;
}
