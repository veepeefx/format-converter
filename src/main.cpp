#include <QApplication>
#include <QMessageBox>
#include "MainWindow.h"


bool isFFmpegAvailable()
{
#ifdef _WIN32
    const char* cmd = "ffmpeg -version >nul 2>&1";
#else
    const char* cmd = "ffmpeg -version >/dev/null 2>&1";
#endif

    return std::system(cmd) == 0;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!isFFmpegAvailable()) {
        QMessageBox::warning(nullptr, "FFmpeg Not Found",
                             "FFmpeg is not installed or not found in your system PATH.\n"
                             "Please install FFmpeg to use this tool.\n"
                             "You can download it from: https://ffmpeg.org/download.html");
        return 1;
    }

    MainWindow w;
    w.show();

    return QApplication::exec();
}