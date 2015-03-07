
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static const char* CPP_HEADER = R"-(
// AUTO GENERATED, DON'T MODIFY
#pragma once

namespace PGTASchemaHeader
{
    static const char* )-";

static const char* CPP_MID = R"-( = R"()-";

static const char* CPP_FOOTER = R"-()";
}
)-";

// turns "track.fbs" into "TRACK_FBS"
void convertFilename(const char* filenameIn, char* filenameOut)
{
    while (*filenameIn)
    {
        if (*filenameIn == '.')
        {
            *filenameOut++ = '_';
        }
        else
        {
            *filenameOut++ = static_cast<char>(toupper(*filenameIn));
        }
        filenameIn++;
    }
    *filenameOut = '\0';
}

size_t trimFbsWhitespace(const char* fbsSrc, char* trimmedSrcOut, const size_t bufLen)
{
    const char* outBegin = trimmedSrcOut;
    const char* end = fbsSrc + bufLen;
    char c;
    bool inString = false;
    while (fbsSrc < end && ((c = *fbsSrc) != 0))
    {
        if (c == '\"')
        {
            inString = !inString;
        }

        // TODO: fbs trim conditions
        if (inString || true)
        {
            *trimmedSrcOut++ = c;
        }
        ++fbsSrc;
    }
    return (trimmedSrcOut - outBegin);
}

int perform(const char* fbsPath, const char* headerOutPath)
{
    FILE* srcFp = fopen(fbsPath, "rb");
    FILE* dstFp = fopen(headerOutPath, "wb");
    if (!srcFp || !dstFp)
    {
        if (srcFp)
        {
            fclose(srcFp);
        }
        if (dstFp)
        {
            fclose(dstFp);
        }
        return -1;
    }

    const char* filename = strrchr(fbsPath, '/');
    if (!filename)
    {
        filename = fbsPath;
    }
    else
    {
        ++filename;
    }

    size_t filenameLen = strlen(filename);
    char* cppVarName = static_cast<char*>(malloc(filenameLen + 1));
    convertFilename(filename, cppVarName);
    
    fputs(CPP_HEADER, dstFp);
    fputs(cppVarName, dstFp);
    fputs(CPP_MID, dstFp);

    size_t len;
    char buf[256];
    char trimmedBuf[256];
    while (!feof(srcFp) && (len = fread(&buf, 1, sizeof(buf), srcFp)) > 0)
    {
        len = trimFbsWhitespace(buf, trimmedBuf, len);
        fwrite(&trimmedBuf, 1, len, dstFp);
    }
    fputs(CPP_FOOTER, dstFp);

    free(cppVarName);
    fclose(srcFp);
    fclose(dstFp);
    return 0;
}

int main(int argc, const char* argv[])
{
    if (argc != 2 || !strstr(argv[1], ".fbs\0"))
    {
        return -1;
    }

    printf("%s\n", argv[1]);
    
    const char* fbsPath = argv[1];
    const size_t len = strlen(fbsPath);
    char* headerOutPath = static_cast<char*>(malloc(len + 3));
    strncpy(headerOutPath, fbsPath, len);
    headerOutPath[len] = '.';
    headerOutPath[len+1] = 'h';
    headerOutPath[len+2] = '\0';

    int res = perform(fbsPath, headerOutPath);
    free(headerOutPath);
    return res;
}
