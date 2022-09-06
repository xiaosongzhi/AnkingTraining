#include "videotipswidget.h"
#include "ui_videotipswidget.h"

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

VideoTipsWidget::VideoTipsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoTipsWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground,true);//设置无边框
    m_instance = new VlcInstance(VlcCommon::args(),this);
    m_player = new VlcMediaPlayer(m_instance);
    m_player->setVideoWidget(ui->video_Widget);
    connect(m_player,SIGNAL(stopped()),this,SLOT(slotStopped()));
    connect(m_player,SIGNAL(end()),this,SLOT(slotEnd()));
}

VideoTipsWidget::~VideoTipsWidget()
{
    delete m_player;
    delete m_media;
    delete m_instance;
    delete ui;
}

void VideoTipsWidget::openLocalFile()
{
    if(!m_currentFilePath.isEmpty())
    {
        m_media = new VlcMedia(m_currentFilePath,true,m_instance);
        m_player->open(m_media);
    }
}

void VideoTipsWidget::stopPlay()
{
    m_player->stop();
}

void VideoTipsWidget::slotStopped()
{
    delete m_media;
    m_media = NULL;
}

void VideoTipsWidget::slotEnd()
{
    delete m_media;
    m_media = NULL;
    openLocalFile();
}

void VideoTipsWidget::on_confirm_Btn_clicked()
{
    this->close();
    stopPlay();
    emit signalVideoTipType(E_videoTips);
}

void VideoTipsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(),QColor(0,0,0,100));
}

void VideoTipsWidget::setCurrentVideoFile(const QString filePath)
{
    m_currentFilePath = filePath;
}

void VideoTipsWidget::setVideoTipsYpe(E_VideoTipsType E_TipsType)
{
    E_videoTips = E_TipsType;
    QString filePath;
    switch(E_TipsType)
    {
    case COMPREHENSIVE_TIPS_E:
        break;
    case PLANTAR_FLEXION_TIPS_E:
        break;
    case ADDUCTION_ABDUCTION_TIPS_E:
        break;
    case VARUS_VALGUS_TIPS_E:
        break;
    case SLOPE_PLANTAR_FLEXION_TIPS_E:
        break;
    case PASSIVE_TRAIN_TIPS:
        break;
    case ACTIVE_TRAIN_TIPS:
        break;
    case INITING_TIPS:
        break;
    case GYROSCOPE_CHECK_TIPS:
        break;
    }
}
