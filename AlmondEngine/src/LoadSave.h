#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <zlib.h> // For compression

namespace almond {
    struct Event {
        std::string type;
        std::map<std::string, std::string> data;
    };

    class SaveSystem {
    public:
        void SaveGame(const std::string& filename, const std::vector<Event>& events);
        void LoadGame(const std::string& filename, std::vector<Event>& events);

    private:
        std::string CompressData(const std::string& data);
        std::string DecompressData(const std::string& data);
    };
}
