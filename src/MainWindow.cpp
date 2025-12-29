#include "MainWindow.h"
#include "CommonEnums.h"
#include "Converter.h"

#include <QFileDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>


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

    mainLayout_->addWidget(fileTypeLabel, mainLayoutRow_, 0);
    mainLayout_->addWidget(convertFileTypeBox_, mainLayoutRow_, 1);

    mainLayoutRow_++;

    convertButton_ = new QPushButton("Convert");
    convertButton_->setEnabled(false);
    connect(convertButton_, &QPushButton::clicked, this, &MainWindow::convertButtonClicked);
    mainLayout_->addWidget(convertButton_, mainLayoutRow_, 0);
}

void MainWindow::updateFileTypeBox()
{
    convertFileTypeBox_->clear();

    if (inputFileFormat_.fileType == FileType::UNKNOWN) {
        convertFileTypeBox_->setEnabled(false);
        convertButton_->setEnabled(false);
        return;
    }

    int index = 0;
    for (const auto& it : fileFormats) {
        if (it.fileType == inputFileFormat_.fileType
            && it.enumValue != inputFileFormat_.enumValue
            && !labelsBlackList.contains(it.label)) {
            convertFileTypeBox_->insertItem(index++, it.label);
        }
    }

    convertFileTypeBox_->setEnabled(true);
    convertButton_->setEnabled(true);
}

void MainWindow::inputFilePathEditingFinished()
{
    QString filePath = inputFilePathLineEdit_->text();
    outputFolderLineEdit_->setText(QFileInfo(filePath).path());

    QString extension = QFileInfo(filePath).suffix();

    inputFileFormat_.fileType = FileType::UNKNOWN;
    for (const auto& it : fileFormats) {
        if (it.label == extension) {
            inputFileFormat_ = it;
            break;
        }
    }
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

void MainWindow::convertButtonClicked()
{
    QString inputFilePath = inputFilePathLineEdit_->text();
    QString outputFolderPath = outputFolderLineEdit_->text();
    QString fileName = QFileInfo(inputFilePath).completeBaseName();
    QString extension = convertFileTypeBox_->currentText().toLower();
    QString outputFilePath = QFileInfo(outputFolderPath, fileName + "." + extension).absoluteFilePath();

    if (QFileInfo(outputFilePath).exists()) {
        QMessageBox::StandardButton overwrite;
        overwrite = QMessageBox::question(nullptr, "File Exists",
            "The file already exists. Overwrite?", QMessageBox::Yes | QMessageBox::No);
        if (overwrite == QMessageBox::No) {
            return;
        }
    }

    Converter::runConverter(inputFilePath, outputFilePath);
}
