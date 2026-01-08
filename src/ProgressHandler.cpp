#include "ProgressHandler.h"

#include <QRegularExpression>
#include <QString>


void ProgressHandler::handleFfmpegProgress(const QString& text)
{
    for (const QString &line : text.split('\n')) {

        const QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) { continue; }
        if (!line.contains("time=") && !line.contains("Duration")) { continue; }

        emit logMessage("FFmpeg: " + trimmed);

        if (totalDuration_ == 0) {
            getTotalDuration(line);
            continue;
        }

        getProgress(line);
    }
}

void ProgressHandler::handleExifToolProgress(const QString &text)
{
    emit logMessage("ExifTool: " + text);
    if (text.contains("Can't write a")) {
        emit logMessage("ExifTool: Using best effort to move all metadata");
    }
}

void ProgressHandler::progressStarted(QString processName)
{
    emit updateProgress(0);
    emit logMessage("\n" + processName + " started!\n");
}

void ProgressHandler::progressFinished(QString processName, bool lastConversion)
{
    emit updateProgress(100, true);
    emit logMessage("\n" + processName + " finished!");
    emit finished();

    // progress has ended so next duration is new process
    totalDuration_ = 0;

    if (lastConversion) {
        emit allDone();
    }
}

void ProgressHandler::progressFailed(QString processName)
{
    emit logMessage("\nProgress failed during " + processName + " process!\n");
}

void ProgressHandler::getTotalDuration(const QString &line)
{
    static const QRegularExpression reDuration(R"(Duration: (\d+):(\d+):(\d+\.\d+))");

    QRegularExpressionMatch match = reDuration.match(line);
    if (match.hasMatch()){
        totalDuration_ = match.captured(1).toInt() * 3600
                        + match.captured(2).toInt() * 60
                        + match.captured(3).toDouble();
    }
}

void ProgressHandler::getProgress(const QString& line)
{
    static const QRegularExpression reTime(R"(time=(\d+):(\d+):(\d+\.\d+))" );

    QRegularExpressionMatch match = reTime.match(line);
    if (match.hasMatch() && totalDuration_ > 0) {
        double current = match.captured(1).toInt() * 3600
                        + match.captured(2).toInt() * 60
                        + match.captured(3).toDouble();
        int progress = static_cast<int>((current / totalDuration_) * 100);

        if (progress >= 100) { progress = 100; }
        emit updateProgress(progress);
    }
}