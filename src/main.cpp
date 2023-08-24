#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstdio>

#include "Utils.h"

namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    try
    {
        if (argc != 2)
            throw std::invalid_argument("Invalid argument, format: <RSCLINK> <FOLDER_PATH>");

        int file_count{};
        uintmax_t heading_size{};
        uintmax_t data_size{};

        for (const auto &entry : fs::directory_iterator(argv[1]))
        {
            static uintmax_t last_pos{};

            if (fs::is_directory(entry.path()))
                continue;

            std::string file_name{entry.path().filename().string()};
            fs::path file_path{(argv[1] + std::string("\\") + file_name)};

            std::ifstream file(file_path.c_str(), std::ifstream::ate | std::ios::binary);

            uintmax_t size{entry.file_size() + 1};

            if (size == 0)
                continue;

            data_size += size;

            auto buffer{std::make_unique<std::uint8_t[]>(size)};

            file.seekg(0, std::ios::beg);

            if (!file.read(reinterpret_cast<char *>(buffer.get()), size - 1))
                throw std::runtime_error("Failed to read file");

            file.close();

            printf("[+] '%s' read\n", file_name.c_str());

            buffer.get()[size - 1] = '\x00';

            Utils::HexBytes hex_list{Utils::ToHexList(reinterpret_cast<char *>(buffer.get()), size)};

            for (const std::array<char, 6> hex : hex_list)
            {
                data_var += hex.data();
                data_var += ", ";
            }

            std::string entry_str{file_name + "|" + std::to_string(size) + "|" + std::to_string(last_pos) + "|" + std::to_string(last_pos + size) + "\n"};
            last_pos += size;

            heading_size += entry_str.size();

            hex_list = Utils::ToHexList(entry_str.c_str(), entry_str.size());

            for (const std::array<char, 6> hex : hex_list)
            {
                heading_var += hex.data();
                heading_var += ", ";
            }

            file_count++;
        }

        heading_var += "'\\x00'};\n\n";
        data_var += "'\\x00'};\n\n";

        std::string output{file_start};

        output += "#define LINK_HEADING_SIZE " + std::to_string(heading_size) + "\n";
        output += "#define LINK_DATA_SIZE " + std::to_string(data_size) + "\n\n";
        output += heading_var;
        output += data_var;
        output += "#endif";

        std::ofstream output_file("Link.h", std::ios::trunc);

	if (!output_file.write(output.c_str(), output.size()))
		throw std::runtime_error("Unable to write file: 'Link.h'. Permissions?");

	output_file.close();

        printf("[+] %i read in total\n", file_count);
        printf("[+] Successfully linked resources into 'Link.h'\n");
    }
    catch (const std::exception &e)
    {
        printf("%s", e.what());
        return 1;
    }

    return 0;
}
