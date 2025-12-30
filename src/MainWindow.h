#ifndef FORMAT_CONVERTER_MAINWINDOW_H
#define FORMAT_CONVERTER_MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QGridLayout>
#include <QComboBox>
#include <QPushButton>

#include "CommonEnums.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    int mainLayoutRow_ = 0;
    QGridLayout *topLayout_;
    QHBoxLayout* midLayout_;

    QLineEdit* inputFilePathLineEdit_;
    QLineEdit* outputFolderLineEdit_;
    QComboBox* convertFileTypeBox_;
    QPushButton* convertButton_;

    FormatInfo inputFileFormat_;

    void initInputFileWidgets();
    void initOutputFileWidgets();
    void initConvertSettings();
    void initMetaDataRemoverSettings();
    void updateFileTypeBox();

    // changes all child widgets enabled recursively to chosen boolean
    void enableLayoutWidgets(QLayout* layout, bool enable);

private slots:
    void browseFileButtonClicked();
    void inputFilePathEditingFinished();
    void browseFolderButtonClicked();
    void convertButtonClicked();
    void removeButtonClicked();

};


#endif //FORMAT_CONVERTER_MAINWINDOW_H