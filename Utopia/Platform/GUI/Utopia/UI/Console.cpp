#include "Console.hpp"

#include "Utopia/ApplicationGUI.hpp"
#include "misc/cpp/imgui_stdlib.h"

namespace Utopia::UI {

    Console::Console(std::string_view title)
        : m_Title(title)
    {
    }

    void Console::ClearLog()
    {
        m_MessageHistory.clear();
    }

    void Console::OnUIRender()
    {
        ImGui::SetNextWindowSize(ImVec2(520.0f, 600.0f), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(m_Title.c_str()))
        {
            ImGui::End();
            return;
        }

        // Options popup
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
            ImGui::EndPopup();
        }

        // Buttons and filter
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");

        ImGui::SameLine();
        ImGui::TextUnformatted("Search");
        ImGui::SameLine();
        m_Filter.Draw("##search", 180.0f);
        ImGui::Separator();

        // Reserve space for the input text at the bottom
        const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear"))
                ClearLog();
            ImGui::EndPopup();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tight spacing
        const float textPadding = 8.0f;

        // Render messages
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + textPadding);
        for (auto& entry : m_MessageHistory)
        {
            if (!m_Filter.PassFilter(entry.Message.c_str()))
                continue;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textPadding);

            // Set color
            ImGui::PushStyleColor(ImGuiCol_Text, ImColor(entry.Color).Value);

            // If there's a tag, show it in bold
            if (!entry.Tag.empty())
            {
                ImGui::PushFont(Application::GetFont("Bold"));
                ImGui::TextUnformatted(entry.Tag.c_str());
                ImGui::PopFont();
                ImGui::SameLine(0.0f, textPadding);
            }

            // Italic font if requested
            if (entry.Italic)
                ImGui::PushFont(Application::GetFont("Italic"));

            ImGui::TextUnformatted(entry.Message.c_str());

            if (entry.Italic)
                ImGui::PopFont();

            ImGui::PopStyleColor();
        }

        // Auto-scroll
        if (m_ScrollToBottom || (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);

        m_ScrollToBottom = false;

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::Separator();

        // Command-line input
        static bool reclaimFocus = false;
        const ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue;

        const float sendButtonWidth = 100.0f;

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - sendButtonWidth - textPadding);
        if (ImGui::InputText("##input", &m_MessageBuffer, inputTextFlags))
        {
            // On Enter: call callback
            if (m_MessageSendCallback)
                m_MessageSendCallback(m_MessageBuffer);

            m_MessageBuffer.clear();
            reclaimFocus = true;
        }

        // Re-focus on the input box
        ImGui::SetItemDefaultFocus();
        if (reclaimFocus)
        {
            ImGui::SetKeyboardFocusHere(-1);
            reclaimFocus = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Send", ImVec2(sendButtonWidth, 0.0f)))
        {
            if (m_MessageSendCallback)
                m_MessageSendCallback(m_MessageBuffer);

            m_MessageBuffer.clear();
            reclaimFocus = true;
        }

        ImGui::End();
    }

    void Console::SetMessageSendCallback(const MessageSendCallback& callback)
    {
        m_MessageSendCallback = callback;
    }

} // namespace Utopia::UI
