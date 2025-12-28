#include "MainWindow.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    mainLayout_ = new QGridLayout(central);
    mainLayout_->setAlignment(Qt::AlignCenter);

    initInputFileWidgets();
    initOutputFileWidgets();

    this->setWindowTitle("Format Converter");
    this->resize(800, 600);
}

MainWindow::~MainWindow() {}

void MainWindow::initInputFileWidgets()
{
    QLabel* filePathLabel = new QLabel("Selected file: ");
    inputFilePathLineEdit_ = new QLineEdit();
    QPushButton* browseFilesButton = new QPushButton("Browse");
    connect(browseFilesButton, &QPushButton::clicked, this, &MainWindow::browseFileButtonClicked);

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
    connect(browseFolderButton, &QPushButton::clicked, this, &MainWindow::browseFolderButtonClicked);

    mainLayout_->addWidget(folderPathLabel, mainLayoutRow_, 0);
    mainLayout_->addWidget(outputFolderLineEdit_, mainLayoutRow_, 1);
    mainLayout_->addWidget(browseFolderButton, mainLayoutRow_, 2);

    mainLayoutRow_++;
}

void MainWindow::browseFileButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Open File"), QDir::homePath());

    QString folderPath = filePath.left(filePath.lastIndexOf('/') + 1);

    inputFilePathLineEdit_->setText(filePath);
    outputFolderLineEdit_->setText(folderPath);
}

void MainWindow::browseFolderButtonClicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(
        this, tr("Select folder"), outputFolderLineEdit_->text());

    if (!folderPath.isEmpty()) {
        outputFolderLineEdit_->setText(folderPath);
    }
}

