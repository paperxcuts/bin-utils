// binary utility functions. mostly for working with data in and as strings

#pragma once
#include <concepts>
#include <iostream>
#include <vector>
#include <cassert>

void byte_swap(uint8_t *b1, uint8_t *b2)
{
    uint8_t t = *b1;
    *b1 = *b2;
    *b2 = t;
}

// single numiercal digit (0 - 15) to hexadecimal equivalent (0-9 A-F)
char digit2hex(char chr, bool upcase = true) {
    if(chr >= 0 && chr <= 9)
        return chr + '0';
    if(chr >= 10 && chr <= 15)
        return upcase ? chr - 10 + 'A' : chr - 10 + 'a';
    return chr;
}

// single hexadecimal character ('0'-'9' 'a'-'f' 'A'-'F') to numerical equivalent (0 - 15)
uint8_t hex2digit(char chr) {
    if(chr >= '0' && chr <= '9')
        return chr - '0';
    if(chr >= 'A' && chr <= 'F')
        return (chr - 'A') + 10;
    if (chr >= 'a' && chr <= 'f')
        return (chr - 'a') + 10;
    return chr;
}

// 4 bits on right and 4 bits on left
std::pair<uint8_t, uint8_t>split_byte(uint8_t byte) {
	return { byte >> 4, byte & 0xF };
}

// takes a hexadecimal string with the following restrictions,
// - no spaces seperating bytes
// - no prefixes or suffixes (0x, h)
// - each byte must have exactly 2 hexadecimal characters (divisable by 2)
// interprets the hex as bytes and returns them in a vector
std::vector<uint8_t> from_hex_string(std::string_view str, bool swap_endian = false)
{
    assert(str.length() % 2 == 0);
    std::vector<uint8_t> bytes;
    bytes.resize(str.length() / 2);

    auto fhs_proc = [&](int i, int j) {
        uint8_t l = hex2digit(str[i]);
        uint8_t r = hex2digit(str[i+1]);
        bytes[j] = (l << 4) | r;
    };

    if(swap_endian)
    {
        for(int i = str.length()-2, j = 0; i >= 0; i -= 2, j++)
            fhs_proc(i, j);
    } else {
        for(int i = 0, j = 0; i < str.length(); i += 2, j++)
            fhs_proc(i, j);
    }
    return bytes;
}

// template overload so hex in hexstring will be interpreted as the type 'T'
template<typename T>
T from_hex_string(std::string_view str, bool swap_endian = false)
{
    assert(str.length() == sizeof(T) * 2);
    T result;
    auto bytes = from_hex_string(str, swap_endian);
    memcpy(&result, bytes.data(), bytes.size());
    return result;
}

// takes binary string with the following restrictions,
// - no spaces seperating bytes 
// - each byte must have exactly 8 bits (divisable by 8)
// interprets the bits in the string as bytes and returns them in a vector of bytes
std::vector<uint8_t> from_bit_string(std::string_view str, bool swap_endian = false)
{
    assert(str.length() % 8 == 0);

    // TODO: check for both 1 AND 0. otherwise every character except 1 will count as 0
    std::vector<uint8_t> bytes;
    bytes.resize(str.length() / 8);

    auto fbs_proc = [&](int i, int k){
        uint8_t byte = 0;
        for(int j = 0; j < 8; j++)
            byte |= ((str[j+i] == '1' ? 1 : 0) << (7-j));
        bytes[k] = byte;
    };


    if(swap_endian){
        for(int i = str.length() - 8, k = 0; i >= 0; i -= 8, k++)
            fbs_proc(i, k);
    } else {
        for(int i = 0, k = 0; i < str.length(); i += 8, k++)
            fbs_proc(i, k);
    }
    return bytes;
}

// template overload so bits in bitstring will be interpreted as the type 'T'
template<typename T>
T from_bit_string(std::string_view str, bool swap_endian = false)
{
    assert(str.length() == sizeof(T) * 8);
    T result;
    auto bytes = from_bit_string(str, little_endian);
    memcpy(&result, bytes.data(), bytes.size());
    return result;
}

// swaps the byte order of any data
void swap_endian(void* data, size_t size) {
    uint8_t* bytes = reinterpret_cast<uint8_t*>(data);
    for (int i = 0; i < (size / 2); i++)
        byte_swap(bytes + i, (bytes + size - 1) - i);
}

template<typename T>
void swap_endian(T& data) {
    swap_endian(&data, sizeof(T));
}

// turns any data into a string of bits
std::string bit_string_data(const void* data, size_t size, bool swap_endian = false)
{
    std::string res;
    res.resize(size * 8);

    const uint8_t *bytes = reinterpret_cast<const uint8_t*>(data);
    
    auto tbs_proc = [&](int i) {
        for(int j = 0; j < 8; j++)
            res += ((bytes[i] >> (7-j)) & 1) ? '1' : '0';
    };

    if(swap_endian){
        for(int i = size-1; i >= 0; i--)
            tbs_proc(i);
    } else {
        for(int i = 0; i < size; i++)
            tbs_proc(i);
    }
    return res;
}

// turns any data into a string of hexadecimal characters
std::string hex_string_data(const void* data, size_t size, bool swap_endian = false, bool upcase = true)
{
    size_t hl = size * 2;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);

    std::string res;
    res.resize(hl);

    auto ths_proc = [&](int i, int j){
        res[hl-i-1] = digit2hex(bytes[j] & 0xF, upcase);
        res[hl-i-2] = digit2hex(bytes[j] >> 4, upcase);
    };

    if(swap_endian){
        for (int i = 0, j = size-1; j < size; i += 2, j--)
            ths_proc(i, j);
     } else{
        for (int i = 0, j = 0; j < size; i += 2, j++)
            ths_proc(i, j);
     }

    return res;
}


template<typename T>
std::string bit_string(const T& value, bool swap_endian = false)
{
    return bit_string_data(&value, sizeof(T), swap_endian);
}

template<typename T>
std::string hex_string(const T& value, bool swap_endian = false, bool upcase = true)
{
    return hex_string_data(&value, sizeof(T), swap_endian, upcase);
}