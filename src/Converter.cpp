#include "Converter.h"

#include <iostream>
#include <QProcess>

bool Converter::runConverter(const QString& inputFilePath, const QString& outputFilePath)
{
    QStringList args;
    args << "-y" << "-i"
            << inputFilePath
            << outputFilePath;

    return runFFmpeg(args);
}

bool Converter::runMetaDataRemover(const QString& inputFilePath, const QString& outputFilePath)
{
    QStringList args;
    args << "-y" << "-i" << inputFilePath
         << "-map_metadata" << "-1"
         << "-frames:v" << "1"
         << "-q:v" << "1"
            << outputFilePath;

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
