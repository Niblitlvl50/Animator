
#include "WriteSpriteFile.h"
#include "System/File.h"
#include "Rendering/Sprite/AnimationSequence.h"
#include "nlohmann/json.hpp"

void animator::WriteSpriteFile(const char* sprite_file, const std::vector<mono::AnimationSequence>& animations)
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

    for(const mono::AnimationSequence& animation : animations)
    {
        std::vector<int> values;

        for(const mono::Frame& frame : animation.GetFrames())
        {
            values.push_back(frame.frame);
            values.push_back(frame.duration);
        }

        nlohmann::json object;
        object["name"] = animation.GetName();
        object["loop"] = animation.IsLooping();
        object["frames"] = values;

        json_animations.push_back(object);
    }

    const std::string& serialized_sprite = json.dump(4);
    file::FilePtr output_file = file::CreateAsciiFile(sprite_file);
    std::fwrite(serialized_sprite.data(), serialized_sprite.length(), sizeof(char), output_file.get());
}