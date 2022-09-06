#ifndef PASSIVETRAINING_H
#define PASSIVETRAINING_H

#include <QWidget>
#include "dataFormate.h"
#include "QTimer"

class SwitchActivePassiveWidget;
namespace Ui {
class PassiveTraining;
}

class PassiveTraining : public QWidget
{
    Q_OBJECT

public:
    explicit PassiveTraining(QWidget *parent = nullptr);
    //重写定时器事件

    ~PassiveTraining();

protected:
    void showEvent(QShowEvent *event);
private:
    //告知游戏训练参数
    void sendTrainParam(const ST_PassiveTrainParam&);
    //被动训练开闭
    void switchPassive(bool);
    //加载游戏
    void loadOnGame();

    void setGameList(QList<ST_GameMsg>& gameList);

    //将目标点以及目标速度发送给下位机
    void sendPassivePosition(ST_PassiveParam st_param);

    //停止训练
    void stopTrain();

private slots:
    void on_startTrain_Btn_clicked();

    void slotButtonGoupClicked(int id);

    void on_angleSubstract_btn_clicked();

    void on_angleAdd_btn_clicked();

    void on_secondSubtract_btn_clicked();

    void on_secondAdd_btn_clicked();

    void on_timeSubstract_btn_clicked();

    void on_timeAdd_btn_clicked();

    void receiveCommData(QByteArray);

    void on_circle_RadioButton_clicked();

    void on_subCircle_RadioButton_clicked();

    void on_circleSubstract_btn_clicked();

    void on_circleAdd_btn_clicked();

    //提示窗口已点击确认
    void slotConfirmed(E_VideoTipsType);

    void slotGetCurrentText(QString text);

    //游戏状态发生变化
    void slotGameStateChanged(E_TRAINMODE,E_GameState);

    void slotSendDestTimer();

private:
    Ui::PassiveTraining *ui;
    ST_PassiveTrainParam st_passiveTrainParam;
    bool isTraining;        //是否在训练中
    QString m_currentText;  //当前游戏的名称
    E_TrainEvaluationType evaluateTypeE;
    SwitchActivePassiveWidget *m_passiveTipsDialog;
    ST_GyroscopeData st_gyroscope;
    int m_destDirection;   //目标方向1-正向  -1负向
    int m_trainPeriod;      //训练间隔时间
    int m_arrivedTimes;     //到达次数
    QTimer *m_sendDestTimer;
};

#endif // PASSIVETRAINING_H
