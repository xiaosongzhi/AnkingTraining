#include "mainwindow.h"
#include <QApplication>
#include "cdatabaseinterface.h"
#include <QDebug>
#include "userdataformate.h"
#include <QFile>
#include <QMessageBox>
#include "readconfig.h"
#include "login.h"
#include "picturetipswidget.h"
#include <QSharedMemory>
int main(int argc, char *argv[])
{
//    qputenv("QT_IM_MODULE",QByteArray("Qt5Input"));

    QApplication a(argc, argv);

    //保证程序只打开一次
    QSharedMemory sharedMemory;
    sharedMemory.setKey(QString("main_window"));
    if(sharedMemory.attach())
    {
        return 0;
    }
    if(sharedMemory.create(1))
    {
        //读取配置文件
        bool ok =ReadConfig::getInstance()->readConfigFile();
        if(!ok)
        {
            qDebug()<<"read config file failed";
            return -1;
        }
        //此处的newConnect.db就是创建的数据库文件,可直接使用第三方工具访问
        if(!CDatabaseInterface::getInstance()->openDB("database123.db","QSQLITE"))
        {
            qDebug()<<"open sqlite failed";
        }

        Login loginDialog;
        int result = loginDialog.exec();
        qDebug()<<"result"<<result;

        MainWindow w;
        QFile file("./QSS/buttonStyle.txt");
        if(file.open(QFile::ReadOnly))
        {
            QString styleSheet = QLatin1String(file.readAll());
            w.setStyleSheet(styleSheet);
            file.close();
        }
        else
        {
            QMessageBox::warning(NULL, "warning", "Open failed", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        }
        w.show();

        //患者信息表
        return a.exec();
    }
    else
    {
        QMessageBox::warning(NULL, "warning", "app is existed", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return 0;
    }

}
