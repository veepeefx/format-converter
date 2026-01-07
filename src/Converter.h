#ifndef FORMAT_CONVERTER_CONVERTER_H
#define FORMAT_CONVERTER_CONVERTER_H
#include <QObject>
#include <QString>

#include "utils/CommonEnums.h"

class Converter : public QObject {
    Q_OBJECT

public:

    Converter(QObject* parent = nullptr) : QObject(parent), totalDuration_(0) {};
    ~Converter() = default;

    void runConverter(const QString& inputFilePath, const QString& outputFilePath);
    void runMetaDataRemover(const QString& inputFilePath, const QString& outputFilePath);

private:

    double totalDuration_;
    void runFFmpeg(const QStringList& args);
    void runExifTool(const QStringList& args);

    void removeVideoMetadata(const QString& filePath, VideoFormats format);
    bool isFragmented(const QString& filePath);

    // handless ffmpeg progress updates
    void handleProgress(const QString& text);
    void getTotalDuration(const QString& line);
    void getProgress(const QString& line);

    static FormatInfo getOutputFormat(const QString& outputFilePath);

signals:
    // converter signals
    void progressChanged(int value, bool isFinished = false);
    void progressFinished();
    void newLogMessage(const QString& logMessage);

};


#endif //FORMAT_CONVERTER_CONVERTER_H