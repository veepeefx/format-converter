#include <QApplication>
#include <QMessageBox>

#include "Converter.h"
#include "utils/DependencyChecker.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // dont start program if ffmpeg isn't installed
    if (!DependencyChecker::isFFmpegAvailable()) {
        QMessageBox::warning(nullptr, "FFmpeg Not Found",
                             "FFmpeg is not installed or not found in your system PATH.\n"
                             "Please install FFmpeg to use this tool.\n"
                             "You can download it from: https://ffmpeg.org/download.html");
        return 1;
    }

    // notify user missing optional dependency ExifTool
    if (!DependencyChecker::isExifToolAvailable()) {
        QMessageBox::warning(nullptr, "ExifTool Not Found",
                     "ExifTool is not installed or not found in your system PATH.\n"
                     "Without ExifTool conversions can't save metadata.\n"
                     "You can download it from: https://exiftool.org/install.html");
    }

    Converter c;

    MainWindow w(&c);
    w.show();

    return QApplication::exec();
}