// Author: HenryAWE
// License: The 3-clause BSD License

#include "vfs.hpp"
#include <stdexcept>
#include <physfs.h>
#include <fmt/format.h>


namespace awe::vfs
{
    namespace detailed
    {
        static const char* GetError(PHYSFS_ErrorCode code) noexcept
        {
            const char* str = PHYSFS_getErrorByCode(code);
            return str ? str : "";
        }
    }

    VfsError::VfsError(PHYSFS_ErrorCode code)
        : runtime_error(detailed::GetError(code)) {}

    std::vector<std::byte> GetData(const std::string& filename)
    {
        InputStream is(filename);
        if(!is)
            throw std::runtime_error(fmt::format("Failed to get data from \"{}\"", filename));
        auto size = is.FileSize();
        std::vector<std::byte> buf;
        buf.resize(size);
        is.read((char*)buf.data(), size);
        return std::move(buf);
    }
    std::string GetString(const std::string& filename)
    {
        InputStream is(filename);
        if(!is)
            throw std::runtime_error(fmt::format("Failed to get data from \"{}\"", filename));
        auto size = is.FileSize();
        std::string buf;
        buf.resize(size);
        is.read((char*)buf.data(), size);
        return std::move(buf);
    }

    bool Exists(const std::string& path)
    {
        return PHYSFS_exists(path.c_str());
    }

    FileBuf* FileBuf::Open(const std::string& filename, FileMode mode)
    {
        PHYSFS_File* f = nullptr;
        switch(mode)
        {
        case FileMode::READ:
            f = PHYSFS_openRead(filename.c_str());
            break;
        case FileMode::WRITE:
            f = PHYSFS_openWrite(filename.c_str());
            break;
        case FileMode::APPEND:
            f = PHYSFS_openAppend(filename.c_str());
            break;
        }

        if(f == nullptr)
        {
            return nullptr;
        }
        
        PHYSFS_Stat stat{};
        int stat_result = PHYSFS_stat(filename.c_str(), &stat);
        

        Close();
        m_file = f;
        m_mode = mode;
        if(stat_result)
            std::memcpy(&m_stat, &stat, sizeof(PHYSFS_Stat));
        else
            std::memset(&m_stat, 0, sizeof(m_stat));

        return this;
    }
    FileBuf* FileBuf::Close()
    {
        if(!m_file)
            return nullptr;
        int err = PHYSFS_close(m_file);
        if(err != 0)
        {
            return nullptr;
        }
        m_file = nullptr;
        m_mode = static_cast<FileMode>(0);
        return this;
    }

    std::size_t FileBuf::FileSize() const noexcept
    {
        if(!m_file)
            return 0;
        return PHYSFS_fileLength(m_file);
    }
    const PHYSFS_Stat& FileBuf::Stat() const noexcept
    {
        return m_stat;
    }

    FileBuf::pos_type FileBuf::seekoff(
            off_type off,
            std::ios_base::seekdir dir,
            std::ios_base::openmode mode
    ) {
        if(!m_file)
            return pos_type(off_type(-1)); // Default error behavior

        switch (dir)
        {
        case std::ios_base::beg:
            PHYSFS_seek(m_file, off);
            break;
        case std::ios_base::cur:
            // Subtract characters currently in buffer from seek position
            PHYSFS_seek(m_file, (PHYSFS_tell(m_file) + off) - (egptr() - gptr()));
            break;
        case std::ios_base::end:
            PHYSFS_seek(m_file, PHYSFS_fileLength(m_file) + off);
            break;
        default: // Error
            return pos_type(off_type(-1));
        }
        if (mode & std::ios_base::in)
        {
            setg(egptr(), egptr(), egptr());
        }
        if (mode & std::ios_base::out)
        {
            setp(m_buffer, m_buffer);
        }

        return PHYSFS_tell(m_file);
    }
    FileBuf::pos_type FileBuf::seekpos(
            pos_type pos,
            std::ios_base::openmode mode
    ) {
        if(!m_file)
            return pos_type(off_type(-1)); // Default error behavior

        PHYSFS_seek(m_file, pos);
        if (mode & std::ios_base::in)
        {
            setg(egptr(), egptr(), egptr());
        }
        if (mode & std::ios_base::out)
        {
            setp(m_buffer, m_buffer);
        }

        return PHYSFS_tell(m_file);
    }
    FileBuf::int_type FileBuf::underflow()
    {
        if(!m_file)
            return traits_type::eof();
        PHYSFS_sint64 read = PHYSFS_readBytes(m_file, m_buffer, BUFSIZ);
        if(read < BUFSIZ)
        {
            if(read == 0)
                return traits_type::eof();
            else if(read == -1)
                throw std::runtime_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        }
        setg(m_buffer, m_buffer, m_buffer + read);
        return traits_type::to_int_type(*gptr());
    }

    InputStream::InputStream()
        : std::istream(&m_buf) {}
    InputStream::InputStream(const std::string& filename, FileMode mode)
        : InputStream()
    {
        Open(filename, mode);
    }

    bool InputStream::Open(const std::string& filename, FileMode mode)
    {
        if(mode != FileMode::READ)
        {
            throw std::invalid_argument("invalid argument");
        }
        bool result = m_buf.Open(filename, mode) != nullptr;
        if(!result)
        {
            setstate(failbit);
        }
        return result;
    }
    void InputStream::Close() noexcept
    {
        m_buf.Close();
    }

    std::size_t InputStream::FileSize() noexcept
    {
        return m_buf.FileSize();
    }
}
