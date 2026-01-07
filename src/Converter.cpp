#include "Converter.h"
#include "utils/CommonEnums.h"
#include "utils/ConverterArguments.h"

#include <iostream>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>


void Converter::runConverter(const QString& inputFilePath, const QString& outputFilePath)
{
    emit newLogMessage("\nStarting format converter...\n");
    FormatInfo format = getOutputFormat(outputFilePath);
    QStringList args;

    // building arguments depending on filetype and format
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
            emit newLogMessage("File type unknown!");
            return;
    }

    runFFmpeg(args);
}

void Converter::runMetaDataRemover(const QString& inputFilePath, const QString& outputFilePath)
{
    emit newLogMessage("\nStarting metadata remover...\n");
    FormatInfo format = getOutputFormat(outputFilePath);

    // if outputfile exists it gets removed
    if (QFile::exists(outputFilePath)) {
        QFile::remove(outputFilePath);
    }

    // and we copy our input file to output
    if (!QFile::copy(inputFilePath, outputFilePath)) {
        emit newLogMessage("\nFailed to make copy of the original file!\n");
        return;
    }

    switch (format.fileType) {
        // all audio and image formats are supported by exiftool
        case FileType::AUDIO:
        case FileType::IMAGE:
            runExifTool(ExifTool::exifArgs(outputFilePath));
            break;

        // only mp4 type non-fragmented formats are supported by exiftool we fall back to ffmpeg
        case FileType::VIDEO:
            //runFFmpeg(FFmpeg::RemoveMetaData::videoArgs(outputFilePath, outputFilePath));
            emit newLogMessage("Not in use");
            break;

        case FileType::UNKNOWN:
            emit newLogMessage("File type unknown!");
    }
}

void Converter::runFFmpeg(const QStringList& args)
{
    // emitting signal that ffmpeg processing has started
    emit progressChanged(0);
    emit newLogMessage("FFmpeg running...");

    QProcess* ffmpeg = new QProcess(this);

    // connecting progress updates
    totalDuration_ = 0;
    connect(ffmpeg, &QProcess::readyReadStandardError, [ffmpeg, this]() {
        handleProgress(ffmpeg->readAllStandardError());
    });

    // emitting error signal
    connect(ffmpeg, &QProcess::errorOccurred, [this]() {
        emit newLogMessage("\nProgress failed during FFmpeg process!\n");
    });

    // emitting finished signal
    connect(ffmpeg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this] () {
        emit progressChanged(100, true);
        emit newLogMessage("\nFFmpeg finished!\n");
    });

    ffmpeg->start("ffmpeg", args);
}

void Converter::runExifTool(const QStringList& args)
{
    emit newLogMessage("ExifTool Running...");
    QProcess* exifTool = new QProcess(this);

    connect(exifTool, &QProcess::readyReadStandardError, [exifTool, this]() {
        emit newLogMessage("ExifTool: " + exifTool->readAllStandardError().trimmed());
    });

    // emitting error signal
    connect(exifTool, &QProcess::errorOccurred, [this]() {
        emit newLogMessage("\nProgress failed during ExifTool process!\n");
    });

    // emitting finished signal
    connect(exifTool, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this] () {
        emit progressChanged(100, true);
        emit newLogMessage("\nExifTool finished!\n");
    });

    exifTool->start("exiftool", args);
}
/*
void Converter::removeVideoMetadata(const QString& filePath, VideoFormats format)
{
    if (format == VideoFormats::MP4 || format == VideoFormats::MOV || format == VideoFormats::M4V) {
        // fragmented mp4 like files ffmpeg + exiftool
        if (!isFragmented(filePath)) {
            std::cerr << "JEE";
            runFFmpeg(FFmpeg::RemoveMetaData::videoArgs(filePath, filePath));
            connect(this, &Converter::progressFinished, this, [this, filePath]() {
                runExifTool(ExifTool::exifArgs(filePath));
                std::cerr << "JEE";
                disconnect(this, &Converter::progressFinished, nullptr, nullptr);
            });
        // non fragmented mp4 needs only exiftool
        } else {
            runExifTool(ExifTool::exifArgs(filePath));
        }
    // other filetypes can be cleared with just ffmpeg
    } else {
        runFFmpeg(FFmpeg::RemoveMetaData::videoArgs(filePath, filePath));
    }
}

bool Converter::isFragmented(const QString &filePath)
{
    QProcess ffprobe;
    ffprobe.start("ffprobe", FFprobe::fragmentedArgs(filePath));
    ffprobe.waitForFinished(-1);

    QString output = ffprobe.readAllStandardOutput().trimmed();
    return !output.isEmpty();
}
*/
void Converter::handleProgress(const QString& text)
{
    for (const QString &line : text.split('\n')) {

        const QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) { continue; }
        if (!line.contains("time=") && !line.contains("Duration")) { continue; }

        emit newLogMessage("FFmpeg: " + trimmed);

        if (totalDuration_ == 0) {
            getTotalDuration(line);
            continue;
        }

        getProgress(line);
    }
}

void Converter::getTotalDuration(const QString &line)
{
    static const QRegularExpression reDuration(R"(Duration: (\d+):(\d+):(\d+\.\d+))");

    QRegularExpressionMatch match = reDuration.match(line);
    if (match.hasMatch()){
        totalDuration_ = match.captured(1).toInt() * 3600
                        + match.captured(2).toInt() * 60
                        + match.captured(3).toDouble();
    }
}

void Converter::getProgress(const QString& line)
{
    static const QRegularExpression reTime(R"(time=(\d+):(\d+):(\d+\.\d+))" );

    QRegularExpressionMatch match = reTime.match(line);
    if (match.hasMatch() && totalDuration_ > 0) {
        double current = match.captured(1).toInt() * 3600
                        + match.captured(2).toInt() * 60
                        + match.captured(3).toDouble();
        int progress = static_cast<int>((current / totalDuration_) * 100);

        if (progress >= 100) { progress = 100; }
        emit progressChanged(progress);
    }
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
    return {FileType::UNKNOWN};
}

