#include "Converter.h"

#include <iostream>
#include <QProcess>

bool Converter::runConverter(const QString& inputFilePath, const QString& outputFilePath)
{
    QProcess ffmpeg;
    QString program = "ffmpeg";
    QStringList args;
    args << "-y" << "-i" << inputFilePath << outputFilePath;

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

    QByteArray output = ffmpeg.readAllStandardOutput();
    std::cout << output.toStdString() << std::endl;

    return true;
}