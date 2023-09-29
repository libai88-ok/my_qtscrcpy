#include "dialog.h"
#include "./ui_dialog.h"
#include "./adb/adbprocess.h"

#include <QDir>
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_testBtn_clicked()
{
    //QObject *parent;
    QStringList arguments;
    //arguments << "devices";

    arguments << "shell";
    arguments << "ip";
    arguments << "-f";
    arguments << "inet";
    arguments << "addr";
    arguments << "show";
    arguments << "wlan0";
    AdbProcess *myProcess = new AdbProcess(this);
    connect(myProcess, &AdbProcess::adbProcessResult, this, [this, myProcess](AdbProcess::ADB_PROCESS_RESULT processResult){
        qDebug() << ">>>>>>>>" << processResult;
        if (processResult == AdbProcess::AER_SUCCESS_EXEC) {
            qDebug() << myProcess->getDeviceIpFromStdOut();
        }
    });
    //QString program = myProcess->getAdbPath();
    //qDebug() << QDir::currentPath();;

    //myProcess->removePath("", "/sdcard/Download/test.txt");
    //myProcess->reverseRemove("", "scrcpy");
    myProcess->execute("", arguments);
}

