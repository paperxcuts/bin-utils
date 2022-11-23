#pragma once
#include <filesystem>
#include <iostream>
#include <fstream>
#include <memory>
#include <bit>

#include "../bin_utils/bin_utils.h" // to swap byte order


template<std::endian byteorder = std::endian::native>
class BinaryInputFile 
{
    std::ifstream m_file;
    std::filesystem::path m_path;
    size_t m_size;
public:
    BinaryInputFile(std::string_view filepath) {
        open(filepath);
    }
    BinaryInputFile() : m_file{}, m_path{}, m_size{0} {}

    BinaryInputFile(const BinaryInputFile&) = delete;
    BinaryInputFile& operator=(const BinaryInputFile&) = delete;
public:
    // skip 'sizeof(T)' bytes from current file position
    template<typename T>
    void dummy() {
        seek(sizeof(T), std::ios::cur);
    }

    // skip 'sizeof(T)' bytes, 'count' number of times from current file position
    template<typename T>
    void dummy(size_t count) {
        seek(sizeof(T) * count, std::ios::cur);
    }

    // skip 'count' bytes from current file position
    void dummy(size_t count) {
        seek(count, std::ios::cur);    
    }

    // each read function, by default, will read data from the current offset positioned by the get pointer
    // each read function also has two overloads for seeking. one with streampos and one with streamoff and streamdir

    // read type 'T' from file
    template<typename T>
    [[nodiscard]] T read() {
        T res;
        m_file.read(reinterpret_cast<char*>(&res), sizeof(T));
        if constexpr(byteorder != std::endian::native)
            swap_endian(res);
        return res;
    }

    template<typename T>
    [[nodiscard]] T read(std::streampos fpos) {
        seek(fpos);
        return read<T>();
    }

    template<typename T>
    [[nodiscard]] T read(std::streamoff foff, std::ios::seekdir fdir) {
        seek(foff, fdir);
        return read<T>();
    }

    
    // reads array of 'T', with size 'count', from the file before returning it in a unique_ptr (heap allocated)
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


    // reads array of 'T', with size 'N', and puts it in an std::array (stack allocated)
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


    // dynamically read 'count' number of bytes from the file
    [[nodiscard]] std::unique_ptr<uint8_t[]> read_bytes(size_t count) {
        auto bytes = std::make_unique<uint8_t[]>(count);
        for(int i = 0; i < count; i++)
            m_file.read(reinterpret_cast<char*>(bytes[i]), sizeof(uint8_t));
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

    // reads null terminated string from the file
    // where limit distance is the number of characters to stop at after not finding a null terminator or reaching the end of the file
    // set limit_distance to -1 for no limit (INT_MAX)
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