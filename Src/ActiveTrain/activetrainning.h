#ifndef ACTIVETRAINNING_H
#define ACTIVETRAINNING_H

#include <QWidget>
#include "dataFormate.h"
#include "activetrainingreport.h"


class SwitchActivePassiveWidget;
class QButtonGroup;
class QTimer;
namespace Ui {
class ActiveTrainning;
}

class ActiveTrainning : public QWidget
{
    Q_OBJECT

public:
    explicit ActiveTrainning(QWidget *parent = nullptr);
    ~ActiveTrainning();

private slots:

    void slotButtonGoupClicked(int id);

    void on_startTrain_Btn_clicked();

    void on_timeAdd_btn_clicked();

    void on_timeSubtract_btn_clicked();

    void receiveCommData(QByteArray);

    void on_leftAngleSubtract_btn_clicked();

    void on_leftAngleAdd_btn_clicked();

    void on_rightAngleSubtract_btn_clicked();

    void on_rightAngleAdd_btn_clicked();

    //提示窗口已点击确认
    void slotConfirmed(E_VideoTipsType);

    void on_plantarFlexion_RadioButton_clicked();

    void on_adductionAbduction_RadioButton_clicked();

    void on_varusValgus_RadioButton_clicked();

    void on_comprehensive_RadioButton_clicked();

    void on_slopePlantar_RadioButton_clicked();

    void slotGetCurrentText(QString text);

    //游戏状态发生变化
    void slotGameStateChanged(E_TRAINMODE,E_GameState);

protected:
    void showEvent(QShowEvent *event);
private:
    //停止训练
     void stopTrain();
    //主动训练开闭
    void switchActive(bool);
    //显示陀螺仪数据
    void displayGyroscopeData(const ST_GyroscopeData&);
    //陀螺仪开闭
    void switchGyroscope(bool);

    void setActiveType();

    void setGameList(QList<ST_GameMsg>& gameList);

    //加载游戏
    void loadOnGame();

    //范围选择显示(除圆周运动都显示范围)
    void setScopeDisplay(bool isShow);
private:
    Ui::ActiveTrainning *ui;
    ST_ActiveTrainParam st_activeTrainParam;
    ActiveTrainingReport *m_activetrainreport;
    ST_GyroscopeData st_gyroscope;
    E_TrainEvaluationType evaluateTypeE;
    SwitchActivePassiveWidget *m_activeTipsDialog;
    bool isTraining;        //是否在训练中
    QString m_currentText;  //当前游戏的名称
    QButtonGroup *buttonGroup;
    int testValue;
    bool isUseRealData;     //是否使用真实的边界数据
    int m_minAngle;         //下边界
    int m_maxAngle;         //上边界
    float basePitch;        //基准俯仰角
};

#endif // ACTIVETRAINNING_H
