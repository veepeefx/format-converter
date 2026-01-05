#include "MainWindow.h"

#include <iostream>

#include "CommonEnums.h"
#include "Converter.h"

#include <QFileDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>


MainWindow::MainWindow(Converter* converter, QWidget *parent) : converter_(converter), QMainWindow(parent)
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    mainLayout_ = new QVBoxLayout(central);
    initFilePathWidgets();
    initConvertToolWidgets();
    initProgressIndicator();

    mainLayout_->setAlignment(Qt::AlignCenter);
    this->setWindowTitle("Format Converter");
    this->resize(800, 600);
}

MainWindow::~MainWindow() {}

void MainWindow::initFilePathWidgets()
{
    QGridLayout* layout = new QGridLayout();

    int row = 0;
    initInput(*layout, row);
    initOutput(*layout, row);

    mainLayout_->addLayout(layout);
    mainLayout_->addSpacerItem(new QSpacerItem(20, 20));
}

void MainWindow::initInput(QGridLayout& layout, int& rowIndex)
{
    QLabel* filePathLabel = new QLabel("Selected file: ");
    iFilePathLE_ = new QLineEdit();
    connect(iFilePathLE_, &QLineEdit::editingFinished, this,
            &MainWindow::inputFilePathEditingFinished);

    QPushButton* browseFilesButton = new QPushButton("Browse");
    connect(browseFilesButton, &QPushButton::clicked, this,
            &MainWindow::browseFileButtonClicked);

    layout.addWidget(filePathLabel, rowIndex, 0);
    layout.addWidget(iFilePathLE_, rowIndex, 1);
    layout.addWidget(browseFilesButton,  rowIndex, 2);

    rowIndex++;
}

void MainWindow::initOutput(QGridLayout& layout, int& rowIndex)
{
    QLabel* folderPathLabel = new QLabel("Output folder: ");
    oFolderPathLE_ = new QLineEdit();
    QPushButton* browseFolderButton = new QPushButton("Browse");
    connect(browseFolderButton, &QPushButton::clicked, this,
            &MainWindow::browseFolderButtonClicked);

    layout.addWidget(folderPathLabel, rowIndex, 0);
    layout.addWidget(oFolderPathLE_, rowIndex, 1);
    layout.addWidget(browseFolderButton, rowIndex, 2);

    rowIndex++;

    QLabel* fileName = new QLabel("Output file name: ");
    oFileNameLE_ = new QLineEdit();

    layout.addWidget(fileName, rowIndex, 0);
    layout.addWidget(oFileNameLE_, rowIndex, 1);
}

void MainWindow::initConvertToolWidgets()
{
    QHBoxLayout* layout = new QHBoxLayout();
    initConvertSettings(*layout);
    initMetaDataRemoverSettings(*layout);

    mainLayout_->addLayout(layout);
    mainLayout_->addSpacerItem(new QSpacerItem(20, 20));

    connect(this, &MainWindow::enableConversionSettings, this, [layout](bool enable) {
        enableLayoutWidgets(layout, enable);
    });

    emit enableConversionSettings(false);
}

void MainWindow::initConvertSettings(QHBoxLayout& layout)
{
    QGridLayout* convertLayout = new QGridLayout();
    convertLayout->setContentsMargins(10, 0, 10, 0);
    convertLayout->setAlignment(Qt::AlignTop);
    int row = 0;

    QLabel* convertLabel = new QLabel("Convert settings:");
    convertLabel->setAlignment(Qt::AlignCenter);
    convertLabel->setStyleSheet("font-size: 12pt; font-weight: bold;");
    convertLayout->addWidget(convertLabel, row, 0, 1, 2);

    row++;

    QLabel* fileTypeLabel = new QLabel("Select new extension format: ");
    oFileTypeCB_ = new QComboBox();
    connect(oFileTypeCB_, &QComboBox::currentIndexChanged, this, &MainWindow::convertFileTypeChanged);

    convertLayout->addWidget(fileTypeLabel, row, 0);
    convertLayout->addWidget(oFileTypeCB_, row, 1);

    row++;

    QPushButton* convertButton = new QPushButton("Convert");
    connect(convertButton, &QPushButton::clicked, this, &MainWindow::convertButtonClicked);
    convertLayout->addWidget(convertButton, row, 0, 1, 2);

    layout.addLayout(convertLayout);
}

void MainWindow::initMetaDataRemoverSettings(QHBoxLayout& layout)
{
    QGridLayout* metaDataLayout = new QGridLayout();
    metaDataLayout->setContentsMargins(10, 0, 10, 0);
    metaDataLayout->setAlignment(Qt::AlignTop);
    int row = 0;

    QLabel* metaDataLabel = new QLabel("Metadata remover settings:");
    metaDataLabel->setAlignment(Qt::AlignCenter);
    metaDataLabel->setStyleSheet("font-size: 12pt; font-weight: bold;");
    metaDataLayout->addWidget(metaDataLabel, row, 0, 1, 2);

    row++;

    QPushButton* removeButton = new QPushButton("Remove");
    metaDataLayout->addWidget(removeButton, row, 0, 1, 2);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeButtonClicked);

    layout.addLayout(metaDataLayout);
}

