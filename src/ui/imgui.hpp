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
    public:
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

        Button& AddButton(
            const std::string& name,
            const std::string& id = std::string()
        );

        void Run();

    private:
        std::list<std::unique_ptr<WidgetBase>> m_widgets;
    };
}


#endif
