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


std::vector<uint8_t> from_bit_string(std::string_view str, bool little_endian = true)
{
    // TODO: check for both 1 AND 0. otherwise every character except 1 will count as 0
    assert(str.length() % 8 == 0);
    std::vector<uint8_t> bytes;
    bytes.resize(str.length() / 8);

    // TODO: reduce code duplication (little endian and big endian do the same thing but with different loop expressions) (make function?)
    if(little_endian){
        for(int i = str.length() - 8, k = 0; i >= 0; i -= 8, k++)
        {
            uint8_t byte = 0;
            for(int j = 0; j < 8; j++) {
                byte |= ((str[j+i] == '1' ? 1 : 0) << (7-j));
            }
            bytes[k] = byte;
        }
    } else {
        for(int i = 0, k = 0; i < str.length(); i += 8, k++)
        {
            uint8_t byte = 0;
            for(int j = 0; j < 8; j++) {
                byte |= ((str[j+i] == '1' ? 1 : 0) << (7-j));
            }
            bytes[k] = byte;
        }
    }
    return bytes;
}

template<typename T>
T from_bit_string(std::string_view str, bool little_endian = true)
{
    assert(str.length() == sizeof(T) * 8);
    T result;
    auto bytes = from_bit_string_2(str, little_endian);
    memcpy(&result, bytes.data(), bytes.size());
    return res;
}

// READY TO TEST
template<typename T>
void swap_endian(T& data)
{
    size_t size = sizeof(T);
    uint8_t* bytes = (uint8_t*)&data;
    for (int i = 0; i < (size / 2); i++)
        byte_swap(bytes + i, (bytes + size - 1) - i);
}

// READY TO TEST
template<typename T>
std::string bit_string(const T& data,  bool little_endian = true)
{
    constexpr int size = sizeof(T);

    std::string res;
    res.resize(size * 8);
    uint8_t* bytes = (uint8_t*)&data;
    
    if(little_endian){
        // little endian
        for(int i = size-1; i >= 0; i--) {
            for(int j = 0; j < 8; j++) {
                res += ((bytes[i] >> (7-j)) & 1) ? '1' : '0';
            }
        }
    } else {
        // big endian
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < 8; j++) {
                res += ((bytes[i] >> (7-j)) & 1) ? '1' : '0';
            }
        }
    }

    return res;
}

// READY TO TEST
template<typename T>
std::string hex_string(const T& data, bool upcase = true)
{
     size_t size = sizeof(T);
     size_t hl = size * 2;

    uint8_t* bytes = (uint8_t*)&data;
    std::string res;
    res.resize(hl);
    
    for (int i = 0, j = 0; j < size; i += 2, j++)
    {
        res[hl-i-2] = digit2hex(bytes[j] >> 4, upcase);
        res[hl-i-1] = digit2hex(bytes[j] & 0xF, upcase);
    }
    return res;
}
