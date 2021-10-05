// Author: HenryAWE
// License: The 3-clause BSD License

#include "vfs.hpp"
#include <stdexcept>
#include <physfs.h>


namespace awe::vfs
{
    VfsFile::VfsFile() = default;
    VfsFile::VfsFile(const std::string& filename, Mode mode)
    {
        Open(filename, mode);
    }

    VfsFile::~VfsFile() noexcept
    {
        Close();
    }

    void VfsFile::Open(const std::string& filename, Mode mode)
    {
        PHYSFS_file* f = nullptr;
        switch(mode)
        {
        case READ:
            f = PHYSFS_openRead(filename.c_str());
            break;
        case WRITE:
            f = PHYSFS_openWrite(filename.c_str());
            break;
        case APPEND:
            f = PHYSFS_openAppend(filename.c_str());
            break;
        }

        if(f)
        {
            Close();
            m_filename = filename;
            m_handle = f;
            m_good = true;
        }
    }
    void VfsFile::Close() noexcept
    {
        if(!m_handle)
            return;
        PHYSFS_close(m_handle);
        m_filename.clear();
        m_handle = nullptr;
        m_good = false;
    }

    std::size_t VfsFile::Read(void* buf, std::size_t count)
    {
        return PHYSFS_readBytes(m_handle, buf, (PHYSFS_uint32)count);
    }

    PHYSFS_Stat VfsFile::Stat()
    {
        PHYSFS_Stat stat{};
        PHYSFS_stat(m_filename.c_str(), &stat);
        return stat;
    }

    std::vector<std::byte> GetData(const std::string& filename)
    {
        VfsFile file(filename, VfsFile::READ);
        if(!file)
            throw std::runtime_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        auto size = file.Stat().filesize;
        std::vector<std::byte> buf;
        buf.resize(size, std::byte(0));
        file.Read(buf.data(), buf.size());
        return std::move(buf);
    }
    std::string GetString(const std::string& filename)
    {
        auto data = GetData(filename);
        return std::string(
            (const char*)data.data(),
            data.size()
        );
    }

    bool Exists(const std::string& path)
    {
        return PHYSFS_exists(path.c_str());
    }
}
