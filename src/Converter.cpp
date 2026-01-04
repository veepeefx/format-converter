#include "Converter.h"

#include <iostream>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>

#include "CommonEnums.h"

Converter::Converter(QObject* parent) : QObject(parent), totalDuration_(0) {}

Converter::~Converter() {}

void Converter::runConverter(const QString& inputFilePath, const QString& outputFilePath)
{
    FormatInfo outputFormat = getOutputFormat(outputFilePath);

    QStringList args;
    args << "-y" << "-i" << inputFilePath;

    // building arguments depending on filetype and format
    switch (outputFormat.fileType) {
        case FileType::AUDIO:
            updateAudioArgs(args, outputFormat.enumValue);
            break;
        case FileType::VIDEO:
            updateVideoArgs(args, outputFormat.enumValue);
            break;
        case FileType::IMAGE:
            updateImageArgs(args, outputFormat.enumValue);
            break;
        case FileType::UNKNOWN:
            emit errorOccured("File type unknown");
            return;
    }

    args << outputFilePath;
    runFFmpeg(args);
}

void Converter::runMetaDataRemover(const QString& inputFilePath, const QString& outputFilePath)
{
    FormatInfo outputFormat = getOutputFormat(outputFilePath);

    QStringList args;
    args << "-y" << "-i" << inputFilePath
         << "-map_metadata" << "-1";

    switch (outputFormat.fileType) {
        case FileType::AUDIO:
        case FileType::VIDEO:
            args << "-c" << "copy";
            break;
        case FileType::IMAGE:
            // JPEG need re encoding as it is lossy
            if (static_cast<ImageFormats>(outputFormat.enumValue) == ImageFormats::JPEG) {
                args << "-frames:v" << "1"
                     << "-q:v" << "1";
            }
            // other image types are lossless so FFmpeg makes lossless copy it self
            break;
        case FileType::UNKNOWN:
            errorOccured("File type unknown");
            return;
    }

    args << outputFilePath;
    runFFmpeg(args);
}

void Converter::runFFmpeg(const QStringList& args)
{
    emit progressChanged(0);
    QProcess* ffmpeg = new QProcess(this);

    // connecting progress updates
    totalDuration_ = 0;
    connect(ffmpeg, &QProcess::readyReadStandardError, [ffmpeg, this]() {
        handleProgress(ffmpeg->readAllStandardError());
    });

    // emitting error signal
    connect(ffmpeg, &QProcess::errorOccurred, [this]() {
        emit errorOccured("During running ffmpeg!");
    });

    // emitting finished signal
    connect(ffmpeg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this] () {
        emit progressChanged(100);
    });

    ffmpeg->start("ffmpeg", args);
}

FormatInfo Converter::getOutputFormat(const QString& outputFilePath)
{
    QString suffix = QFileInfo(outputFilePath).suffix();
    for (const FormatInfo& it : fileFormats) {
        if (it.label == suffix) {
            // always finds correct it as only supported suffixes are given previously
            return it;
        }
    }
    return {};
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

void Converter::handleProgress(const QString& text)
{
    std::cerr << "JEE";
    if (totalDuration_ == 0) {
        QRegularExpression reDuration("Duration: (\\d+):(\\d+):(\\d+\\.\\d+)");
        QRegularExpressionMatch match = reDuration.match(text);
        if (match.hasMatch()){
            totalDuration_ = match.captured(1).toInt() * 3600
                            + match.captured(2).toInt() * 60
                            + match.captured(3).toDouble();
        }
    }

    // read current
    QRegularExpression reTime("time=(\\d+):(\\d+):(\\d+\\.\\d+)");
    QRegularExpressionMatch match = reTime.match(text);
    if (match.hasMatch() && totalDuration_ > 0) {
        double current = match.captured(1).toInt() * 3600
                        + match.captured(2).toInt() * 60
                        + match.captured(3).toDouble();
        int progress = int((current / totalDuration_) * 100);
        emit progressChanged(progress);
    }
}
