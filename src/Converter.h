#ifndef FORMAT_CONVERTER_CONVERTER_H
#define FORMAT_CONVERTER_CONVERTER_H
#include <QObject>
#include <QString>

#include "CommonEnums.h"

class Converter : public QObject {
    Q_OBJECT

public:

    Converter(QObject* parent = nullptr);
    ~Converter();

    void runConverter(const QString& inputFilePath, const QString& outputFilePath);
    void runMetaDataRemover(const QString& inputFilePath, const QString& outputFilePath);

private:

    double totalDuration_;

    // builders for ffmpeg arguments
    static void updateVideoArgs(QStringList& args, int enumValue);
    static void updateAudioArgs(QStringList& args, int enumValue);
    static void updateImageArgs(QStringList& args, int enumValue);

    void runFFmpeg(const QStringList& args);

    // handless ffmpeg progress updates
    void handleProgress(const QString& text);

    FormatInfo getOutputFormat(const QString& outputFilePath);

signals:
    // converter signals
    void progressChanged(int value, bool isFinished = false);
    void progressFinished();
    void newLogMessage(const QString& logMessage);

};


#endif //FORMAT_CONVERTER_CONVERTER_H