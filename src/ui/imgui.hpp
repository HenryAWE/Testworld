// Author: HenryAWE
// License: The 3-clause BSD License

// Wrappers and extensions for Dear ImGui

#ifndef TESTWORLD_UI_IMGUI_HPP
#define TESTWORLD_UI_IMGUI_HPP

#include <functional>
#include <list>
#include <string>
#include <variant>
#include <imgui.h>


namespace awe::ui
{
    class ImGuiBuilder
    {
    public:
        class WidgetBase
        {
            friend class ImGuiBuilder;
            std::string m_name_id; // name##id required by ImGui
            std::string m_id;

        public:
            WidgetBase(
                const std::string& name,
                const std::string& id
            );

            [[nodiscard]]
            constexpr const std::string& GetNameId() const
            {
                return m_name_id;
            }
            [[nodiscard]]
            constexpr const std::string& GetId() const
            {
                return m_name_id;
            }

        protected:
            virtual void Run(ImGuiBuilder& builder) = 0;
        };
        // Static text
        class Text : public WidgetBase
        {
            std::string m_text;
        public:
            Text(
                const std::string& text,
                const std::string& id
            );

            Text& SetText(std::string text)
            {
                m_text.swap(text);
                return *this;
            }
            [[nodiscard]]
            constexpr const std::string& GetText() noexcept
            {
                return m_text;
            }

        protected:
            void Run(ImGuiBuilder& builder) override;
        };
        class Button : public WidgetBase
        {
            friend class ImGuiBuilder;
            std::function<void()> m_callback;

        public:
            using WidgetBase::WidgetBase;

            Button& Connect(std::function<void()> f);

        protected:
            void Run(ImGuiBuilder& builder) override;
        };

        class MenuItem : public WidgetBase
        {
            std::function<void()> m_callback;
        public:
            using WidgetBase::WidgetBase;

            MenuItem& Connect(std::function<void()> f);
        protected:
            void Run(ImGuiBuilder& builder) override;
        };
        class Menu : public WidgetBase
        {
            std::list<std::unique_ptr<WidgetBase>> m_widgets;
        public:
            using WidgetBase::WidgetBase;

            MenuItem& AddMenuItem(
                const std::string& name,
                const std::string& id = std::string()
            );
            Menu& AddMenu(
                const std::string& name,
                const std::string& id = std::string()
            );

        protected:
            void Run(ImGuiBuilder& builder) override;
        };

        Text& AddText(
            const std::string& text,
            const std::string& id = std::string()
        );
        Button& AddButton(
            const std::string& name,
            const std::string& id = std::string()
        );
        Menu& AddMenu(
            const std::string& name,
            const std::string& id = std::string()
        );
        WidgetBase& AddCustomWidget(
            std::unique_ptr<WidgetBase> widget
        );

        void Run();

    private:
        std::list<std::unique_ptr<WidgetBase>> m_widgets;
    };
}


#endif
