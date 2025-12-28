#ifndef FORMAT_CONVERTER_MAINWINDOW_H
#define FORMAT_CONVERTER_MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QGridLayout>
#include <QComboBox>

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
    QComboBox* convertFileTypeBox_;
    QLineEdit* resultFilePath_;

    FileType inputFileType_ = FileType::UNKNOWN;

    void initInputFileWidgets();
    void initOutputFileWidgets();
    void initConvertSettings();

    QString getFilesFolderPath(QString filePath);
    void updateFileTypeBox();

private slots:
    void browseFileButtonClicked();
    void inputFilePathEditingFinished();
    void browseFolderButtonClicked();
    void checkNewConvertIndex();

};


#endif //FORMAT_CONVERTER_MAINWINDOW_H