#ifndef FORMAT_CONVERTER_MAINWINDOW_H
#define FORMAT_CONVERTER_MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QVBoxLayout>

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

    void initInputFileWidgets();
    void initOutputFileWidgets();

private slots:
    void browseFileButtonClicked();
    void browseFolderButtonClicked();

};


#endif //FORMAT_CONVERTER_MAINWINDOW_H