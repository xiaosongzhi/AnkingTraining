#ifndef VIDEOTIPSWIDGET_H
#define VIDEOTIPSWIDGET_H

#include <QWidget>
#include "dataFormate.h"
/********视频提示窗口**********
**使用到视频提示的有以下界面
** 1.跖屈背屈评估 2.内收外展评估
** 3.内翻外翻评估 4.圆周评估
** 5.斜跖屈背屈评估
** 6.主动训练提示 7.被动训练提示
** 8.初始化提示 9.陀螺仪校准提示
***********/

namespace Ui {
class VideoTipsWidget;
}

class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;

class VideoTipsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoTipsWidget(QWidget *parent = nullptr);
    ~VideoTipsWidget();
    //打开本地视频文件
    void openLocalFile();
    //关闭播放
    void stopPlay();
    //设置要播放的视频提示类型
    void setVideoTipsYpe(E_VideoTipsType);

protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void slotStopped();
    void slotEnd();
    void on_confirm_Btn_clicked();
signals:
    //当点击确认后发送此信号
    void signalVideoTipType(E_VideoTipsType);

private:
     void setCurrentVideoFile(const QString filePath);

private:
    Ui::VideoTipsWidget *ui;
    VlcInstance *m_instance;
    VlcMedia *m_media;
    VlcMediaPlayer *m_player;
    QString m_currentFilePath;
    E_VideoTipsType E_videoTips;
};

#endif // VIDEOTIPSWIDGET_H
