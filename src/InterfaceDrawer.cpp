
#include "InterfaceDrawer.h"
#include "UIContext.h"
#include "ImGuiImpl/ImGuiImpl.h"

using namespace animator;

namespace
{
    constexpr int padding = 20;
    constexpr int window_width = 300;

    void DrawOverlayToolbar(animator::UIContext& context)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        const ImVec4& default_color = style.Colors[ImGuiCol_Button];
        const ImVec4& hovered_color = style.Colors[ImGuiCol_ButtonHovered];

        const ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove;

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

        if (ImGui::Begin("Example: Simple overlay", nullptr, window_flags))
        {
            const ImVec4& color = (context.offset_mode) ? hovered_color : default_color;
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            if(ImGui::Button("Show Grid"))
                context.toggle_offset_mode();
            ImGui::PopStyleColor();

            ImGui::SameLine();

            if(ImGui::SliderFloat("Speed", &context.update_speed, 0.1f, 2.0f, "%.1f x"))
                context.set_speed(context.update_speed);
        }
        
        ImGui::End();
    }

    void DrawAnimationWindow(animator::UIContext& context)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.0f;
        style.WindowRounding = 0.0f;

        const ImVec2& window_size = ImGui::GetIO().DisplaySize;

        const int animation_window_height = window_size.y; // - (padding * 6);
        const int window_x = window_size.x - window_width;

        //const ImageCoords& add_icon = QuadToImageCoords(m_context.add_icon);
        const ImageCoords& delete_icon = QuadToImageCoords(context.delete_icon);
        const ImageCoords& plus_icon = QuadToImageCoords(context.plus_icon);
        const ImageCoords& save_icon = QuadToImageCoords(context.save_icon);

        const ImVec4 bg_color(1.0f, 1.0f, 1.0f, 1.0f);
        const ImVec2 small_button_size(22, 22);
        const ImVec2 number_button_size(25, 0);
        const ImVec2 button_size(50, 21);

        const ImVec4& default_color = style.Colors[ImGuiCol_Button];
        const ImVec4& hovered_color = style.Colors[ImGuiCol_ButtonHovered];
        const ImVec4& window_bg_color = style.Colors[ImGuiCol_WindowBg];

        void* texture_id = reinterpret_cast<void*>(context.tools_texture_id);

        ImGui::SetNextWindowPos(ImVec2(window_x, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(window_width, animation_window_height));

        ImGui::Begin("Animation", nullptr, ImGuiWindowFlags_NoDecoration);
        
        if(ImGui::Button("New", button_size))
            context.add_animation();

        ImGui::SameLine();
        if(ImGui::Button("Delete", button_size))
            context.delete_animation();

        ImGui::SameLine();
        ImGui::PushID(111);
        if(ImGui::ImageButton(texture_id, small_button_size, save_icon.uv1, save_icon.uv2, 0, window_bg_color))
            context.on_save();
        ImGui::PopID();

        ImGui::Spacing();
        //ImGui::Separator();
        ImGui::Spacing();

        for(int index = 0; index < context.n_animations; ++index)
        {
            char buffer[32] = { 0 };
            snprintf(buffer, 32, "%i", index);

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
        ImGui::SameLine();
        if(ImGui::Checkbox("Loop", &context.loop_animation))
            context.toggle_loop(context.loop_animation);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Text("Frames");
        ImGui::SameLine();

        ImGui::PushID(222);
        if(ImGui::ImageButton(texture_id, small_button_size, plus_icon.uv1, plus_icon.uv2, 0, window_bg_color))
            context.add_frame();
        ImGui::PopID();

        ImGui::Separator();
        ImGui::BeginChild(666);

        for(size_t index = 0; index < context.frames->size(); ++index)
        {
            mono::SpriteAnimation::Frame& frame = context.frames->at(index);

            ImGui::Spacing();
            ImGui::PushID(index);

            ImGui::SliderInt("frame", &frame.frame, 0, context.max_frames -1);
            ImGui::SameLine(0.0f, 20.0f);

            if(ImGui::ImageButton(texture_id, small_button_size, delete_icon.uv1, delete_icon.uv2, 0, window_bg_color))
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

        ImGui::Begin("Frames", nullptr, animation_flags);

        for(int index = 0; index < context.max_frames; ++index)
        {
            char buffer[32] = { 0 };
            snprintf(buffer, 32, "%i", index);

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

void InterfaceDrawer::Update(const mono::UpdateContext& update_context)
{
    ImGui::GetIO().DeltaTime = float(update_context.delta_ms) / 1000.0f;
    ImGui::NewFrame();

    DrawOverlayToolbar(m_context);
    DrawAnimationWindow(m_context);
    //DrawActiveFrameWindow(m_context);

    //ImGui::ShowDemoWindow();
    //ImGui::ShowStyleEditor();

    ImGui::Render();
}