void MainWindow::initProgressIndicator()
{
    QProgressBar* progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    QLabel* progressLabel = new QLabel();
    progressLabel->setAlignment(Qt::AlignCenter);

    mainLayout_->addWidget(progressBar);
    mainLayout_->addWidget(progressLabel);

    // updating progressbar and label
    connect(converter_, &Converter::progressChanged, this, [progressBar, progressLabel]
        (int progress, bool isFinished) {
        if (isFinished)             { progressLabel->setText("Done!"); }
        else if (progress == 0)     { progressLabel->setText("Starting..."); }
        else                        { progressLabel->setText("Processing..."); }
        progressBar->setValue(progress);
    });

    // showing error
    connect(converter_, &Converter::errorOccured, this, [progressLabel](QString message) {
        progressLabel->setText("ERROR! " + message);
    });

    // resetting progress
    connect(this, &MainWindow::resetProgress, this, [progressBar, progressLabel]() {
        progressLabel->setText("Waiting for new process");
        progressBar->setValue(0);
    });

    // setting starting values
    emit resetProgress();
}

void MainWindow::updateFileTypeBox(const FormatInfo& inputFileFormat)
{
    oFileTypeCB_->clear();

    if (inputFileFormat.fileType == FileType::UNKNOWN) {
        QMessageBox::warning(this, "File type isn't supported",
            "Selected file type '." + QFileInfo(iFilePathLE_->text()).suffix() + "' is not supported.");
        emit enableConversionSettings(false);
        return;
    }

    int index = 0;
    for (const auto& it : fileFormats) {
        if (it.fileType == inputFileFormat.fileType
            && it.enumValue != inputFileFormat.enumValue
            && !labelsBlackList.contains(it.label)) {
            oFileTypeCB_->insertItem(index++, it.label);
        }
    }
    emit enableConversionSettings(true);
}

void MainWindow::enableLayoutWidgets(QLayout *layout, bool enable)
{
    for (int i = 0; i < layout->count(); i++) {

        QLayoutItem* item = layout->itemAt(i);

        if (QWidget* widget = item->widget()) {
            // all but Qlabels
            if (!qobject_cast<QLabel*>(widget)) {
                widget->setEnabled(enable);
            }
        }

        // check for childLayouts
        if (QLayout* childLayout = item->layout()) {
            enableLayoutWidgets(childLayout, enable);
        }
    }
}

void MainWindow::inputFilePathEditingFinished()
{
    QString filePath = iFilePathLE_->text();
    oFolderPathLE_->setText(QFileInfo(filePath).path());
    oFileNameLE_->setText(QFileInfo(filePath).completeBaseName());

    QString suffix = QFileInfo(filePath).suffix();

    FormatInfo fileFormatInfo = {FileType::UNKNOWN};
    for (const auto& it : fileFormats) {
        if (it.label == suffix) {
            fileFormatInfo = std::move(it);
            break;
        }
    }
    updateFileTypeBox(fileFormatInfo);
    emit resetProgress();
}

void MainWindow::browseFileButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Open File"), QDir::homePath());

    if (!filePath.isEmpty()) {
        iFilePathLE_->setText(filePath);
        inputFilePathEditingFinished();
    }

    emit resetProgress();
}

void MainWindow::browseFolderButtonClicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(
        this, tr("Select Folder"), oFolderPathLE_->text());

    if (!folderPath.isEmpty()) {
        oFolderPathLE_->setText(folderPath);
    }
    emit resetProgress();
}

void MainWindow::convertButtonClicked()
{
    QString oPath = oFolderPathLE_->text();
    QString oName = oFileNameLE_->text();
    QString oSuffix = oFileTypeCB_->currentText();
    QString outputFilePath = QFileInfo(oPath + oName + "." + oSuffix).absoluteFilePath();

    if (QFileInfo(outputFilePath).exists()) {
        QMessageBox::StandardButton overwrite = QMessageBox::question(nullptr, "File Exists",
            "The file already exists. Overwrite?", QMessageBox::Yes | QMessageBox::No);
        if (overwrite == QMessageBox::No) {
            return;
        }
    }
    converter_->runConverter(iFilePathLE_->text(), outputFilePath);
}

void MainWindow::removeButtonClicked()
{
    QString oPath = oFolderPathLE_->text();
    QString oName = oFileNameLE_->text();
    QString oSuffix = QFileInfo(iFilePathLE_->text()).suffix();
    QString outputFilePath = QFileInfo(oPath + oName + "." + oSuffix).absoluteFilePath();

    if (QFileInfo(outputFilePath).exists()) {
        QMessageBox::StandardButton overwrite = QMessageBox::question(nullptr, "File Exists",
            "The file already exists. Overwrite?", QMessageBox::Yes | QMessageBox::No);
        if (overwrite == QMessageBox::No) {
            return;
        }
    }

    converter_->runMetaDataRemover(iFilePathLE_->text(), outputFilePath);
}

void MainWindow::convertFileTypeChanged()
{
    emit resetProgress();
}