#include "Converter.h"
#include "utils/CommonEnums.h"
#include "utils/ConverterArguments.h"

#include <iostream>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>


void Converter::runConverter(const QString& inputFilePath, const QString& outputFilePath)
{
    emit newLogMessage("\nStarting...\n");
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
    emit newLogMessage("\nStarting...\n");
    FormatInfo format = getOutputFormat(outputFilePath);
    QStringList args;

    // building arguments depending on filetype and format
    switch (format.fileType) {
        case FileType::AUDIO:
            break;
        case FileType::VIDEO:
            break;
        case FileType::IMAGE:
            break;
        case FileType::UNKNOWN:
            emit newLogMessage("File type unknown!");
            return;
    }

    runFFmpeg(args);
}

void Converter::runFFmpeg(const QStringList& args)
{
    // emitting signal that ffmpeg processing has started
    emit progressChanged(0);

    QProcess* ffmpeg = new QProcess(this);

    // connecting progress updates
    totalDuration_ = 0;
    connect(ffmpeg, &QProcess::readyReadStandardError, [ffmpeg, this]() {
        handleProgress(ffmpeg->readAllStandardError());
    });

    // emitting error signal
    connect(ffmpeg, &QProcess::errorOccurred, [this]() {
        emit newLogMessage("\nProgress failed during process!\n");
    });

    // emitting finished signal
    connect(ffmpeg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this] () {
        emit progressChanged(100, true);
        emit newLogMessage("\nProgress finished!\n");
    });

    ffmpeg->start("ffmpeg", args);
}

void Converter::handleProgress(const QString& text)
{
    for (const QString &line : text.split('\n')) {

        const QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) { continue; }
        if (!line.contains("time=") && !line.contains("Duration")) { continue; }

        emit newLogMessage(trimmed);

        if (totalDuration_ == 0 && !trimmed.contains("Duration")) {
            getTotalDuration(line);
            continue;
        }

        if (trimmed.contains("time=")) {
            getProgress(line);
        }
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

