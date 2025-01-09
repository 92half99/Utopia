#include "Utopia/Application.hpp"
#include "Utopia/EntryPoint.hpp"

#include "Utopia/Image.hpp"
#include "Utopia/UI/UI.hpp"

class ExampleLayer : public Utopia::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Hello");
		ImGui::Button("Button");
		ImGui::End();

		ImGui::ShowDemoWindow();

		UI_DrawAboutModal();
	}

	void UI_DrawAboutModal()
	{
		if (!m_AboutModalOpen)
			return;

		ImGui::OpenPopup("About");
		m_AboutModalOpen = ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		if (m_AboutModalOpen)
		{
			auto image = Utopia::Application::Get().GetApplicationIcon();
			ImGui::Image((ImTextureID)image->GetDescriptorSet(), { 48, 48 });

			ImGui::SameLine();
			Utopia::UI::ShiftCursorX(20.0f);

			ImGui::BeginGroup();
			ImGui::Text("Utopia application framework");
			ImGui::Text("by 92half99.");
			ImGui::EndGroup();

			if (Utopia::UI::ButtonCentered("Close"))
			{
				m_AboutModalOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void ShowAboutModal()
	{
		m_AboutModalOpen = true;
	}
private:
	bool m_AboutModalOpen = false;
};

Utopia::Application* Utopia::CreateApplication(int argc, char** argv)
{
	Utopia::ApplicationSpecification spec;
	spec.Name = "Utopia Example";
	spec.CustomTitlebar = true;

	Utopia::Application* app = new Utopia::Application(spec);
	std::shared_ptr<ExampleLayer> exampleLayer = std::make_shared<ExampleLayer>();
	app->PushLayer(exampleLayer);
	app->SetMenubarCallback([app, exampleLayer]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About"))
				{
					exampleLayer->ShowAboutModal();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}