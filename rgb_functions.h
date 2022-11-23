#pragma once
#include <iostream>
#include <optional>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <array>

#include "bin_utils.h"

using rgb_array_t = std::array<uint8_t, 3>;
using hsl_array_t = std::array<float, 3>;

hsl_array_t rgb_to_hsl(float r, float g, float b)
{
	r /= 255.0f;
	g /= 255.0f;
	b /= 255.0f;

	float cmax = std::max({ r, g, b });
	float cmin = std::min({ r, g, b });

	float L = (cmax + cmin) / 2.0f;
	float S = cmin == cmax ? 0 : L <= 0.5f ? (cmax - cmin) / (cmax + cmin) : (cmax - cmin) / (2.0f - cmax - cmin);
	float H = S == 0 ? 0 :
		r == cmax ? 0.0f + (g - b) / (cmax - cmin) : 
		g == cmax ? 2.0f + (b - r) / (cmax - cmin) :
		b == cmax ? 4.0f + (r - g) / (cmax - cmin) : 0;

	H = std::round(H * 60.0f);
	S = std::round(S * 100.0f);
	L = std::round(L * 100.0f);

	H += H < 0 ? 360.0f : 0;

	return { H, S, L };
}

namespace rgb {
	
	// stores rgb values into a uint32_t
	uint32_t pack(uint8_t r, uint8_t g, uint8_t b) {
		return static_cast<uint32_t>((r << 16) | (g << 8) | b);
	}
	
	uint32_t pack(rgb_array_t rgb) {
		return rgb::pack(rgb[0], rgb[1], rgb[2]);
	}
	
	// gets rgb values stored in a uint32_t and returns in a rgb_array_t
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
	// - three_digit: should we round the values to fit/round and return a 3 digit hex code instead of 6
	// - uppercase: should the hexadecimal letters being uppercase or lowercase
	std::string rgb_to_hex(uint8_t r, uint8_t g, uint8_t b, bool three_digit = false, bool uppercase = true) {
		std::string ret;

		if (three_digit)
		{
			ret.push_back(digit2hex(round_digit_17(r) / 17, uppercase));
			ret.push_back(digit2hex(round_digit_17(g) / 17, uppercase));
			ret.push_back(digit2hex(round_digit_17(b) / 17, uppercase));
		}
		else {
			auto [r1, r2] = split_byte(r);
			ret.push_back(digit2hex(r1, uppercase));
			ret.push_back(digit2hex(r2, uppercase));

			auto [g1, g2] = split_byte(g);
			ret.push_back(digit2hex(g1, uppercase));
			ret.push_back(digit2hex(g2, uppercase));

			auto [b1, b2] = split_byte(b);
			ret.push_back(digit2hex(b1, uppercase));
			ret.push_back(digit2hex(b2, uppercase));
		}

		return ret;
	}

	// turns a rgb hexadecimal color code string to numerical form in a 3 component array.
	// functions returns an std::optional that will contain a value only if the input string is valid.
	// - supports 3 or 6 hex values (RGB | RRGGBB)
	// - '#' hash is optional
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
				uint8_t digit = hex2digit(str[i]);
				if (digit == str[i])
					return std::nullopt;
				color[i] = digit * 17;
			}
			return color;
		}
		if (len == len6)
		{
			for (int i = 0, j = 0; i < 6; i += 2, j++)
			{
				uint8_t left = hex2digit(str[i]);
				uint8_t right = hex2digit(str[i+1]);
				if (left == str[i] || right == str[i+1])
					return std::nullopt;
				color[j] = (left << 4 | right);
			}
			return color;
		}

		return std::nullopt;
	}
}

class color_rgb {
public:
	uint8_t r;
	uint8_t g;
	uint8_t b;
	
	color_rgb() : r(0), g(0), b(0) {}
	color_rgb(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
	color_rgb(rgb_array_t cols) : r(cols[0]), g(cols[1]), b(cols[2]) {}

	color_rgb(std::string_view hexcode) {
		rgb_array_t colors = rgb::hex_to_rgb(hexcode).value_or(rgb_array_t{0,0,0});
		*this = color_rgb(colors);
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