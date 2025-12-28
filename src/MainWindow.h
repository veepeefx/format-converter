#ifndef FORMAT_CONVERTER_MAINWINDOW_H
#define FORMAT_CONVERTER_MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QVBoxLayout>

#include "CommonEnums.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    int mainLayoutRow_ = 0;
    QGridLayout *mainLayout_;

    QLineEdit* inputFilePathLineEdit_;
    QLineEdit* outputFolderLineEdit_;

    FileType inputFileType_ = FileType::UNKNOWN;

    void initInputFileWidgets();
    void initOutputFileWidgets();
    void initConvertSettings();

    QString getFilesFolderPath(QString filePath);
    void updateConvertSettings();

private slots:
    void browseFileButtonClicked();
    void inputFilePathEditingFinished();
    void browseFolderButtonClicked();

};


#endif //FORMAT_CONVERTER_MAINWINDOW_H