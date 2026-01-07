#ifndef FORMAT_CONVERTER_DEPENDENCYCHECKER_H
#define FORMAT_CONVERTER_DEPENDENCYCHECKER_H
#include <cstdlib>

class DependencyChecker {
public:

    // ffmpeg check
    static bool isFFmpegAvailable()
    {
        static const bool available = checkFFmpeg();
        return available;
    }

    // exiftool check
    static bool isExifToolAvailable()
    {
        static const bool available = checkExifTool();
        return available;
    }

private:

    static bool checkFFmpeg()
    {
#ifdef _WIN32
    return std::system("ffmpeg -version >nul 2>&1") == 0;
#else
    return std::system("ffmpeg -version >/dev/null 2>&1") == 0;
#endif
    }

    static bool checkExifTool()
    {
#ifdef _WIN32
        return std::system("exiftool -ver >nul 2>&1") == 0;
#else
        return std::system("exiftool -ver >/dev/null 2>&1") == 0;
#endif
    }

    DependencyChecker() = delete;
    ~DependencyChecker() = delete;
};


#endif //FORMAT_CONVERTER_DEPENDENCYCHECKER_H