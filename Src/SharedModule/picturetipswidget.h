#ifndef PICTURETIPSWIDGET_H
#define PICTURETIPSWIDGET_H

#include <QWidget>
#include "dataFormate.h"

class QTimer;
namespace Ui {
class PictureTipsWidget;
}

class PictureTipsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PictureTipsWidget(QWidget *parent = nullptr);
    ~PictureTipsWidget();
    //设置要播放动画的路径
    void setPictureTipsType(E_VideoTipsType);
    //动画播放开关
    void switchPlay(bool);
    //设置提示语index 1-left 2-right
    void setTipsMessage(int index,QString msg);
protected:
//    void paintEvent(QPaintEvent *event);

//    void showEvent(QShowEvent *event);

//    void closeEvent(QCloseEvent *event);
private slots:

    void slotPlayerTimer();

private:
    void setStepPictureList(int index,QString dirPath);

private:
    Ui::PictureTipsWidget *ui;
    QTimer *playerTimer;

    QList<QString> step1PictureList;
    QList<QString> step2PictureList;

    int step1Index;
    int step2Index;
};

#endif // PICTURETIPSWIDGET_H
