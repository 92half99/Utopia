#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <format>
#include <functional>

#include <imgui.h>

namespace Utopia::UI {

    class Console
    {
    public:
        using MessageSendCallback = std::function<void(std::string_view)>;

    public:
        explicit Console(std::string_view title = "Utopia Console");
        ~Console() = default;

        void ClearLog();

        template<typename... Args>
        void AddMessage(std::string_view fmt, Args&&... args)
        {
            std::string msgString = std::vformat(fmt, std::make_format_args(args...));
            m_MessageHistory.emplace_back(msgString);
        }

        template<typename... Args>
        void AddItalicMessage(std::string_view fmt, Args&&... args)
        {
            std::string msgString = std::vformat(fmt, std::make_format_args(args...));
            MessageInfo info(msgString);
            info.Italic = true;
            m_MessageHistory.push_back(info);
        }

        template<typename... Args>
        void AddTaggedMessage(std::string_view tag, std::string_view fmt, Args&&... args)
        {
            std::string msgString = std::vformat(fmt, std::make_format_args(args...));
            m_MessageHistory.emplace_back(std::string(tag), msgString);
        }

        template<typename... Args>
        void AddMessageWithColor(uint32_t color, std::string_view fmt, Args&&... args)
        {
            std::string msgString = std::vformat(fmt, std::make_format_args(args...));
            m_MessageHistory.emplace_back(msgString, color);
        }

        template<typename... Args>
        void AddItalicMessageWithColor(uint32_t color, std::string_view fmt, Args&&... args)
        {
            std::string msgString = std::vformat(fmt, std::make_format_args(args...));
            MessageInfo info(msgString, color);
            info.Italic = true;
            m_MessageHistory.push_back(info);
        }

        template<typename... Args>
        void AddTaggedMessageWithColor(uint32_t color, std::string_view tag, std::string_view fmt, Args&&... args)
        {
            std::string msgString = std::vformat(fmt, std::make_format_args(args...));
            m_MessageHistory.emplace_back(std::string(tag), msgString, color);
        }

        void OnUIRender();

        void SetMessageSendCallback(const MessageSendCallback& callback);

    private:
        struct MessageInfo
        {
            std::string Tag;
            std::string Message;
            bool Italic = false;
            uint32_t Color = 0xffffffff;

            explicit MessageInfo(const std::string& msg, uint32_t color_ = 0xffffffff)
                : Message(msg), Color(color_) {
            }

            MessageInfo(const std::string& tag, const std::string& msg, uint32_t color_ = 0xffffffff)
                : Tag(tag), Message(msg), Color(color_) {
            }
        };

        std::string m_Title;
        std::string m_MessageBuffer;
        std::vector<MessageInfo> m_MessageHistory;
        ImGuiTextFilter m_Filter;
        bool m_AutoScroll = true;
        bool m_ScrollToBottom = false;

        MessageSendCallback m_MessageSendCallback;
    };

} // namespace Utopia::UI
