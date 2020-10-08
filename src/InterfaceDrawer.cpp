
#include "InterfaceDrawer.h"
#include "UIContext.h"
#include "ImGuiImpl/ImGuiImpl.h"

using namespace animator;

namespace
{
    constexpr int window_width = 300;

    void DrawOverlayToolbar(animator::UIContext& context)
    {
        ImGuiStyle& style = ImGui::GetStyle();

        const ImageCoords& delete_icon = QuadToImageCoords(context.delete_icon);
        const ImageCoords& plus_icon = QuadToImageCoords(context.plus_icon);
        const ImageCoords& save_icon = QuadToImageCoords(context.save_icon);
        void* texture_id = reinterpret_cast<void*>(context.tools_texture_id);
        const ImVec2 small_button_size(22, 22);

        const ImVec4& default_color = style.Colors[ImGuiCol_Button];
        const ImVec4& hovered_color = style.Colors[ImGuiCol_ButtonHovered];
        const ImVec4& window_bg_color = style.Colors[ImGuiCol_WindowBg];

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

            ImGui::PushID(1);
            if(ImGui::Button("||"))
            //if(ImGui::ImageButton(texture_id, small_button_size, delete_icon.uv1, delete_icon.uv2, 0))
                context.set_paused();
            ImGui::PopID();

            ImGui::SameLine();

            ImGui::PushID(2);
            //if(ImGui::ImageButton(texture_id, small_button_size, plus_icon.uv1, plus_icon.uv2, 0))
            if(ImGui::Button("|>"))
                context.set_playing();
            ImGui::PopID();

            ImGui::SameLine();

            if(ImGui::SliderFloat("Speed", &context.update_speed, 0.1f, 2.0f, "%.1f x"))
                context.set_speed(context.update_speed);
        }
        
        ImGui::End();
    }

    void DrawAnimationWindow(animator::UIContext& context)
    {
        ImGuiStyle& style = ImGui::GetStyle();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        const ImVec2& window_size = ImGui::GetIO().DisplaySize;

        const int animation_window_height = window_size.y;
        const int window_x = window_size.x - window_width;

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
        ImGui::TextDisabled("ANIMATIONS");
        ImGui::Spacing();

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
        ImGui::Spacing();

        ImGui::BeginChild("asdfasd", ImVec2(-1, 40), false, ImGuiWindowFlags_HorizontalScrollbar);

        for(int index = 0; index < context.n_animations; ++index)
        {
            const bool is_selected_index = (index == context.animation_id);

            char buffer[32] = { 0 };
            snprintf(buffer, 32, "%i", index);

            const ImVec4& color = is_selected_index ? hovered_color : default_color;

            ImGui::PushStyleColor(ImGuiCol_Button, color);
            if(ImGui::Button(buffer, number_button_size))
                context.set_active_animation(index);

            if(is_selected_index)
                ImGui::SetScrollHereX();

            ImGui::PopStyleColor();
            ImGui::SameLine();
        }

        ImGui::EndChild();

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        char buffer[100] = { 0 };
        snprintf(buffer, 100, "%s", context.animation_name);
        if(ImGui::InputText("", buffer, 100))
            context.set_name(buffer);

        ImGui::SameLine();
        if(ImGui::Checkbox("Loop", &context.animation_looping))
            context.toggle_loop(context.animation_looping);

        if(ImGui::DragInt("Frame Rate", &context.animation_frame_rate, 1.0f, 10, 1000))
            context.set_frame_rate(context.animation_frame_rate);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("FRAMES");
        ImGui::SameLine();

        ImGui::PushID(222);
        if(ImGui::ImageButton(texture_id, small_button_size, plus_icon.uv1, plus_icon.uv2, 0, window_bg_color))
            context.add_frame();
        ImGui::PopID();

        ImGui::Separator();
        ImGui::BeginChild(666);

        const float second_column_width = 160.0f;
        const float second_column_item_width = second_column_width - style.ItemSpacing.x * 2.0f;

        ImGui::Columns(3, "animation_data", false);
        ImGui::SetColumnWidth(0, 50.0f);
        ImGui::SetColumnWidth(1, second_column_width);
        //ImGui::SetColumnWidth(2, 25.0f);

        for(size_t index = 0; index < context.frames->size(); ++index)
        {
            mono::SpriteAnimation::Frame& frame = context.frames->at(index);
            const std::string string_index = std::to_string(index + 1);

            ImGui::PushID(index);
            ImGui::AlignTextToFramePadding();
            bool selected = (index == (size_t)context.selected_frame);
            ImGui::Selectable(
                string_index.c_str(),
                &selected,
                ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap); //, ImVec2(0, 22));

            ImGui::NextColumn();
            ImGui::SetNextItemWidth(second_column_item_width);
            ImGui::SliderInt("", &frame.frame, 0, context.max_frames -1);
            ImGui::NextColumn();

            //if(ImGui::ImageButton(texture_id, ImVec2(16, 16), delete_icon.uv1, delete_icon.uv2, 3, window_bg_color))
            //    context.delete_frame(index);

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(70, 70, 70));
            if(ImGui::Button("Delete"))
                context.delete_frame(index);
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            
            ImGui::NextColumn();
            ImGui::PopID();
            ImGui::Separator();
        }

        ImGui::EndChild();
        ImGui::End();

        ImGui::PopStyleVar(2);
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

    //ImGui::ShowDemoWindow();

    ImGui::Render();
}
