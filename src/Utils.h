#ifndef UTILS_H_
#define UTILS_H_

#include <vector>
#include <array>
#include <string_view>

static constexpr std::string_view file_start = "#ifndef RSCLINK_LINK_H_\n"
											   "#define RSCLINK_LINK_H_\n\n"
   											   "#include <cstdint>\n\n";

inline std::string heading_var = "constexpr std::uint8_t link_heading[]{";

inline std::string data_var = "constexpr std::uint8_t link_data[]{";

namespace Utils
{
	using HexBytes = std::vector<std::array<char, 6>>;

	HexBytes ToHexList(const char *input, uintmax_t size)
	{
		static constexpr char hex_map[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

		HexBytes result(size);

		for (uintmax_t i{}; i < size; ++i)
		{
			// Init with '\x'
			memcpy(result[i].data(), "'\\x", 3);
			// Set the two hex digits
			memset(reinterpret_cast<void *>(result[i].data() + 3), hex_map[(input[i] & 0xF0) >> 4], 1);
			memset(reinterpret_cast<void *>(result[i].data() + 4), hex_map[input[i] & 0x0F], 1);
			memset(reinterpret_cast<void *>(result[i].data() + 5), '\'', 1);
		}

		return result;
	}
}

#endif