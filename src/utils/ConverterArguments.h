#ifndef FORMAT_CONVERTER_CONVERTERARGUMENTS_H
#define FORMAT_CONVERTER_CONVERTERARGUMENTS_H

#include <QString>
#include <QStringList>

#include "CommonEnums.h"

namespace FFmpeg::Converter {

    inline QStringList audioArgs(const QString& inputFilePath,
                                      const QString& outputFilePath,
                                      int enumValue)
    {
        QStringList args;
        args << "-y" << "-i" << inputFilePath;

        switch (static_cast<AudioFormats>(enumValue)) {
            case AudioFormats::MP3:
                args << "-c:a" << "libmp3lame"
                     << "-q:a" << "0";
                break;
            case AudioFormats::WAV:
                args << "-c:a" << "pcm_s16le";
                break;
            case AudioFormats::AAC:
                args << "-c:a" << "aac"
                     << "-b:a" << "256k";
                break;
            case AudioFormats::FLAC:
                args << "-c:a" << "flac";
                break;
            case AudioFormats::OGG:
                args << "-c:a" << "libvorbis"
                     << "-q:a" << "6";
                break;
            case AudioFormats::WMA:
                args << "-c:a" << "wmav2";
                break;
            case AudioFormats::ALAC_M4A:
                args << "-c:a" << "alac";
                break;
            case AudioFormats::AIFF:
                args << "-c:a" << "pcm_s16be";
                break;
            default: break;
        }

        args << outputFilePath;
        return args;
    }

    inline QStringList videoArgs(const QString& inputFilePath,
                                  const QString& outputFilePath,
                                  int enumValue)
    {
        QStringList args;
        args << "-y" << "-i" << inputFilePath;

        switch (enumValue) {
            case static_cast<int>(VideoFormats::MP4):
            case static_cast<int>(VideoFormats::M4V):
                args << "-c:v" << "libx264"
                     << "-preset" << "slow"
                     << "-crf" << "18"
                     << "-c:a" << "aac"
                     << "-b:a" << "192k";
                break;
            case static_cast<int>(VideoFormats::AVI):
                args << "-c:v" << "mpeg4"
                     << "-q:v" << "2"
                     << "-c:a" << "libmp3lame";
                break;
            case static_cast<int>(VideoFormats::MKV):
                args << "-c:v" << "libx264"
                     << "-preset" << "slow"
                     << "-crf" << "18"
                     << "-c:a" << "aac";
                break;
            case static_cast<int>(VideoFormats::MOV):
                args << "-c:v" << "libx264"
                     << "-preset" << "slow"
                     << "-crf" << "18"
                     << "-c:a" << "aac";
                break;
            case static_cast<int>(VideoFormats::WMV):
                args << "-c:v" << "wmv2"
                     << "-c:a" << "wmav2";
                break;
            case static_cast<int>(VideoFormats::FLV):
                args << "-c:v" << "flv1"
                     << "-c:a" << "libmp3lame";
                break;
            case static_cast<int>(VideoFormats::WEBM):
                args << "-c:v" << "libvpx-vp9"
                     << "-b:v" << "0"
                     << "-crf" << "30"
                     << "-c:a" << "libopus";
                break;
            case static_cast<int>(VideoFormats::MPEG):
                args << "-c:v" << "mpeg2video"
                     << "-q:v" << "2"
                     << "-c:a" << "mp2";
                break;
            default: break;
        }

        args << outputFilePath;
        return args;
    }

    inline QStringList imageArgs(const QString& inputFilePath,
                                 const QString& outputFilePath,
                                 int enumValue)
    {
        QStringList args;
        args << "-y" << "-i" << inputFilePath
             << "-update" << "1"
             << "-frames:v" << "1";

        switch (enumValue) {
            case static_cast<int>(ImageFormats::JPEG):
                args << "-q:v" << "1";
                break;
            case static_cast<int>(ImageFormats::GIF):
                args << "-f" << "gif";
                break;
            case static_cast<int>(ImageFormats::HEIF):
                args << "-c:v" << "libx265"
                     << "-x265-params" << "lossless=1"
                     << "-tag:v" << "hvc1";
                break;
            case static_cast<int>(ImageFormats::WEBP):
                args << "-c:v" << "libwebp"
                     << "-lossless" << "1";
                break;
            default: break;
        }

        args << outputFilePath;
        return args;
    }
}

namespace FFmpeg::RemoveMetadata {

    inline QStringList mp3Args(const QString& inputFilePath, const QString& outputFilePath)
    {
        QStringList args;
        args << "-y" << "-i"
            << inputFilePath
             << "-map_metadata" << "-1"
             << "-c" << "copy"
             << outputFilePath;

        return args;
    }

