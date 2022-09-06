#include "switchactivepassivewidget.h"
#include "ui_switchactivepassivewidget.h"
#include <QPainter>

SwitchActivePassiveWidget::SwitchActivePassiveWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SwitchActivePassiveWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);
    setAttribute(Qt::WA_QuitOnClose,false);
}

SwitchActivePassiveWidget::~SwitchActivePassiveWidget()
{
    delete ui;
}

void SwitchActivePassiveWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(),QColor(0,0,0,100));
}

//切换主被动提示
void SwitchActivePassiveWidget::switchTipsType(E_VideoTipsType type)
{
    m_videoTipsType = type;

    if(type == COMPREHENSIVE_TIPS_E || type == ACTIVE_TRAIN_TIPS)
    {
        type = ACTIVE_TRAIN_TIPS;
    }
    else
        type = PASSIVE_TRAIN_TIPS;

    ui->video_Widget->setPictureTipsType(type);
    switch (type)
    {
    case PASSIVE_TRAIN_TIPS:
        ui->video_Widget->setTipsMessage(1,tr("请将控制杆调至被动位置"));
        ui->video_Widget->setTipsMessage(2,tr("调整制动旋钮，使其指示在绿色区间"));
        break;
    case ACTIVE_TRAIN_TIPS:
        ui->video_Widget->setTipsMessage(1,tr("请将控制杆调至主动位置"));
        ui->video_Widget->setTipsMessage(2,tr("调整制动旋钮，使其指示在红色区间"));
        break;
    }

    ui->video_Widget->switchPlay(true);
}

void SwitchActivePassiveWidget::on_confirm_Btn_clicked()
{
    ui->video_Widget->switchPlay(false);
    this->close();
    emit signalConfirmed(m_videoTipsType);
}
