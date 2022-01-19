// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_RES_VFS_HPP
#define TESTWORLD_RES_VFS_HPP

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <physfs.h>


namespace awe::vfs
{
    class VfsError : public std::runtime_error
    {
    public:
        VfsError(PHYSFS_ErrorCode code);
    };

    enum class FileMode
    {
        READ = 1,
        WRITE,
        APPEND
    };

    std::vector<std::byte> GetData(const std::string& filename);
    std::string GetString(const std::string& filename);

    template <typename OutputIt>
    void EnumFiles(const std::string& path, OutputIt iter)
    {
        PHYSFS_EnumFilesCallback cb = [](void* data, const char* dir, const char* file)
        {
            auto& iter = *(OutputIt*)data;
            *iter = std::string(file);
        };
        PHYSFS_enumerateFilesCallback(path.c_str(), cb, &iter);
    }

    bool Exists(const std::string& path);

    class FileBuf : public std::streambuf
    {
    public:
        FileBuf* Open(const std::string& filename, FileMode mode = FileMode::READ);
        FileBuf* Close();

        [[nodiscard]]
        std::size_t FileSize() const noexcept;
        [[nodiscard]]
        const PHYSFS_Stat& Stat() const noexcept;

    protected:
        pos_type seekoff(
            off_type off,
            std::ios_base::seekdir dir,
            std::ios_base::openmode mode
        ) override;
        pos_type seekpos(
            pos_type pos,
            std::ios_base::openmode mode
        ) override;
        int_type underflow() override;

    private:
        PHYSFS_File* m_file = nullptr;
        PHYSFS_Stat m_stat{};
        FileMode m_mode = static_cast<FileMode>(0);
        char_type m_buffer[BUFSIZ];
    };

    class InputStream : public std::istream
    {
    public:
        InputStream();
        InputStream(const std::string& filename, FileMode mode = FileMode::READ);

        bool Open(const std::string& filename, FileMode mode = FileMode::READ);
        void Close() noexcept;

        [[nodiscard]]
        std::size_t FileSize() noexcept;

    private:
        FileBuf m_buf;
    };
}

#endif
