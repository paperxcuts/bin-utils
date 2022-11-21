#pragma once
#include <array>
#include <iostream>
#include <optional>
#include <cassert>

using rgb_array_t = std::array<uint8_t, 3>;
using hsl_array_t = std::array<float, 3>;



// takes a single hexadecimal character and returns its numerical equivalent
// - returns -1 if input is invalid
char hex_chr2dec(char hex)
{
	if (hex >= '0' && hex <= '9')
		return hex - '0';
	if (hex >= 'a' && hex <= 'f')
		return (hex - 'a') + 10;
	if (hex >= 'A' && hex <= 'F')
		return (hex - 'A') + 10;
	return -1;
}

// takes a single value from 0 to 15 and returns its hexadecimal character equivalent 
// - returns -1 if input is invalid
char hex_dec2chr(char digit, bool upcase = true)
{
	assert(digit <= 15);
	if (digit >= 0 && digit <= 9)
		return '0' + digit;
	if (digit >= 10 && digit <= 15)
		return upcase ? (digit - 10) + 'A' : (digit - 10) + 'a';
	return -1;
}

// gets value from the 4 bits on the left and the 4 bits on the right of a byte
std::pair<uint8_t, uint8_t>split_byte(uint8_t byte) {
	return { byte >> 4, byte & 0x0F };
}

namespace rgb {
	
	// stores rgb values into a uint32_t
	uint32_t pack(uint8_t r, uint8_t g, uint8_t b) {
		return static_cast<uint32_t>((r << 16) | (g << 8) | b);
	}
	
	uint32_t pack(rgb_array_t rgb) {
		return rgb::pack(rgb[0], rgb[1], rgb[2]);
	}
	
	// gets rgb values stored in a uint32_t and returns in a color_array_t
	rgb_array_t unpack(uint32_t rgb_pack) {
		return {
			static_cast<uint8_t>(rgb_pack >> 16),
			static_cast<uint8_t>((rgb_pack >> 8) & 0xFF0000),
			static_cast<uint8_t>(rgb_pack & 0xFFFF00)
		};
	}

	// n rounded to the nearest number divisable by 17 (255 / 15)
	uint8_t round_digit_17(int n)
	{
		int d = 17;
		int q = n / d;
		int p1 = d * q;
		int p2 = d * (q + 1);
		return static_cast<uint8_t>(abs(n - p1) > (n - p2) ? p2 : p1);
	}

	// turns rgb values to an rgb hexadecimal color code
	// - option to round the values to fit/round and return a 3 digit hex code instead of 6
	// - option of the hexadecimal letters being uppercase or lowercase 
	std::string rgb_to_hex(uint8_t r, uint8_t g, uint8_t b, bool three_digit = false, bool uppercase = true) {
		std::string ret;

		if (three_digit)
		{
			ret.push_back(hex_dec2chr(round_digit_17(r) / 17, uppercase));
			ret.push_back(hex_dec2chr(round_digit_17(g) / 17, uppercase));
			ret.push_back(hex_dec2chr(round_digit_17(b) / 17, uppercase));
		}
		else {
			auto [r1, r2] = split_byte(r);
			ret.push_back(hex_dec2chr(r1, uppercase));
			ret.push_back(hex_dec2chr(r2, uppercase));

			auto [g1, g2] = split_byte(g);
			ret.push_back(hex_dec2chr(g1, uppercase));
			ret.push_back(hex_dec2chr(g2, uppercase));

			auto [b1, b2] = split_byte(b);
			ret.push_back(hex_dec2chr(b1, uppercase));
			ret.push_back(hex_dec2chr(b2, uppercase));
		}

		return ret;
	}


	// turns a rgb hexadecimal color code string to numerical form in a 3 component array
	// - returns an std::optional that will contain a value only if the input string is valid
	// - '#' hash is optional
	// - supports 3 or 6 hex values (RGB | RRGGBB)
	std::optional<rgb_array_t> hex_to_rgb(std::string_view hexcode)
	{
		if (hexcode.empty()) return std::nullopt;

		rgb_array_t color;
		int len = hexcode.length();
		int len3 = 3;
		int len6 = 6;

		char* str = const_cast<char*>(hexcode.data());

		if (hexcode[0] == '#') str++, len3++, len6++;

		if (len == len3)
		{
			for (int i = 0; i < 3; i++)
			{
				char digit = hex_chr2dec(str[i]);
				if (digit < 0)
					return std::nullopt;
				color[i] = static_cast<uint8_t>(digit * 17);
			}
			return color;
		}
		if (len == len6)
		{
			for (int i = 0, j = 0; i < 6; i += 2, j++)
			{
				char left = hex_chr2dec(str[i]);
				char right = hex_chr2dec(str[i + 1]);
				if (left < 0 || right < 0)
					return std::nullopt;
				color[j] = (static_cast<uint8_t>(left) << 4 | static_cast<uint8_t>(right));
			}
			return color;
		}

		return std::nullopt;
	}
}
class color_rgb {
	uint8_t r;
	uint8_t g;
	uint8_t b;
public:
	color_rgb() : r(0), g(0), b(0) {}
	color_rgb(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
	color_rgb(rgb_array_t cols) : r(cols[0]), g(cols[1]), b(cols[2]) {}

	color_rgb(std::string_view hexcode) {
		rgb_array_t colors = rgb::hex_to_rgb(hexcode).value_or(rgb_array_t{0,0,0});
		*this = colors;
	}

	color_rgb& operator=(rgb_array_t cols) {
		*this = color_rgb(cols);
		return *this;
	}
	color_rgb& operator=(std::string_view hexcode) {
		*this = color_rgb(hexcode);
		return *this;
	}

public:
	rgb_array_t array() {
		return {r, g, b};
	}
	uint32_t packed() {
		return rgb::pack(r, g, b);
	}
	std::string hex(bool three_digit = false, bool uppercase = true) {
		return rgb::rgb_to_hex(r, g, b, three_digit, uppercase);
	}
};