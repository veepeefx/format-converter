#ifndef FORMAT_CONVERTER_COMMONENUMS_H
#define FORMAT_CONVERTER_COMMONENUMS_H

#include <QSet>
#include <QString>

enum class ProcessType {
    FFMPEG,
    EXIFTOOL
};

inline QString processTypeToString(ProcessType process)
{
    switch (process) {
        case ProcessType::FFMPEG:   return "FFmpeg";
        case ProcessType::EXIFTOOL: return "ExifTool";
        default:                    return "Unknown";
    }
}

enum class FileType {
    AUDIO,
    VIDEO,
    IMAGE,
    UNKNOWN
};

enum class AudioFormats {
    MP3, WAV, AAC, FLAC, OGG, WMA, ALAC_M4A, AIFF
};

enum class VideoFormats {
    MP4, AVI, MKV, MOV, WMV, FLV, WEBM, MPEG, M4V
};

// GIF, BMP, ICO no metadata by design
enum class ImageFormats {
    JPEG, PNG, GIF, BMP, TIFF, HEIF, WEBP, ICO
};

struct FormatInfo {
    FileType fileType;
    int enumValue;
    QString label;
};

static QVector<FormatInfo> fileFormats {
        // AUDIO
        { FileType::AUDIO, static_cast<int>(AudioFormats::MP3), "mp3" },
        { FileType::AUDIO, static_cast<int>(AudioFormats::WAV), "wav" },
        { FileType::AUDIO, static_cast<int>(AudioFormats::AAC), "aac" },
        { FileType::AUDIO, static_cast<int>(AudioFormats::FLAC), "flac" },
        { FileType::AUDIO, static_cast<int>(AudioFormats::OGG), "ogg" },
        { FileType::AUDIO, static_cast<int>(AudioFormats::WMA), "wma" },
        { FileType::AUDIO, static_cast<int>(AudioFormats::ALAC_M4A), "m4a" },
        { FileType::AUDIO, static_cast<int>(AudioFormats::AIFF), "aiff" },

        // VIDEO
        { FileType::VIDEO, static_cast<int>(VideoFormats::MP4), "mp4" },
        { FileType::VIDEO, static_cast<int>(VideoFormats::AVI), "avi" },
        { FileType::VIDEO, static_cast<int>(VideoFormats::MKV), "mkv" },
        { FileType::VIDEO, static_cast<int>(VideoFormats::MOV), "mov" },
        { FileType::VIDEO, static_cast<int>(VideoFormats::WMV), "wmv" },
        { FileType::VIDEO, static_cast<int>(VideoFormats::FLV), "flv" },
        { FileType::VIDEO, static_cast<int>(VideoFormats::WEBM), "webm" },
        { FileType::VIDEO, static_cast<int>(VideoFormats::MPEG), "mpeg" },
        { FileType::VIDEO, static_cast<int>(VideoFormats::MPEG), "mpg" },
        { FileType::VIDEO, static_cast<int>(VideoFormats::M4V), "m4v" },

        // IMAGE
        { FileType::IMAGE, static_cast<int>(ImageFormats::JPEG), "jpeg" },
        { FileType::IMAGE, static_cast<int>(ImageFormats::JPEG), "jpg" },
        { FileType::IMAGE, static_cast<int>(ImageFormats::PNG), "png" },
        { FileType::IMAGE, static_cast<int>(ImageFormats::GIF), "gif" },
        { FileType::IMAGE, static_cast<int>(ImageFormats::BMP), "bmp" },
        { FileType::IMAGE, static_cast<int>(ImageFormats::TIFF), "tif" },
        { FileType::IMAGE, static_cast<int>(ImageFormats::TIFF), "tiff" },
        { FileType::IMAGE, static_cast<int>(ImageFormats::HEIF), "heif" },
        { FileType::IMAGE, static_cast<int>(ImageFormats::HEIF), "heic" },
        { FileType::IMAGE, static_cast<int>(ImageFormats::WEBP), "webp"},
        { FileType::IMAGE, static_cast<int>(ImageFormats::ICO), "ico" },
};

// includes duplicates (multiple forms) in format labels for example jpg and jpeg only jpg is shown
static const QSet<QString> labelsBlackList = {
    "mpg", "jpeg", "tiff", "heic"
};


#endif //FORMAT_CONVERTER_COMMONENUMS_H