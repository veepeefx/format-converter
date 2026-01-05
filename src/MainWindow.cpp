#include "MainWindow.h"
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

    topLayout_ = new QGridLayout();

    initInputFileWidgets();
    initOutputFileWidgets();

    midLayout_ = new QHBoxLayout();

    initConvertSettings();
    initMetaDataRemoverSettings();

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addLayout(topLayout_);
    mainLayout->addSpacerItem(new QSpacerItem(20, 20));
    mainLayout->addLayout(midLayout_);

    enableLayoutWidgets(midLayout_, false);

    progressBar_ = new QProgressBar();
    progressBar_->setRange(0, 100);
    progressLabel_ = new QLabel();
    progressLabel_->setAlignment(Qt::AlignCenter);

    resetProgressBar();
    mainLayout->addWidget(progressBar_);
    mainLayout->addWidget(progressLabel_);

    connect(converter_, &Converter::progressChanged, this, &MainWindow::updateProgressBar);
    connect(converter_, &Converter::errorOccured, this, &MainWindow::handleError);

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

    topLayout_->addWidget(filePathLabel, mainLayoutRow_, 0);
    topLayout_->addWidget(inputFilePathLineEdit_, mainLayoutRow_, 1);
    topLayout_->addWidget(browseFilesButton,  mainLayoutRow_, 2);

    mainLayoutRow_++;
}

void MainWindow::initOutputFileWidgets()
{
    QLabel* folderPathLabel = new QLabel("Output folder: ");
    outputFolderLineEdit_ = new QLineEdit();
    QPushButton* browseFolderButton = new QPushButton("Browse");
    connect(browseFolderButton, &QPushButton::clicked, this,
            &MainWindow::browseFolderButtonClicked);

    topLayout_->addWidget(folderPathLabel, mainLayoutRow_, 0);
    topLayout_->addWidget(outputFolderLineEdit_, mainLayoutRow_, 1);
    topLayout_->addWidget(browseFolderButton, mainLayoutRow_, 2);

    mainLayoutRow_++;
}

void MainWindow::initConvertSettings()
{
    QGridLayout* convertLayout = new QGridLayout();
    convertLayout->setAlignment(Qt::AlignTop);
    int row = 0;

    QLabel* convertLabel = new QLabel("Convert settings:");
    convertLabel->setAlignment(Qt::AlignCenter);
    convertLabel->setStyleSheet("font-size: 12pt; font-weight: bold;");
    convertLayout->addWidget(convertLabel, row, 0, 1, 2);

    row++;

    QLabel* fileTypeLabel = new QLabel("Select new extension format: ");
    convertFileTypeBox_ = new QComboBox();
    connect(convertFileTypeBox_, &QComboBox::currentIndexChanged, this, &MainWindow::convertFileTypeChanged);

    convertLayout->addWidget(fileTypeLabel, row, 0);
    convertLayout->addWidget(convertFileTypeBox_, row, 1);

    row++;

    convertButton_ = new QPushButton("Convert");
    connect(convertButton_, &QPushButton::clicked, this, &MainWindow::convertButtonClicked);
    convertLayout->addWidget(convertButton_, row, 0, 1, 2);

    midLayout_->addLayout(convertLayout);
}

void MainWindow::initMetaDataRemoverSettings()
{
    QGridLayout* metaDataLayout = new QGridLayout();
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

    midLayout_->addLayout(metaDataLayout);
}


void MainWindow::updateFileTypeBox()
{
    convertFileTypeBox_->clear();

    if (inputFileFormat_.fileType == FileType::UNKNOWN) {
        enableLayoutWidgets(midLayout_, false);
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

    enableLayoutWidgets(midLayout_, true);
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

void MainWindow::resetProgressBar()
{
    progressLabel_->setText("Waiting for new process");
    progressBar_->setValue(0);
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
    resetProgressBar();
}

void MainWindow::browseFileButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Open File"), QDir::homePath());

    if (!filePath.isEmpty()) {
        inputFilePathLineEdit_->setText(filePath);
        inputFilePathEditingFinished();
    }

    resetProgressBar();
}

void MainWindow::browseFolderButtonClicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(
        this, tr("Select Folder"), outputFolderLineEdit_->text());

    if (!folderPath.isEmpty()) {
        outputFolderLineEdit_->setText(folderPath);
    }

    resetProgressBar();
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

    converter_->runConverter(inputFilePath, outputFilePath);
}

void MainWindow::removeButtonClicked()
{
    // ADD OUTPUT PATH
    QString filePath = inputFilePathLineEdit_->text();

    QMessageBox::StandardButton replace;
    replace = QMessageBox::question(nullptr, "Overwrites old file",
        "THIS OPERATION OVERWRITES OLD FILE WITH FILE THAT DOESN'T CONTAIN METADATA. DO YOU WANT TO PROCEED?",
        QMessageBox::Yes | QMessageBox::No);

    if (replace == QMessageBox::No) {
        return;
    }

    converter_->runMetaDataRemover(filePath, filePath);
}

void MainWindow::convertFileTypeChanged()
{
    resetProgressBar();
}

void MainWindow::updateProgressBar(int progress, bool isFinished)
{
    // progress can be 100 but ffmpeg still processing so adding state if isFinished
    if (isFinished) {
        progressLabel_->setText("Done!");
    } else if (progress == 0) {
        progressLabel_->setText("Starting...");
    } else {
        progressLabel_->setText("Processing...");
    }

    progressBar_->setValue(progress);
}

void MainWindow::handleError(const QString &message)
{
    progressLabel_->setText("ERROR! " + message);
}