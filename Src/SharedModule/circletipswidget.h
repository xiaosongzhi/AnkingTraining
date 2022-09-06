#ifndef CIRCLETIPSWIDGET_H
#define CIRCLETIPSWIDGET_H

#include <QWidget>
#include "dataFormate.h"
namespace Ui {
class CircleTipsWidget;
}

class CircleTipsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CircleTipsWidget(QWidget *parent = nullptr);
    ~CircleTipsWidget();

    //只有两种情况需要三部提示 (1)圆周评估 (2)被动运动
    void switchTipsType(E_VideoTipsType);

    void setStepPictureList(int index,QString dirPath);

    void switchPlay(bool isPlay);

    void setTipsMessage(int index,QString msg);
public slots:
    void slotPlayerTimer();
signals:
    //点击确认
    void signalConfirmed(E_VideoTipsType);
protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *event);
private slots:
    void on_confirm_Btn_clicked();

private:
    Ui::CircleTipsWidget *ui;
    QTimer *playerTimer;

    QList<QString> step1PictureList;
    QList<QString> step2PictureList;
    QList<QString> step3PictureList;

    int step1Index;
    int step2Index;
    int step3Index;

    E_VideoTipsType m_videoTipsType;
};

#endif // CIRCLETIPSWIDGET_H
