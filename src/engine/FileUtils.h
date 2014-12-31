
#pragma once

#include <stdio.h>
#include <sys/stat.h>
#include <string>

namespace
{
    off_t GetFileSize(int fd)
    {
        struct stat buf;
        auto ret = fstat(fd, &buf);
        return ret == 0 ? buf.st_size : -1;
    }

    off_t GetFileSize(FILE *f)
    {
        return GetFileSize(fileno(f));
    }

    bool ReadBinaryFileToString(const char *filename, std::string &out)
    {
        // try to open the file
        auto ShaderFile = fopen(filename, "rb");
        if (ShaderFile)
        {
            // get the size to read
            auto size = GetFileSize(ShaderFile);

            // allocate enough space for size+null
            out.resize(size+1);

            // set the null char
            out[size] = '\0';

            // read the file to memory
            fread(&out[0], 1, size, ShaderFile);

            // close the file
            fclose(ShaderFile);
            return true;
        }
        return false;
    }

    bool ReadBinaryFileToString(const std::string &filename, std::string &out)
    {
        return ReadBinaryFileToString(filename.c_str(), out);
    }
}
