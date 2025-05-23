#include "aleLoadSave.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <zlib.h>

namespace almond {

    void SaveSystem::SaveGame(const std::string& filename, const std::vector<Event>& events) {
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) {
            std::cerr << "Error opening file for saving!" << std::endl;
            return;
        }

        std::string data;
        for (const auto& event : events) {
            // Use EventTypeToString for serialization
            data += EventTypeToString(event.type) + ":";
            for (const auto& pair : event.data) {
                data += pair.first + "=" + pair.second + ";";
            }
            // Serialize additional fields
            data += "x=" + std::to_string(event.x) + ";";
            data += "y=" + std::to_string(event.y) + ";";
            data += "key=" + std::to_string(event.key) + ";";
            data += "text=" + std::string(event.text) + ";";
            data += "\n";
        }

        std::string compressedData = CompressData(data);
        ofs.write(compressedData.c_str(), compressedData.size());
        ofs.close();
    }

    void SaveSystem::LoadGame(const std::string& filename, std::vector<Event>& events) {
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) {
            std::cerr << "Error opening file for loading!" << std::endl;
            return;
        }

        std::string compressedData((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        std::string data = DecompressData(compressedData);

        size_t pos = 0;
        while ((pos = data.find('\n')) != std::string::npos) {
            std::string line = data.substr(0, pos);
            Event event;

            size_t typeEnd = line.find(':');
            if (typeEnd == std::string::npos) {
                data.erase(0, pos + 1);
                continue;
            }

            // Use StringToEventType for deserialization
            event.type = StringToEventType(line.substr(0, typeEnd));
            std::string details = line.substr(typeEnd + 1);

            size_t semicolonPos;
            while ((semicolonPos = details.find(';')) != std::string::npos) {
                std::string keyValue = details.substr(0, semicolonPos);
                size_t equalPos = keyValue.find('=');
                if (equalPos != std::string::npos) {
                    std::string key = keyValue.substr(0, equalPos);
                    std::string value = keyValue.substr(equalPos + 1);

                    if (key == "x") {
                        event.x = std::stof(value);
                    }
                    else if (key == "y") {
                        event.y = std::stof(value);
                    }
                    else if (key == "key") {
                        event.key = std::stoi(value);
                    }
                    else if (key == "text") {
                        strncpy_s(event.text, value.c_str(), sizeof(event.text) - 1);
                        event.text[sizeof(event.text) - 1] = '\0';
                    }
                    else {
                        event.data[key] = value;
                    }
                }
                details.erase(0, semicolonPos + 1);
            }

            events.push_back(event);
            data.erase(0, pos + 1);
        }

        ifs.close();
    }

    std::string SaveSystem::CompressData(const std::string& data) {
        uLongf compressedSize = compressBound(static_cast<uLong>(data.size()));
        std::vector<Bytef> compressedData(compressedSize);
        if (compress(compressedData.data(), &compressedSize, reinterpret_cast<const Bytef*>(data.data()), static_cast<uLong>(data.size())) != Z_OK) {
            return data;
        }
        compressedData.resize(compressedSize);
        return std::string(reinterpret_cast<char*>(compressedData.data()), compressedSize);
    }

    std::string SaveSystem::DecompressData(const std::string& compressedData) {
        uLongf decompressedSize = static_cast<uLong>(compressedData.size()) * 4;
        std::vector<Bytef> decompressedData(decompressedSize);
        while (uncompress(decompressedData.data(), &decompressedSize, reinterpret_cast<const Bytef*>(compressedData.data()), static_cast<uLong>(compressedData.size())) == Z_BUF_ERROR) {
            decompressedSize *= 2;
            decompressedData.resize(decompressedSize);
        }
        return std::string(reinterpret_cast<char*>(decompressedData.data()), decompressedSize);
    }
}
/*
#include "LoadSave.h"
#include <iostream>

int main() {
    almond::SaveSystem saveSystem;
    std::vector<Event> events = {
        {EventType::MouseButtonClick, {{"action", "click"}}, 50.0f, 100.0f, 0, "A"},
        {EventType::KeyPress, {{"action", "press"}}, 0.0f, 0.0f, 65, ""}
    };

    saveSystem.SaveGame("savegame.dat", events);

    std::vector<Event> loadedEvents;
    saveSystem.LoadGame("savegame.dat", loadedEvents);

    for (const auto& event : loadedEvents) {
        std::cout << "Type: " << EventTypeToString(event.type)
                  << ", x: " << event.x << ", y: " << event.y
                  << ", key: " << event.key << ", text: " << event.text
                  << ", action: " << event.data.at("action") << std::endl;
    }

    return 0;
}
*/