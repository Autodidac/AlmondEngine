/**************************************************************
 *   █████╗ ██╗     ███╗   ███╗   ███╗   ██╗    ██╗██████╗    *
 *  ██╔══██╗██║     ████╗ ████║ ██╔═══██╗████╗  ██║██╔══██╗   *
 *  ███████║██║     ██╔████╔██║ ██║   ██║██╔██╗ ██║██║  ██║   *
 *  ██╔══██║██║     ██║╚██╔╝██║ ██║   ██║██║╚██╗██║██║  ██║   *
 *  ██║  ██║███████╗██║ ╚═╝ ██║ ╚██████╔╝██║ ╚████║██████╔╝   *
 *  ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝    *
 *                                                            *
 *   This file is part of the Almond Project.                 *
 *   AlmondEngine - Modular C++ Game Engine                   *
 *                                                            *
 *   SPDX-License-Identifier: LicenseRef-MIT-NoSell           *
 *                                                            *
 *   Provided "AS IS", without warranty of any kind.          *
 *   Use permitted for non-commercial purposes only           *
 *   without prior commercial licensing agreement.            *
 *                                                            *
 *   Redistribution allowed with this notice.                 *
 *   No obligation to disclose modifications.                 *
 *   See LICENSE file for full terms.                         *
 **************************************************************/
#include "aeventsystem.hpp"

#include <zlib.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

namespace almondnamespace {

    class SaveSystem {
    public:
        static void SaveGame(const std::string& filename, const std::vector<almondnamespace::events::Event>& events) {  
           std::ofstream ofs(filename, std::ios::binary);  
           if (!ofs) {  
               std::cerr << "Error opening file for saving!" << std::endl;  
               return;  
           }  

           std::string data;  
           for (const auto& event : events) {  
               data += event_type_to_string(event.type) + ":";  
               for (const auto& pair : event.data) {  
                   data += pair.first + "=" + pair.second + ";";  
               }  
               data += "x=" + std::to_string(event.x) + ";";  
               data += "y=" + std::to_string(event.y) + ";";  
               data += "key=" + std::to_string(event.key) + ";";  

               // Fix for the error: Convert char32_t to UTF-8 string before appending  
               std::string utf8Text;  
               if (event.text != 0) {  
                   char buffer[5] = { 0 };  
                   if (event.text <= 0x7F) {  
                       buffer[0] = static_cast<char>(event.text);  
                   } else if (event.text <= 0x7FF) {  
                       buffer[0] = static_cast<char>(0xC0 | ((event.text >> 6) & 0x1F));  
                       buffer[1] = static_cast<char>(0x80 | (event.text & 0x3F));  
                   } else if (event.text <= 0xFFFF) {  
                       buffer[0] = static_cast<char>(0xE0 | ((event.text >> 12) & 0x0F));  
                       buffer[1] = static_cast<char>(0x80 | ((event.text >> 6) & 0x3F));  
                       buffer[2] = static_cast<char>(0x80 | (event.text & 0x3F));  
                   } else {  
                       buffer[0] = static_cast<char>(0xF0 | ((event.text >> 18) & 0x07));  
                       buffer[1] = static_cast<char>(0x80 | ((event.text >> 12) & 0x3F));  
                       buffer[2] = static_cast<char>(0x80 | ((event.text >> 6) & 0x3F));  
                       buffer[3] = static_cast<char>(0x80 | (event.text & 0x3F));  
                   }  
                   utf8Text = std::string(buffer);  
               }  
               data += "text=" + utf8Text + ";";  
               data += "\n";  
           }  

           std::string compressedData = CompressData(data);  
           ofs.write(compressedData.c_str(), compressedData.size());  
           ofs.close();  
        }

        static void LoadGame(const std::string& filename, std::vector<almondnamespace::events::Event>& events) {
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
                events::Event event;

                size_t typeEnd = line.find(':');
                if (typeEnd == std::string::npos) {
                    data.erase(0, pos + 1);
                    continue;
                }

                event.type = almondnamespace::events::event_type_from(line.substr(0, typeEnd));
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
                            // Replace the problematic line with the following code to fix the error:  
                            // Fix for the error: Use a temporary buffer to handle char32_t to char conversion  
                            char buffer[5] = { 0 };  
                            // Replace the problematic line with the following code to fix the error:  
                            //std::memset(event.text, 0, sizeof(event.text));  
                            if (event.text <= 0x7F) {  
                                buffer[0] = static_cast<char>(event.text);  
                            } else if (event.text <= 0x7FF) {  
                                buffer[0] = static_cast<char>(0xC0 | ((event.text >> 6) & 0x1F));  
                                buffer[1] = static_cast<char>(0x80 | (event.text & 0x3F));  
                            } else if (event.text <= 0xFFFF) {  
                                buffer[0] = static_cast<char>(0xE0 | ((event.text >> 12) & 0x0F));  
                                buffer[1] = static_cast<char>(0x80 | ((event.text >> 6) & 0x3F));  
                                buffer[2] = static_cast<char>(0x80 | (event.text & 0x3F));  
                            } else {  
                                buffer[0] = static_cast<char>(0xF0 | ((event.text >> 18) & 0x07));  
                                buffer[1] = static_cast<char>(0x80 | ((event.text >> 12) & 0x3F));  
                                buffer[2] = static_cast<char>(0x80 | ((event.text >> 6) & 0x3F));  
                                buffer[3] = static_cast<char>(0x80 | (event.text & 0x3F));  
                            }  
                            std::memset(reinterpret_cast<void*>(&event.text), 0, sizeof(event.text));
                            std::strncpy(reinterpret_cast<char*>(event.text), buffer, sizeof(event.text) - 1);

                            //strncpy_s(event.text, value.c_str(), sizeof(event.text) - 1);
                           // event.text[sizeof(event.text) - 1] = '\0';
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

    private:
        static std::string CompressData(const std::string& data) {
            uLongf compressedSize = compressBound(static_cast<uLong>(data.size()));
            std::vector<Bytef> compressedData(compressedSize);
            if (compress(compressedData.data(), &compressedSize, reinterpret_cast<const Bytef*>(data.data()), static_cast<uLong>(data.size())) != Z_OK) {
                return data;
            }
            compressedData.resize(compressedSize);
            return std::string(reinterpret_cast<char*>(compressedData.data()), compressedSize);
        }

        static std::string DecompressData(const std::string& compressedData) {
            uLongf decompressedSize = static_cast<uLong>(compressedData.size()) * 4;
            std::vector<Bytef> decompressedData(decompressedSize);
            while (uncompress(decompressedData.data(), &decompressedSize, reinterpret_cast<const Bytef*>(compressedData.data()), static_cast<uLong>(compressedData.size())) == Z_BUF_ERROR) {
                decompressedSize *= 2;
                decompressedData.resize(decompressedSize);
            }
            return std::string(reinterpret_cast<char*>(decompressedData.data()), decompressedSize);
        }
    };

}  // namespace almond
