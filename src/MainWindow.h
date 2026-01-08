#ifndef FORMAT_CONVERTER_MAINWINDOW_H
#define FORMAT_CONVERTER_MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QGridLayout>
#include <QComboBox>
#include <QCheckBox>

#include "utils/CommonEnums.h"
#include "Converter.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    explicit MainWindow(Converter* converter, QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    Converter* converter_;

    QVBoxLayout* mainLayout_;

    QLineEdit* iFilePathLE_ = nullptr;
    QLineEdit* oFolderPathLE_ = nullptr;
    QLineEdit* oFileNameLE_ = nullptr;
    QComboBox* oFileTypeCB_ = nullptr;
    QCheckBox* metadataCheckBox_ = nullptr;

    // all widgets which cannot be enabled due restrictions
    // for example exiftool isn't installed
    QSet<QWidget*> widgetNotInUse_;

    // init functions
    void initFilePathWidgets();
    void initInput(QGridLayout& layout, int& rowIndex);
    void initOutput(QGridLayout& layout, int& rowIndex);

    void initConvertToolWidgets();
    void initConvertSettings(QHBoxLayout& layout);
    void initMetaDataRemoverSettings(QHBoxLayout& layout);

    void initProgressIndicator();

    // update functions
    void updateFileTypeBox(const FormatInfo& inputFileFormat);

    // helper functions
    void enableLayoutWidgets(QLayout* layout, bool enable);

private slots:
    void browseFileButtonClicked();
    void inputFilePathEditingFinished();
    void browseFolderButtonClicked();
    void convertButtonClicked();
    void removeButtonClicked();
    void convertFileTypeChanged();

signals:
    void resetProgress();
    void enableConversionSettings(bool enable);

};


#endif //FORMAT_CONVERTER_MAINWINDOW_H