    inline QStringList wavArgs(const QString& inputFilePath, const QString& outputFilePath)
    {
        QStringList args;
        args << "-i" << inputFilePath
             << "-map_metadata" << "-1"
             << "-c:a" << "pcm_s16le"
             << outputFilePath;
        return args;
    }

    inline QStringList mkvArgs(const QString& inputFilePath, const QString& outputFilePath)
    {
        QStringList args;
        args << "-i" << inputFilePath
             << "-map" << "0"
             << "-map_metadata" << "-1"
             << "-c" << "copy"
             << outputFilePath;

        return args;
    }
}

namespace FFprobe {
    inline QStringList fragmentedArgs (const QString& filePath)
    {
        QStringList args;
        args << "-v" << "error"
         << "-show_entries" << "moof"
         << "-of" << "default=nw=1"
         << filePath;

        return args;
    }
}

namespace ExifTool::RemoveMetadata {

    // images
    // legacyVideos (AVI, FLV, WMV)
    // standard audio (FLAC, OGG, ACC, WMA, AIFF, ?WAV?)
    inline QStringList standardArgs (const QString& filePath)
    {
        return {
            "-all=",
            "-overwrite_original",
            filePath
        };
    }

    // videos MP4, MOV, M4V
    // audio ALAC, M4A
    inline QStringList quickTimeArgs(const QString& filePath)
    {
        return {
            "-all=",
            "-XMP:all=",
            "-QuickTime:all=",
            "-UserData:all=",
            "-Keys:all=",
            "-overwrite_original",
            filePath
        };
    }

    inline QStringList audioArgs(const QString& filePath, int enumValue)
    {
        switch (static_cast<AudioFormats>(enumValue)) {
            case AudioFormats::ALAC_M4A:
                return quickTimeArgs(filePath);

            case AudioFormats::AAC:
            case AudioFormats::AIFF:
            case AudioFormats::FLAC:
            case AudioFormats::OGG:
            case AudioFormats::WMA:
                return standardArgs(filePath);

            // ExifTool doesn't support MP3, WAV
            case AudioFormats::MP3:
            case AudioFormats::WAV:
            default:
                return {};
        }
    }

    inline QStringList imageArgs(QString filePath, int enumValue)
    {
        return standardArgs(filePath);
    }

    inline QStringList videoArgs(QString filePath, int enumValue)
    {
        switch (static_cast<VideoFormats>(enumValue)) {
            case VideoFormats::MP4:
            case VideoFormats::MOV:
            case VideoFormats::M4V:
                return quickTimeArgs(filePath);

            case VideoFormats::AVI:
            case VideoFormats::FLV:
            case VideoFormats::WMV:
                return standardArgs(filePath);

            // Exiftool doesn't support WEBM MPEG or MKV
            case VideoFormats::WEBM:
            case VideoFormats::MPEG:
            case VideoFormats::MKV:
            default:
                return {};
        }
    }
}

namespace ExifTool::CopyMetadata {
    inline QStringList standardArgs(const QString& inputFilePath, const QString& outputFilePath)
    {
        QStringList args;
        args << "-TagsFromFile" << inputFilePath
             << "-All:All"
             << "-All:All>XMP"
             << "-overwrite_original"
             << "-ignoreMinorErrors"
             << outputFilePath;

        return args;
    }
}

namespace Arguments {
    inline QStringList converter(const QString& inputFilePath,
                                 const QString& outputFilePath,
                                 FormatInfo format)
    {
        QStringList args;
        switch (format.fileType) {
            case FileType::AUDIO:
                args = FFmpeg::Converter::audioArgs(inputFilePath, outputFilePath, format.enumValue);
                break;

            case FileType::VIDEO:
                args = FFmpeg::Converter::videoArgs(inputFilePath, outputFilePath, format.enumValue);
                break;

            case FileType::IMAGE:
                args = FFmpeg::Converter::imageArgs(inputFilePath, outputFilePath, format.enumValue);
                break;

            case FileType::UNKNOWN:
                args = {};
                break;
        }

        return args;
    }

    inline QStringList metadata(const QString& filePath,
                                FormatInfo format)
    {
        QStringList args;

        switch (format.fileType) {
            case FileType::AUDIO:
                args = ExifTool::RemoveMetadata::audioArgs(filePath, format.enumValue);
                break;

            case FileType::IMAGE:
                args = ExifTool::RemoveMetadata::imageArgs(filePath, format.enumValue);
                break;

            case FileType::VIDEO:
                args = ExifTool::RemoveMetadata::videoArgs(filePath, format.enumValue);
                break;

            case FileType::UNKNOWN:
                args = {};
        }

        return args;
    }
}

#endif //FORMAT_CONVERTER_CONVERTERARGUMENTS_H