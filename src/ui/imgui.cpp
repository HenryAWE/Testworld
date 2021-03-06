// Author: HenryAWE
// License: The 3-clause BSD License

// Wrappers and extensions for Dear ImGui

#include "imgui.hpp"
#include <type_traits>


namespace awe::ui
{
    ImGuiBuilder::WidgetBase::WidgetBase(
        const std::string& name,
        const std::string& id
    ) : m_name_id(name), m_id(id) {
        if(!m_id.empty())
        {
            m_name_id += "##" + m_id;
        }
    }

    ImGuiBuilder::Text::Text(
        const std::string& text,
        const std::string& id = std::string()
    ) : WidgetBase(std::string(), id),
        m_text(text) {}
    void ImGuiBuilder::Text::Run(ImGuiBuilder& builder)
    {
        ImGui::TextUnformatted(
            m_text.c_str(),
            m_text.c_str() + m_text.length()
        );
    }

    ImGuiBuilder::Button& ImGuiBuilder::Button::Connect(std::function<void()> f)
    {
        m_callback.swap(f);
        return *this;
    }
    void ImGuiBuilder::Button::Run(ImGuiBuilder& builder)
    {
        bool clicked = ImGui::Button(
            GetNameId().c_str()
        );
        if(clicked && m_callback)
            m_callback();
    }

    ImGuiBuilder::MenuItem& ImGuiBuilder::MenuItem::Connect(std::function<void()> f)
    {
        m_callback.swap(f);
        return *this;
    }
    void ImGuiBuilder::MenuItem::Run(ImGuiBuilder& builder)
    {
        bool clicked = ImGui::MenuItem(
            GetNameId().c_str()
        );
        if(clicked && m_callback)
            m_callback();
    }

    ImGuiBuilder::MenuItem& ImGuiBuilder::Menu::AddMenuItem(
        const std::string& name,
        const std::string& id
    ) {
        m_widgets.emplace_back(std::make_unique<MenuItem>(
            name,
            id
        ));
        return *static_cast<MenuItem*>(m_widgets.back().get());
    }
    ImGuiBuilder::Menu& ImGuiBuilder::Menu::AddMenu(
        const std::string& name,
        const std::string& id
    ) {
        m_widgets.emplace_back(std::make_unique<Menu>(
            name,
            id
        ));
        return *static_cast<Menu*>(m_widgets.back().get());
    }


    void ImGuiBuilder::Menu::Run(ImGuiBuilder& builder)
    {
        if(!ImGui::BeginMenu(m_name_id.c_str()))
            return;
        for(auto& i : m_widgets)
        {
            i->Run(builder);
        }
        ImGui::EndMenu();
    }

    ImGuiBuilder::Text& ImGuiBuilder::AddText(
        const std::string& text,
        const std::string& id
    ) {
        m_widgets.emplace_back(std::make_unique<Text>(
            text,
            id
        ));
        return *static_cast<Text*>(m_widgets.back().get());
    }
    ImGuiBuilder::Button& ImGuiBuilder::AddButton(
        const std::string& name,
        const std::string& id
    ) {
        m_widgets.emplace_back(std::make_unique<Button>(
            name,
            id
        ));
        return *static_cast<Button*>(m_widgets.back().get());
    }
    ImGuiBuilder::Menu& ImGuiBuilder::AddMenu(
        const std::string& name,
        const std::string& id
    ) {
        m_widgets.emplace_back(std::make_unique<Menu>(
            name,
            id
        ));
        return *static_cast<Menu*>(m_widgets.back().get());
    }
    void ImGuiBuilder::AddSeparator()
    {
        class SeparatorImpl final : public WidgetBase
        {
        public:
            SeparatorImpl()
                : WidgetBase(std::string(), std::string()) {}

        protected:
            void Run(ImGuiBuilder& builder) override
            {
                ImGui::Separator();
            }
        };
        m_widgets.emplace_back(
            std::make_unique<SeparatorImpl>()
        );
    }
    ImGuiBuilder::WidgetBase& ImGuiBuilder::AddCustomWidget(
        std::unique_ptr<WidgetBase> widget
    ) {
        m_widgets.emplace_back(
            std::move(widget)
        );
        return *m_widgets.back();
    }

    void ImGuiBuilder::Run()
    {
        for(auto& i : m_widgets)
        {
            i->Run(*this);
        }
    }
}
