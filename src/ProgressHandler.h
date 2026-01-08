#ifndef FORMAT_CONVERTER_PROGRESSHANDLER_H
#define FORMAT_CONVERTER_PROGRESSHANDLER_H
#include <QObject>

class ProgressHandler : public QObject {
    Q_OBJECT

public:
    ProgressHandler(QObject* parent = nullptr) : QObject(parent), totalDuration_(0.0) {}

    ~ProgressHandler() = default;

    void handleFfmpegProgress(const QString& text);
    void handleExifToolProgress(const QString& text);

    void progressStarted(QString progressName);
    void progressFinished(QString progressName, bool lastConversion);
    void progressFailed(QString processName);

private:

    double totalDuration_;

    void getTotalDuration(const QString &line);
    void getProgress(const QString& line);

signals:

    void updateProgress(int percent, bool isFinished = false);
    void logMessage(const QString& message);
    void finished();
    void allDone();

};


#endif //FORMAT_CONVERTER_PROGRESSHANDLER_H