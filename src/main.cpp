#include <QApplication>
#include <QMessageBox>

#include "Converter.h"
#include "MainWindow.h"


bool isFFmpegAvailable()
{
#ifdef _WIN32
    return system("ffmpeg -version >nul 2>&1") == 0;
#else
    return system("ffmpeg -version >/dev/null 2>&1") == 0;
#endif
}

bool isExifToolAvailable()
{
    #ifdef _WIN32
        return system("exiftool -ver >null 2>&1") == 0;
    #else
        return system("exiftool -ver >/dev/null 2>&1") == 0;
    #endif
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // dont start program if ffmpeg isn't installed
    if (!isFFmpegAvailable()) {
        QMessageBox::warning(nullptr, "FFmpeg Not Found",
                             "FFmpeg is not installed or not found in your system PATH.\n"
                             "Please install FFmpeg to use this tool.\n"
                             "You can download it from: https://ffmpeg.org/download.html");
        return 1;
    }

    // notify user that without exiftool installed metadata isn't transferred to the new file
    bool exifToolInstalled = isExifToolAvailable();
    if (!exifToolInstalled) {
        QMessageBox::warning(nullptr, "ExifTool Not Found",
                     "ExifTool is not installed or not found in your system PATH.\n"
                     "Without ExifTool conversions can't save metadata.\n"
                     "You can download it from: https://exiftool.org/install.html");
    }

    Converter c;

    MainWindow w(&c, exifToolInstalled);
    w.show();

    return QApplication::exec();
}