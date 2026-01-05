#ifndef FORMAT_CONVERTER_MAINWINDOW_H
#define FORMAT_CONVERTER_MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QGridLayout>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>

#include "CommonEnums.h"
#include "Converter.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    explicit MainWindow(Converter* converter, QWidget *parent = nullptr);

    ~MainWindow() override;

private:

    Converter* converter_;

    int mainLayoutRow_ = 0;
    QGridLayout *topLayout_;
    QHBoxLayout* midLayout_;

    QLineEdit* inputFilePathLineEdit_ = nullptr;
    QLineEdit* outputFolderLineEdit_ = nullptr;
    QComboBox* convertFileTypeBox_ = nullptr;
    QPushButton* convertButton_ = nullptr;
    QProgressBar* progressBar_ = nullptr;
    QLabel* progressLabel_ = nullptr;

    FormatInfo inputFileFormat_;

    void initInputFileWidgets();
    void initOutputFileWidgets();
    void initConvertSettings();
    void initMetaDataRemoverSettings();
    void updateFileTypeBox();

    // changes all child widgets enabled recursively to chosen boolean
    static void enableLayoutWidgets(QLayout* layout, bool enable);

    void resetProgressBar();

private slots:
    void browseFileButtonClicked();
    void inputFilePathEditingFinished();
    void browseFolderButtonClicked();
    void convertButtonClicked();
    void removeButtonClicked();
    void convertFileTypeChanged();

    void updateProgressBar(int progress, bool isFinished);
    void handleError(const QString& message);
};


#endif //FORMAT_CONVERTER_MAINWINDOW_H