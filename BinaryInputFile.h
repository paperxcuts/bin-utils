#pragma once
#include <iostream>
#include <fstream>
#include <bit>
#include <filesystem>
#include "../Binary_Functions/Binary.h"

// bin file input

template<std::endian byteorder = std::endian::native>
class BinaryInputFile {
    std::ifstream m_file;
    std::filesystem::path m_path;
    size_t m_size;
public:
    BinaryInputFile(std::string_view filepath) {
        open(filepath);
    }
    BinaryInputFile() : m_file{}, m_path{}, m_size{0} {}

public:
    // skip 'sizeof(T)' bytes from current file offset
    template<typename T>
    void dummy() {
        seek(sizeof(T), std::ios::cur);
    }

    // skip 'sizeof(T)' bytes, 'count' number of times from current file offset
    template<typename T>
    void dummy(size_t count) {
        seek(sizeof(T) * count, std::ios::cur);
    }

    // skip 'count' bytes from current file offset
    void dummy(std::streamoff count) {
        seek(count, std::ios::cur);    
    }

    // read type 'T' from file, swapping the byte order if neccessary
    template<typename T>
    [[nodiscard]] T read() {
        T res;
        // TODO: c++ cast?
        m_file.read((char*)&res, sizeof(T));
        if constexpr(byteorder != std::endian::native)
            swap_endian(res);
        return res;
    }

    // read overload for seeking to streampos before reading
    template<typename T>
    [[nodiscard]] T read(std::streampos fpos) {
        seek(fpos);
        return read<T>();
    }

    // read overload for seeking with stream offset and seek direction
    template<typename T>
    [[nodiscard]] T read(std::streamoff foff, std::ios::seekdir fdir) {
        seek(foff, fdir);
        return read<T>();
    }

    // 
    template<typename T>
    [[nodiscard]] std::unique_ptr<T[]> read_dynamic(size_t count) {
        auto data = std::make_unique<T[]>(count);
        for(int i = 0; i < count; i++)
            data[i] = read<T>();
        return data;
    }

    template<typename T>
    [[nodiscard]] std::unique_ptr<T[]> read_dynamic(size_t count, std::streampos fpos) {
        seek(fpos);
        return read_dynamic<T>(count);
    }

    template<typename T>
    [[nodiscard]] std::unique_ptr<T[]> read_dynamic(size_t count, std::streamoff foff, std::ios::seekdir fdir) {
        seek(foff, fdir);
        return read_dynamic<T>(count);
    }


    template<typename T, int N>
    [[nodiscard]] std::array<T, N> read_static() {
        std::array<T, N> data;
        for(int i = 0; i < N; i++)
            data[i] = read<T>();
        return data;
    }

    template<typename T, int N>
    [[nodiscard]] std::array<T, N> read_static(std::streampos fpos) {
        seek(fpos);
        return read_static<T, N>();
    }

    template<typename T, int N>
    [[nodiscard]] std::array<T, N> read_static(std::streamoff foff, std::ios::seekdir fdir) {
        seek(foff, fdir)
        return read_static<T, N>();
    }


    [[nodiscard]] std::unique_ptr<uint8_t[]> read_bytes(size_t count) {
        auto bytes = std::make_unique<uint8_t[]>(count);
        for(int i = 0; i < count; i++)
            m_file.read((char*)bytes[i], sizeof(uint8_t));
        return bytes;
    }
    [[nodiscard]] std::unique_ptr<uint8_t[]> read_bytes(size_t count, std::streampos fpos) {
        seek(fpos);
        return read_bytes(count);
    }
    [[nodiscard]] std::unique_ptr<uint8_t[]> read_bytes(size_t count, std::streamoff foff, std::ios::seekdir fdir) {
        seek(foff, fdir);
        return read_bytes(count);
    }


    [[nodiscard]] std::string read_string(int limit_distance = -1)
    {
        if(limit_distance < 0)
            limit_distance = std::numeric_limits<int>::max();
        
        std::string res;
        char chr;
        m_file.read(&chr, 1);

        for(int i = 0; !m_file.eof() && chr != '\0' && i < limit_distance; i++) {
            res += chr;
            m_file.read(&chr, 1);
        }
        return res;
    }

public:
    std::filesystem::path path() const {
        return m_path;
    }
    size_t size() const {
        return m_size;
    }
    std::string name() const {
        return path().filename().string();
    }
    std::ifstream& handle() {
        return m_file;
    }

public:
    std::streampos tell() {
        return m_file.tellg();
    }
    void seek(std::streampos fpos) {
        m_file.seekg(fpos);
    }
    void seek(std::streamoff foff, std::ios::seekdir fdir) {
        m_file.seekg(foff, fdir);
    }

    void open(std::string_view filepath)
    {
        m_file.open(filepath, std::ios::in | std::ios::ate | std::ios::binary);
        m_size = tell();
        m_path = std::filesystem::path(filepath);
        seek(std::ios::beg);
    }
    void close() {
        m_file.close();
    }
};