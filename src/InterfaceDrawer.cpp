
#include "InterfaceDrawer.h"
#include "UIContext.h"
#include "ImGuiImpl/ImGuiImpl.h"

using namespace animator;

namespace
{
    constexpr int padding = 20;
    constexpr int window_width = 280;

    void DrawAnimationWindow(animator::UIContext& context)
    {
        const ImVec2& window_size = ImGui::GetIO().DisplaySize;

        const int animation_window_height = window_size.y - (padding * 6);
        const int window_x = window_size.x - window_width - padding;

        //const ImageCoords& add_icon = QuadToImageCoords(m_context.add_icon);
        const ImageCoords& delete_icon = QuadToImageCoords(context.delete_icon);
        const ImageCoords& plus_icon = QuadToImageCoords(context.plus_icon);
        const ImageCoords& save_icon = QuadToImageCoords(context.save_icon);

        const ImVec4 bg_color(1.0f, 1.0f, 1.0f, 1.0f);
        const ImVec4 delete_bg_color(0.0f, 0.0f, 0.0f, 1.0f);
        const ImVec2 small_button_size(22, 22);
        const ImVec2 number_button_size(25, 0);
        const ImVec2 button_size(50, 21);

        const ImVec4& default_color = ImGui::GetStyle().Colors[ImGuiCol_Button];
        const ImVec4& hovered_color = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];

        void* texture_id = reinterpret_cast<void*>(context.tools_texture_id);

        ImGui::SetNextWindowPos(ImVec2(window_x, padding));
        ImGui::SetNextWindowSize(ImVec2(window_width, animation_window_height));

        constexpr int animation_flags = ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | 
                                        ImGuiWindowFlags_NoCollapse;
        ImGui::Begin("Animation", nullptr, ImVec2(0.0f, 0.0f), 1.0f, animation_flags);
        
        if(ImGui::Button("New", button_size))
            context.add_animation();

        ImGui::SameLine();
        if(ImGui::Button("Delete", button_size))
            context.delete_animation();

        ImGui::SameLine();
        ImGui::PushID(111);
        if(ImGui::ImageButton(texture_id, small_button_size, save_icon.uv1, save_icon.uv2, 0, delete_bg_color))
            context.on_save();
        ImGui::PopID();

        ImGui::Spacing();
        //ImGui::Separator();
        ImGui::Spacing();

        for(int index = 0; index < context.n_animations; ++index)
        {
            char buffer[10] = { 0 };
            snprintf(buffer, 10, "%i", index);

            const ImVec4& color = (index == context.animation_id) ? hovered_color : default_color;

            ImGui::PushStyleColor(ImGuiCol_Button, color);
            if(ImGui::Button(buffer, number_button_size))
                context.set_active_animation(index);

            ImGui::PopStyleColor();
            ImGui::SameLine();
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        char buffer[100] = { 0 };
        snprintf(buffer, 100, "%s", context.display_name);
        if(ImGui::InputText("", buffer, 100))
            context.set_name(buffer);

        ImGui::Spacing();
        if(ImGui::Checkbox("Loop", &context.loop_animation))
            context.toggle_loop(context.loop_animation);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::AlignFirstTextHeightToWidgets();
        ImGui::Text("Frames");
        ImGui::SameLine();

        ImGui::PushID(222);
        if(ImGui::ImageButton(texture_id, small_button_size, plus_icon.uv1, plus_icon.uv2, 0, delete_bg_color))
            context.add_frame();
        ImGui::PopID();

        ImGui::Separator();
        ImGui::BeginChild(666);

        for(size_t index = 0; index < context.frames->size(); ++index)
        {
            mono::Frame& frame = context.frames->at(index);

            ImGui::Spacing();

            ImGui::PushID(index);
            ImGui::SliderInt("frame", &frame.frame, 0, context.max_frames -1);

            ImGui::SameLine(0.0f, 20.0f);
            if(ImGui::ImageButton(texture_id, small_button_size, delete_icon.uv1, delete_icon.uv2, 0, delete_bg_color))
                context.delete_frame(index);
            ImGui::DragInt("duration", &frame.duration, 10.0f, 10, 1000, "%.0f ms");
            ImGui::PopID();

            ImGui::Spacing();
            ImGui::Separator();
        }

        ImGui::EndChild();
        ImGui::End();
    }

    void DrawActiveFrameWindow(animator::UIContext& context)
    {
        const ImVec4& default_color = ImGui::GetStyle().Colors[ImGuiCol_Button];
        const ImVec4& hovered_color = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];

        const ImVec2& window_size = ImGui::GetIO().DisplaySize;
        const int animation_window_height = window_size.y - (padding * 6);
        const int window_x = window_size.x - window_width - padding;

        const ImVec2 number_button_size(25, 0);

        ImGui::SetNextWindowPos(ImVec2(window_x, animation_window_height + padding * 2));
        ImGui::SetNextWindowSize(ImVec2(window_width, padding * 3));

        constexpr int animation_flags = ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | 
                                        ImGuiWindowFlags_NoCollapse;
        ImGui::Begin("Frames", nullptr, ImVec2(0.0f, 0.0f), 1.0f, animation_flags);

        for(int index = 0; index < context.max_frames; ++index)
        {
            char buffer[10] = { 0 };
            snprintf(buffer, 10, "%i", index);

            const ImVec4& color = (index == context.selected_frame) ? hovered_color : default_color;

            ImGui::PushStyleColor(ImGuiCol_Button, color);
            if(ImGui::Button(buffer, number_button_size))
                context.set_active_frame(index);

            ImGui::PopStyleColor();
            ImGui::SameLine();
        }

        ImGui::End();
    }
}

InterfaceDrawer::InterfaceDrawer(UIContext& context)
    : m_context(context)
{ }

void InterfaceDrawer::doUpdate(unsigned int delta)
{
    ImGui::GetIO().DeltaTime = float(delta) / 1000.0f;
    ImGui::NewFrame();

    DrawAnimationWindow(m_context);
    DrawActiveFrameWindow(m_context);

    //ImGui::ShowDemoWindow();
    //ImGui::ShowStyleEditor();

    ImGui::Render();
}
