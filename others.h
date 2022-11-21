#pragma once
#include <array>
#include <cmath>

using hsl_t = std::array<float, 3>;
using rgb_t = std::array<uint8_t, 3>;

hsl_t rgb_to_hsl(float r, float g, float b)
{
	r /= 255.0f;
	g /= 255.0f;
	b /= 255.0f;

	float cmax = std::max({ r, g, b });
	float cmin = std::min({ r, g, b });

	float L = (cmax + cmin) / 2.0f;
	float S = cmin == cmax ? 0	: L <= 0.5f ? (cmax - cmin) / (cmax + cmin) : (cmax - cmin) / (2.0f - cmax - cmin);
	float H = S == 0 ? 0 :
		r == cmax ? 0.0f + (g - b) / (cmax - cmin) :
		g == cmax ? 2.0f + (b - r) / (cmax - cmin) :
		b == cmax ? 4.0f + (r - g) / (cmax - cmin) : 0;

	H *= 60.0f;

	S = std::round(S * 100.0f);
	L = std::round(L * 100.0f);

	H += H < 0 ? 360.0f : 0;

	return { H, S, L };
}

rgb_t hsl_to_rgb(float h, float s, float l)
{
	s *= 0.01f;
	l *= 0.01f;
	
	float tmp1 = l < 0.5f ? l * (1.0 + s) : (l+s) - (l*s);
	float tmp2 = 2.0f * l - tmp1;
	h /= 360.0f;

}