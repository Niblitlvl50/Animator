
#include "WriteSpriteFile.h"
#include "System/File.h"

#include "nlohmann/json.hpp"

void animator::WriteSpriteFile(const char* sprite_file, const mono::SpriteData* sprite_data)
{
    file::FilePtr input_file = file::OpenAsciiFile(sprite_file);
    if(!input_file)
        return;

    std::vector<byte> file_data;
    file::FileRead(input_file, file_data);

    input_file = nullptr;

    nlohmann::json json = nlohmann::json::parse(file_data);
    nlohmann::json& json_animations = json["animations"];
    json_animations.clear();

    for(const mono::SpriteAnimation& animation : sprite_data->animations)
    {
        std::vector<int> values;

        for(const mono::SpriteAnimation::Frame& frame : animation.frames)
        {
            values.push_back(frame.frame);
            values.push_back(frame.duration);
        }

        nlohmann::json object;
        object["name"] = animation.name;
        object["loop"] = animation.looping;
        object["frame_duration"] = animation.frame_duration;
        object["frames"] = values;

        json_animations.push_back(object);
    }

    nlohmann::json& json_frames = json["frames"];
    for(size_t index = 0; index < sprite_data->frames.size(); ++index)
    {
        const mono::SpriteFrame& frame = sprite_data->frames[index];
        json_frames[index]["x_offset"] = frame.center_offset.x;
        json_frames[index]["y_offset"] = frame.center_offset.y;
    }

    const std::string& serialized_sprite = json.dump(4);
    file::FilePtr output_file = file::CreateAsciiFile(sprite_file);
    std::fwrite(serialized_sprite.data(), serialized_sprite.length(), sizeof(char), output_file.get());
}