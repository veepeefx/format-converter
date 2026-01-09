#ifndef FORMAT_CONVERTER_CONVERTER_H
#define FORMAT_CONVERTER_CONVERTER_H
#include <QProcess>
#include <QString>

#include "ProgressHandler.h"
#include "utils/CommonEnums.h"

enum class State {
    IDLE,
    RUNNING,            // some function is running
    FFMPEG_RUNNING,     // while FFmpeg running in QProcess
    EXIFTOOL_RUNNING    // while ExifTool running in QProcess
};

class Converter : public QObject {
    Q_OBJECT

public:

    Converter(QObject* parent = nullptr);
    ~Converter() = default;

    void runConverter(const QString& inputFilePath, const QString& outputFilePath, bool saveMetadata);
    void runMetadataRemover(const QString& inputFilePath, const QString& outputFilePath);

private:

    // keeps track on converter state. new calls are accepted when state is idle
    State state_ = State::IDLE;
    ProgressHandler progressHandler_;

    void copyMetadata(const QString& inputFilePath, const QString& outputFilePath, FileType type);
    bool checkInputAndOutput(const QString& inputFilePath, const QString& outputFilePath);
    bool copyAndReplaceFile(const QString &inputFilePath, const QString &outputFilePath);
    void ffmpegMetadataRemoval(const QString& inputFilePath, const QString& outputFilePath, FormatInfo format);

    void runProcess(ProcessType processType, const QStringList& args, bool lastConversion = true);
    void connectProcesses(QProcess* process, ProcessType processType, bool lastConversion);

    static FormatInfo getFileFormat(const QString& filePath);

private slots:
    void conversionEnded();

signals:
    void allDone();
    void error(const QString& message);

    // pass trought signals from progress handler to main window
    void onUpdateProgress(int percent, bool isFinished = false);
    void onLogMessage(const QString& message);
    void onFinished();
};


#endif //FORMAT_CONVERTER_CONVERTER_H