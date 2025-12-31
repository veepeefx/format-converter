#include "Converter.h"

#include <iostream>
#include <QProcess>

#include "CommonEnums.h"

bool Converter::runConverter(const QString& inputFilePath,
                             const QString& outputFilePath,
                             const FormatInfo& inputFormat)
{
    QStringList args;
    args << "-y" << "-i" << inputFilePath;

    // building arguments depending on filetype and format
    switch (inputFormat.fileType) {
        case FileType::AUDIO:
            updateAudioArgs(args, inputFormat.enumValue);
            break;
        case FileType::VIDEO:
            updateVideoArgs(args, inputFormat.enumValue);
            break;
        case FileType::IMAGE:
            updateImageArgs(args, inputFormat.enumValue);
            break;
        case FileType::UNKNOWN:
            return false;
    }

    args << outputFilePath;
    return runFFmpeg(args);
}

bool Converter::runMetaDataRemover(const QString& inputFilePath,
                                   const QString& outputFilePath,
                                   const FormatInfo& inputFormat)
{
    QStringList args;

    // building arguments depending on filetype
    args << "-y" << "-i" << inputFilePath
         << "-map_metadata" << "-1";

    switch (inputFormat.fileType) {
        case FileType::AUDIO:
        case FileType::VIDEO:
            args << "-c" << "copy";
            break;
        case FileType::IMAGE:
            // JPEG need re encoding as it is lossy
            if (static_cast<ImageFormats>(inputFormat.enumValue) == ImageFormats::JPEG) {
                args << "-frames:v" << "1"
                     << "-q:v" << "1";
            }
            // other image types are lossless so FFmpeg makes lossless copy it self
            break;
        case FileType::UNKNOWN:
            return false;
    }

    args << outputFilePath;
    return runFFmpeg(args);
}

bool Converter::runFFmpeg(QStringList args)
{
    QProcess ffmpeg;
    QString program = "ffmpeg";

    QObject::connect(&ffmpeg, &QProcess::readyReadStandardError, [&]() {
        QByteArray data = ffmpeg.readAllStandardError();
        std::cerr << data.toStdString();
    });

    ffmpeg.start(program, args);

    if (!ffmpeg.waitForStarted()) {
        std::cerr << "FFmpeg failed to start" << std::endl;
        return false;
    }

    if (!ffmpeg.waitForFinished(20000)) {
        std::cerr << "FFmpeg timed out" << std::endl;
        ffmpeg.kill();
        return false;
    }

    return true;
}

void Converter::updateAudioArgs(QStringList &args, const int &enumValue)
{
    switch (enumValue) {
        case static_cast<int>(AudioFormats::MP3):
            args << "-c:a" << "libmp3lame"
                 << "-q:a" << "0";
            break;
        case static_cast<int>(AudioFormats::WAV):
            args << "-c:a" << "pcm_s16le";
            break;
        case static_cast<int>(AudioFormats::AAC):
            args << "-c:a" << "aac"
                 << "-b:a" << "256k";
            break;
        case static_cast<int>(AudioFormats::FLAC):
            args << "-c:a" << "flac";
            break;
        case static_cast<int>(AudioFormats::OGG):
            args << "-c:a" << "libvorbis"
                 << "-q:a" << "6";
            break;
        case static_cast<int>(AudioFormats::WMA):
            args << "-c:a" << "wmav2";
            break;
        case static_cast<int>(AudioFormats::ALAC_M4A):
            args << "-c:a" << "alac";
            break;
        case static_cast<int>(AudioFormats::AIFF):
            args << "-c:a" << "pcm_s16be";
            break;
        default: break;
    }
}

void Converter::updateVideoArgs(QStringList &args, const int &enumValue)
{

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
}

void Converter::updateImageArgs(QStringList &args, const int &enumValue)
{
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
}