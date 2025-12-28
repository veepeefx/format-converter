#include "MainWindow.h"
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget* central = new QWidget(this);
    mainLayout_ = new QVBoxLayout(central);

    this->setWindowTitle("Format Converter");
    this->resize(800, 600);
}

MainWindow::~MainWindow() {}