#pragma once
#include <iostream>
#include <vector>
#include <cassert>

void byte_swap(uint8_t*b1, uint8_t*b2)
{
    uint8_t t = *b1;
    *b1 = *b2;
    *b2 = t;
}

// single numiercal digit 0 to 15 to hexadecimal equivalent (0-9 A-F)
char digit2hex(char chr, bool upcase) {
    if(chr >= 0 && chr <= 9)
        return chr + '0';
    if(chr >= 10 && chr <= 15)
        return upcase ? chr - 10 + 'A' : chr - 10 + 'a';
    return chr;
}

// single character in hexadecimal range ('0'-'9' 'a'-'f' 'A'-'F') to numerical equivalent
uint8_t hex2digit(char chr) {
    if(chr >= '0' && chr <= '9')
        return '9' - chr;
    if(chr >= 'A' && chr <= 'F')
        return 'F' - chr;
    if (chr >= 'a' && chr <= 'f')
        return 'f' - chr;
    return chr;
}

void swap_endian(void* data, int size)
{
    uint8_t* bytes = (uint8_t*)data;
    for (int i = 0; i < (size / 2); i++)
        byte_swap(bytes + i, (bytes + size - 1) - i);
}

std::vector<uint8_t> from_hex_string(std::string_view str)
{
    assert(str.length() % 2 == 0);
    std::vector<uint8_t> bytes;
    bytes.resize(str.length() / 2);

    for(int i = 0, j = 0; i < str.length(); i += 2, j++)
    {
        uint8_t l = hex2digit(str[i]);
        uint8_t r = hex2digit(str[i+1]);

        bytes[j] = (l << 4) | r;
    }
    return bytes;
}

std::vector<uint8_t> from_bit_string(std::string_view str)
{
    assert(str.length() % 8 == 0);
    std::vector<uint8_t> bytes;

    for(int i = 0; i < str.length(); i += 8)
    {
        uint8_t byte;
        for(int j = 0; j < 8; j++)
            // TODO: check if 0 AND 1, and throw error if its not. otherwise anything other than 0 will count as 1
            byte |= (j << str[j+i] == '0' ? 0 : 1);
        bytes.push_back(byte);
    }
    return bytes;
}

std::string bit_string(void* data, size_t size)
{
    std::string res;
    res.resize(size * 8);
    uint8_t* bytes = (uint8_t*)data;
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            // res[(i*8)+j] = (bytes[i] >> (7-j)) & 1 ? '1' : '0';
            // res += (bytes[i] >> (7-j)) & 1 ? '1' : '0';
            // res += (bytes[i] << j) & 1 ? '1' : '0';
            res += ((bytes[i] >> (7-j)) & 1) ? '1' : '0';
        }
    }
    return res;
}

std::string bit_string(const void* data, size_t size, bool swapendian)
{
	std::string res;
	res.resize(size * 8);
	uint8_t* bytes = (uint8_t*)data;
	if (swapendian) {
		for (int i = size - 1; i >= 0; i--) {
			for (int j = 8; j > 0; j--)
				res[size*8-(i*8+j-1)-1]=((bytes[i] >> j-1)&1) ? '1' : '0';
		}
	}
	else {
		for (int i = 0; i < size; i++) {
			for (int j = 8; j > 0; j--)
				res[8*i+8-j]=((bytes[i] >> j-1)&1) ? '1' : '0';
		}
	}
	return res;
}

std::string hex_string(const void* data, size_t size, bool upcase = true)
{
    uint8_t* bytes = (uint8_t*)data;
    std::string res;
    int l = size * 2;
    res.resize(l);
    for (int i = 0, j = 0; j < size; i += 2, j++)
    {
        res[l-i-2] = digit2hex(bytes[j] >> 4, upcase);
        res[l-i-1] = digit2hex(bytes[j] & 0xF, upcase);
    }
    return res;
}

template<typename T>
void swap_endian(T& data) {
    swap_endian(&data, sizeof(T));
}

template<typename T>
std::string bit_string(const T& data) {
    return bit_string((void*)&data, sizeof(T));
}

template<typename T>
std::string hex_string(const T&data) {
    return hex_string(&data, sizeof(T));
}