#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include "firmwaresend.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    FirmwareSend *_firmwaresend;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString filename;
    uint16_t fLength;
signals:
    void send_file();
    void fileLocation_Sent(QString fLocation);
private slots:
    void on_pushButton_browse_clicked();
    void setProgressBar(uint16_t i);
    void setFileLength(uint16_t file_length);
    void messageBox(bool status);
    void on_pushButton_send_clicked();
    void on_pushButton_quit_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    MainWindow *mWindow;
    QMessageBox msgBox;
public:
};

#endif // MAINWINDOW_H
