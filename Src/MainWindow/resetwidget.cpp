#include "resetwidget.h"
#include "ui_resetwidget.h"
#include "ccommunicateapi.h"
#include <QDebug>
#include <QPainter>
ResetWidget::ResetWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResetWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);
    setAttribute(Qt::WA_QuitOnClose,false);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);
    //设置图片播放
    ui->video_Widget->setPictureTipsType(INITING_TIPS);

}

ResetWidget::~ResetWidget()
{
    delete ui;
}

void ResetWidget::on_confirm_Btn_clicked()
{
    resetDevice();
    //显示复位中的提示
//    ui->deviceResetTip_GroupBox->setVisible(true);
//    ui->confirm_Btn->setVisible(false);
//    ui->video_Widget->setVisible(false);
    ui->video_Widget->switchPlay(false);
    setResettingState();
}

//复位设备
void ResetWidget::resetDevice()
{
    uint16_t cmdId = ERROR_CHECKRESULT;
    uint8_t seq = 0;
    uint8_t id = 1;
    QByteArray array;
    array.resize(1);
    array[0] = 1;
    CCommunicateAPI::getInstance()->sendData(seq,id,cmdId,array);
}

void ResetWidget::receiveCommData(QByteArray receiveData)
{
    int length = receiveData[1];
    QByteArray data = receiveData.mid(6,length);
    uint16_t comID;
    memcpy(&comID,receiveData.data()+4,2);

    switch(comID)
    {
    case ERROR_CHECKRESULT:
    {
        //校准陀螺仪
        CCommunicateAPI::getInstance()->TLYCheck();

        this->close();
        //解锁左侧按钮
    }
        break;
    }
}

void ResetWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    connect(CCommunicateAPI::getInstance(),SIGNAL(signalReadyRead(QByteArray)),this,SLOT(receiveCommData(QByteArray)));
//    ui->deviceResetTip_GroupBox->setVisible(false);
//    ui->confirm_Btn->setVisible(true);
//    ui->video1_Widget->setVisible(true);
//    ui->video_Widget->setVisible(true);
    setBeforeResetState();
}

void ResetWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    disconnect(CCommunicateAPI::getInstance(),SIGNAL(signalReadyRead(QByteArray)),this,SLOT(receiveCommData(QByteArray)));
}

void ResetWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(),QColor(0,0,0,100));
}

void ResetWidget::on_close_Btn_clicked()
{
    this->close();
}

//复位前
void ResetWidget::setBeforeResetState()
{
    ui->deviceResetTip_GroupBox->setVisible(false);
    ui->widget->setVisible(false);
    ui->video_Widget->setVisible(true);
    ui->confirm_Btn->setVisible(true);
    ui->video_Widget->switchPlay(true);
    ui->video_Widget->setTipsMessage(1,tr("请将控制杆调到被动位置"));
    ui->video_Widget->setTipsMessage(2,tr("请将定位柱塞移动至中点"));
}
//复位中
void ResetWidget::setResettingState()
{
    ui->deviceResetTip_GroupBox->setVisible(true);
    ui->widget->setVisible(true);
    ui->video_Widget->setVisible(false);
    ui->confirm_Btn->setVisible(false);
}
