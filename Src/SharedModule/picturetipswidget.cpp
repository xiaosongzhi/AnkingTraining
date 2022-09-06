#include "picturetipswidget.h"
#include "ui_picturetipswidget.h"
#include <QPainter>
#include <QTimer>
#include <QDir>
#include <QFileInfoList>
#include <QDebug>
PictureTipsWidget::PictureTipsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PictureTipsWidget),
    step1Index(0),
    step2Index(0)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground,true);//设置无边框

    playerTimer = new QTimer();
    playerTimer->setInterval(300);
    connect(playerTimer,SIGNAL(timeout()),this,SLOT(slotPlayerTimer()));
}

PictureTipsWidget::~PictureTipsWidget()
{
    delete  playerTimer;
    delete ui;
}

void PictureTipsWidget::setPictureTipsType(E_VideoTipsType type)
{
    switch(type)
    {
    case PASSIVE_TRAIN_TIPS: // 主动转被动
    {
        setStepPictureList(1,"./Source/activepassive/active2passive");
        setStepPictureList(2,"./Source/resistance/red2green");
    }
        break;
    case ACTIVE_TRAIN_TIPS: //被动转主动
    {
        setStepPictureList(1,"./Source/activepassive/passive2active");
        setStepPictureList(2,"./Source/resistance/green2red");
    }
        break;
    case INITING_TIPS: //初始化
    {
        setStepPictureList(1,"./Source/activepassive/active2passive");
        setStepPictureList(2,"./Source/silder");
    }
        break;
    }
}


void PictureTipsWidget::slotPlayerTimer()
{
    QPixmap step1Pixmap,step2Pixmap;
    if(step1PictureList.size() > step1Index)
    {
        step1Pixmap.load(step1PictureList.at(step1Index));
        ui->step1Picture_Label->setPixmap(step1Pixmap);
        step1Index++;
        if(step1Index >= step1PictureList.size())
            step1Index = 0;
    }

    if(step2PictureList.size() > step2Index)
    {
        step2Pixmap.load(step2PictureList.at(step2Index));
        ui->step2Picture_Label->setPixmap(step2Pixmap);
        step2Index++;
        if(step2Index >= step2PictureList.size())
            step2Index = 0;
    }
}

void PictureTipsWidget::setStepPictureList(int index,QString dirPath)
{
    QDir dir(dirPath);
    dir.setSorting(QDir::NoSort);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);

    switch(index)
    {
    case 1: //左侧
    {
        step1PictureList.clear();
        for(int i = 0;i < fileList.size();i++)
            step1PictureList.append(fileList.at(i).absoluteFilePath());
    }
        break;
    case 2: //右侧
    {
        step2PictureList.clear();
        for(int i = 0;i < fileList.size();i++)
            step2PictureList.append(fileList.at(i).absoluteFilePath());
    }
        break;
    }
}

//void PictureTipsWidget::showEvent(QShowEvent *event)
//{
//    Q_UNUSED(event)

//}

//void PictureTipsWidget::closeEvent(QCloseEvent *event)
//{
//    Q_UNUSED(event)
//    playerTimer->stop();
//}

void PictureTipsWidget::switchPlay(bool isPlay)
{
    if(isPlay)
    {
        step1Index = 0;
        step2Index = 0;
        playerTimer->start();
    }
    else
    {
        playerTimer->stop();
    }
}

void PictureTipsWidget::setTipsMessage(int index,QString msg)
{
    if(1 == index)
    {
        ui->tip1_Label->setText(msg);
    }
    else if(2== index)
    {
        ui->tip2_Label->setText(msg);
    }
}
