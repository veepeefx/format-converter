#include "Converter.h"
#include "utils/CommonEnums.h"
#include "utils/ConverterArguments.h"
#include "ProgressHandler.h"

#include <iostream>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>


Converter::Converter(QObject* parent) : QObject(parent)
{
    // passing progress handler signals to main window
    connect(&progressHandler_, &ProgressHandler::updateProgress, this, &Converter::onUpdateProgress);
    connect(&progressHandler_, &ProgressHandler::logMessage, this, &Converter::onLogMessage);
    connect(&progressHandler_, &ProgressHandler::finished, this, &Converter::onFinished);

    // signal when full conversion is ended (example ffmpeg + exiftool encoding + metadata move)
    connect(&progressHandler_, &ProgressHandler::allDone, this, &Converter::conversionEnded);
}

void Converter::runConverter(const QString& inputFilePath, const QString& outputFilePath, bool saveMetadata)
{
    if (state_ != State::IDLE) {
        return;
    }
    state_ = State::RUNNING;
    emit onLogMessage("\nStarting format converter...\n");

    FormatInfo format = getFileFormat(outputFilePath);

    // building arguments depending on filetype and format
    QStringList args = Arguments::converter(inputFilePath, outputFilePath, format);

    // if args are empty stop running
    if (args.empty()) {
        emit onLogMessage("File type unknown!");
        state_ = State::IDLE;
        return;
    }

    // if metadata isn't saved we only run ffmpeg and end
    if (!saveMetadata) {
        runProcess(ProcessType::FFMPEG, args, true);
        return;
    }

    // if metadata is saved we first run ffmpeg and after exiftool
    runProcess(ProcessType::FFMPEG, args, false);
    copyMetadata(inputFilePath, outputFilePath, format.fileType);
}

void Converter::copyMetadata(const QString &inputFilePath, const QString &outputFilePath, FileType type)
{
    connect(&progressHandler_, &ProgressHandler::finished, this,
    [this, outputFilePath, inputFilePath, type]() {

        // images and audio metadata is moved with ExifTool
        if (type == FileType::IMAGE || type == FileType::AUDIO) {
            QStringList args = ExifTool::CopyMetadata::standardArgs(inputFilePath, outputFilePath);
            runProcess(ProcessType::EXIFTOOL, args, true);

        // video metadata
        } else if (type == FileType::VIDEO) {

        }
    }, Qt::SingleShotConnection);
}

void Converter::runMetadataRemover(const QString& inputFilePath, const QString& outputFilePath)
{
    if (state_ != State::IDLE) {
        return;
    }
    state_ = State::RUNNING;
    emit onLogMessage("\nStarting metadata remover...\n");

    // TEST IS THIS NESSESARY
    if (!copyAndReplaceFile(inputFilePath, outputFilePath)) { return; }

    FormatInfo format = getFileFormat(outputFilePath);
    QStringList args = Arguments::metadata(outputFilePath, format);

    // if args are valid we can run exiftool
    if (!args.empty()) {
        runProcess(ProcessType::EXIFTOOL, args);
        return;
    }

    // empty args can be unknown filetype OR filetypes not working with ExifTool
    switch (format.fileType) {
        case FileType::AUDIO:
            args = FFmpeg::RemoveMetadata::mp3Args(inputFilePath, outputFilePath);
            break;

        case FileType::VIDEO:
            // TEST MP4 FRAGMENTATION
            args = FFmpeg::RemoveMetadata::mkvArgs(inputFilePath, outputFilePath);
            break;

        case FileType::IMAGE:
        case FileType::UNKNOWN:
            emit onLogMessage("File type unknown!");
            state_ = State::IDLE;
            return;
    }
    runProcess(ProcessType::FFMPEG, args);
}

bool Converter::copyAndReplaceFile(const QString &inputFilePath, const QString &outputFilePath)
{
    // if outputfile exists it gets removed
    if (QFile::exists(outputFilePath)) {
        QFile::remove(outputFilePath);
    }

    // and we copy our input file to output
    if (!QFile::copy(inputFilePath, outputFilePath)) {
        emit onLogMessage("\nFailed to make copy of the original file!\n");
        return false;
    }

    return true;
}

void Converter::runProcess(ProcessType processType, const QStringList& args, bool lastConversion)
{
    switch (processType) {
        case ProcessType::FFMPEG:   state_ = State::FFMPEG_RUNNING;     break;
        case ProcessType::EXIFTOOL: state_ = State::EXIFTOOL_RUNNING;   break;
    }

    QString processName = processTypeToString(processType);
    progressHandler_.progressStarted(processName);

    QProcess* qProcess = new QProcess(this);
    connectProcesses(qProcess, processType, lastConversion);

    qProcess->start(processName.toLower(), args);
}

void Converter::connectProcesses(QProcess *process, ProcessType processType, bool lastConversion)
{
    QString processName = processTypeToString(processType);
    // connecting progress updates
    connect(process, &QProcess::readyReadStandardError, [process, this, processType]() {
        switch (processType) {
            case ProcessType::FFMPEG:
                progressHandler_.handleFfmpegProgress(process->readAllStandardError().trimmed());
                break;
            case ProcessType::EXIFTOOL:
                progressHandler_.handleExifToolProgress(process->readAllStandardError().trimmed());
                break;
        }
    });

    // emitting error signal
    connect(process, &QProcess::errorOccurred, [this, processName]() {
        progressHandler_.progressFailed(processName);
    });

    // emitting finished signal
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [this, processName, lastConversion] () {
        progressHandler_.progressFinished(processName, lastConversion);
    });
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

FormatInfo Converter::getFileFormat(const QString& filePath)
{
    QString suffix = QFileInfo(filePath).suffix();
    for (const FormatInfo& it : fileFormats) {
        if (it.label == suffix) {
            // always finds correct it as only supported suffixes are given previously
            return it;
        }
    }
    return {FileType::UNKNOWN};
}

void Converter::conversionEnded()
{
    state_ = State::IDLE;
}

