#include "MainWindow.h"
#include "CommonEnums.h"

#include <QFileDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    mainLayout_ = new QGridLayout(central);
    mainLayout_->setAlignment(Qt::AlignCenter);

    initInputFileWidgets();
    initOutputFileWidgets();
    initConvertSettings();

    this->setWindowTitle("Format Converter");
    this->resize(800, 600);
}

MainWindow::~MainWindow() {}

void MainWindow::initInputFileWidgets()
{
    QLabel* filePathLabel = new QLabel("Selected file: ");
    inputFilePathLineEdit_ = new QLineEdit();
    connect(inputFilePathLineEdit_, &QLineEdit::editingFinished, this,
            &MainWindow::inputFilePathEditingFinished);

    QPushButton* browseFilesButton = new QPushButton("Browse");
    connect(browseFilesButton, &QPushButton::clicked, this,
            &MainWindow::browseFileButtonClicked);

    mainLayout_->addWidget(filePathLabel, mainLayoutRow_, 0);
    mainLayout_->addWidget(inputFilePathLineEdit_, mainLayoutRow_, 1);
    mainLayout_->addWidget(browseFilesButton,  mainLayoutRow_, 2);

    mainLayoutRow_++;
}

void MainWindow::initOutputFileWidgets()
{
    QLabel* folderPathLabel = new QLabel("Output folder: ");
    outputFolderLineEdit_ = new QLineEdit();
    QPushButton* browseFolderButton = new QPushButton("Browse");
    connect(browseFolderButton, &QPushButton::clicked, this,
            &MainWindow::browseFolderButtonClicked);

    mainLayout_->addWidget(folderPathLabel, mainLayoutRow_, 0);
    mainLayout_->addWidget(outputFolderLineEdit_, mainLayoutRow_, 1);
    mainLayout_->addWidget(browseFolderButton, mainLayoutRow_, 2);

    mainLayoutRow_++;
}

void MainWindow::initConvertSettings()
{
    QLabel* fileTypeLabel = new QLabel("Select new extension format: ");
    convertFileTypeBox_ = new QComboBox();
    convertFileTypeBox_->setEnabled(false);

    connect(convertFileTypeBox_, &QComboBox::currentIndexChanged, this,
            &MainWindow::checkNewConvertIndex);

    mainLayout_->addWidget(fileTypeLabel, mainLayoutRow_, 0);
    mainLayout_->addWidget(convertFileTypeBox_, mainLayoutRow_, 1);

    mainLayoutRow_++;

    QPushButton* convertButton = new QPushButton("Convert");
    //connect(convertButton, &QPushButton::clicked, this, );
    mainLayout_->addWidget(convertButton, mainLayoutRow_, 0);
}

QString MainWindow::getFilesFolderPath(QString filePath)
{
    return filePath.left(filePath.lastIndexOf('/') + 1);
}

void MainWindow::updateFileTypeBox()
{
    int index = 0;
    convertFileTypeBox_->clear();
    convertFileTypeBox_->setEnabled(true);

    switch (inputFileType_) {
        case FileType::AUDIO:
            for (auto it = audioFormatLabels.begin(); it != audioFormatLabels.end(); ++it) {
                if (!labelsBlackList.contains(it.key())) {
                    convertFileTypeBox_->insertItem(index++, it.key());
                }
            }
            break;
        case FileType::VIDEO:
            for (auto it = videoFormatLabels.begin(); it != videoFormatLabels.end(); ++it) {
                if (!labelsBlackList.contains(it.key())) {
                    convertFileTypeBox_->insertItem(index++, it.key());
                }
            }
            break;
        case FileType::IMAGE:
            for (auto it = imageFormatLabels.begin(); it != imageFormatLabels.end(); ++it) {
                if (!labelsBlackList.contains(it.key())) {
                    convertFileTypeBox_->insertItem(index++, it.key());
                }
            }
            break;
        case FileType::UNKNOWN:
            convertFileTypeBox_->setEnabled(false);
            break;
    }
}

void MainWindow::inputFilePathEditingFinished()
{
    QString filePath = inputFilePathLineEdit_->text();
    outputFolderLineEdit_->setText(getFilesFolderPath(filePath));

    QString extension = filePath.mid(filePath.lastIndexOf('.') + 1);

    if (audioFormatLabels.contains(extension))          { inputFileType_ = FileType::AUDIO; }
    else if (videoFormatLabels.contains(extension))     { inputFileType_ = FileType::VIDEO; }
    else if (imageFormatLabels.contains(extension))     { inputFileType_ = FileType::IMAGE; }
    else                                                { inputFileType_ = FileType::UNKNOWN; }

    updateFileTypeBox();
}

void MainWindow::browseFileButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Open File"), QDir::homePath());

    if (!filePath.isEmpty()) {
        inputFilePathLineEdit_->setText(filePath);
        inputFilePathEditingFinished();
    }
}

void MainWindow::browseFolderButtonClicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(
        this, tr("Select Folder"), outputFolderLineEdit_->text());

    if (!folderPath.isEmpty()) {
        outputFolderLineEdit_->setText(folderPath);
    }
}

void MainWindow::checkNewConvertIndex()
{
}

