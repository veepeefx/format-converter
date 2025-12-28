#ifndef FORMAT_CONVERTER_COMMONENUMS_H
#define FORMAT_CONVERTER_COMMONENUMS_H
#include <qmap.h>
#include <qset.h>
#include <qstring.h>

enum class FileType {
    AUDIO,
    VIDEO,
    IMAGE,
    UNKNOWN
};

enum class AudioFormats {
    MP3, WAV, AAC, FLAC, OGG, WMA, ALAC_M4A, AIFF
};

static const QMap<QString, AudioFormats> audioFormatLabels = {
    { "mp3", AudioFormats::MP3 },
    { "wav", AudioFormats::WAV },
    { "aac", AudioFormats::AAC },
    { "flac", AudioFormats::FLAC },
    { "ogg", AudioFormats::OGG },
    { "wma", AudioFormats::WMA },
    { "m4a", AudioFormats::ALAC_M4A },
    { "aiff", AudioFormats::AIFF },
};

enum class VideoFormats {
    MP4, AVI, MKV, MOV, WMV, FLV, WEBM, MPEG, M4V
};

static const QMap<QString, VideoFormats> videoFormatLabels = {
    { "mp4", VideoFormats::MP4 },
    { "avi", VideoFormats::AVI },
    { "mkv", VideoFormats::MKV },
    { "mov", VideoFormats::MOV },
    { "wmv", VideoFormats::WMV },
    { "flv", VideoFormats::FLV },
    { "webm", VideoFormats::WEBM },
    { "mpeg", VideoFormats::MPEG },
    { "mpg", VideoFormats::MPEG },
    { "m4v", VideoFormats::M4V }
};

enum class ImageFormats {
    JPEG, PNG, GIF, BMP, TIFF, HEIF, WEBP, ICO
};

static const QMap<QString, ImageFormats> imageFormatLabels = {
    { "jpg", ImageFormats::JPEG },
    { "jpeg", ImageFormats::JPEG },
    { "png", ImageFormats::PNG },
    { "gif", ImageFormats::GIF },
    { "bmp", ImageFormats::BMP },
    { "tif", ImageFormats::TIFF },
    { "tiff", ImageFormats::TIFF },
    { "heif", ImageFormats::HEIF },
    { "heic", ImageFormats::HEIF },
    { "ico", ImageFormats::ICO }
};

// includes duplicates (multiple forms) in format labels for example jpg and jpeg only jpg is shown
static const QSet<QString> labelsBlackList = {
    "mpg", "jpeg", "tiff", "heic"
};


#endif //FORMAT_CONVERTER_COMMONENUMS_H