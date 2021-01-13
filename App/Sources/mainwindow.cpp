#include "App/Headers/mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    _firmwaresend = new FirmwareSend();
    ui->setupUi(this);

    connect(_firmwaresend, SIGNAL(file_length(uint16_t)), this, SLOT(setFileLength(uint16_t)));
    connect(_firmwaresend, SIGNAL(file_counter(uint16_t)), this, SLOT(setProgressBar(uint16_t)));
    connect(_firmwaresend, SIGNAL(CAN_writeStatus(bool)), this, SLOT(messageBox(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_browse_clicked()
{
    filename= QFileDialog::getOpenFileName(this, tr("Open File"),"C://","Binary files(*.bin)");
    ui->lineEdit->insert(filename);
}

void MainWindow::on_pushButton_send_clicked()
{
    qDebug() << "File location:" << filename;
    _firmwaresend->sendFile(filename);
}

void MainWindow::setFileLength(uint16_t file_length){
    fLength=file_length;
    qDebug() <<"File length:"<<fLength;
}

void MainWindow::setProgressBar(uint16_t i){
    uint8_t bar_value=(100*i)/fLength;
    ui->progressBar->setValue(bar_value);
}

void MainWindow::messageBox(bool status)
{
    if(!status)
    {
        QMessageBox::warning(this,
                              tr("Firmware Upload Status"),
                              tr("Failes writing in flash. Application stoped!"),
                              QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this,
                                 tr("Firmware Upload Status"),
                                 tr("New firmware available on controller!"),
                                 QMessageBox:: Ok);
    }
}

void MainWindow::on_pushButton_quit_clicked()
{
    close();
}

void MainWindow::on_pushButton_clicked()
{
    close();

    mWindow = new MainWindow();
    mWindow->show();
}

void MainWindow::on_pushButton_2_clicked()
{
    QString firmwareAddress;

    firmwareAddress = ui->lineEdit_2->text();

    _firmwaresend->setFirmwareAddress(firmwareAddress);
}
