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

    void runFFmpeg(const QStringList& args);
    void updateVideoArgs(QStringList& args, const int& enumValue);
    void updateAudioArgs(QStringList& args, const int& enumValue);
    void updateImageArgs(QStringList& args, const int& enumValue);
    void handleProgress(const QString& text);

    FormatInfo getOutputFormat(const QString& outputFilePath);

signals:
    void progressChanged(const int& value);
    void errorOccured(const QString& errorMessage);

};


#endif //FORMAT_CONVERTER_CONVERTER_H