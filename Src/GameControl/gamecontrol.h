#ifndef GAMECONTROL_H
#define GAMECONTROL_H

#include <QWidget>
#include "dataFormate.h"
#include <QDomDocument>
#include <QProcess>
#include <QUdpSocket>
#include "userdataformate.h"
#include "waitingdialog.h"
#include "activetrainingreport.h"

class QTimer;
class GameControl : public QWidget
{
    Q_OBJECT
public:
    //更新游戏参数，通过该文件与游戏进行通信，游戏在启动时读取该文件
    void setGamParam(ST_GameParam&);

    //通过配置表获取所有游戏的信息
    QList<ST_GameMsg> getGameMsgs();

    //根据游戏名获得游戏信息
    ST_GameMsg getGameMsgByName(QString name);

    //获取实例
    static GameControl* getInstance();

    //给游戏发送实时点
    void sendCurrentPointToGame(QPoint point);

    //给游戏发送实时角度
    void sendCurrentAngleToGame(float value,float minValue,float maxValue);

    //设置主动训练参数
    void setActiveParam(const ST_ActiveTrainParam&);

    //设置被动训练参数
    void setPassvieParam(const ST_PassiveTrainParam&);

    //启动游戏
    void startGame(QString path);
    //关闭游戏
    void stopGame();
signals:
    //游戏状态标志
    void signalGameState(E_TRAINMODE,E_GameState);

    void signalSendDestPosition(float angle);
public slots:
    //游戏数据接收
    void slotReceiveGameData();
    //游戏界面关闭时间
    void slotWaitTimer();
private:
    explicit GameControl(QWidget *parent = nullptr);

    void updateXml(ST_GameParam&);

    void initGameSocket();

    void parseGameMsg(QByteArray jsonArray);

    //生成报告
    void createTrainReport();
    //停止训练
    void sendStopCmd();

private:
    static GameControl* m_GameParamControl;

    QMap<QString,ST_GameMsg> m_mapGameName;

    QProcess* m_Process;

    QUdpSocket *m_gameSocket;

    ST_TrainReport st_TrainReport;

    int m_spasmTimes;   //痉挛次数

    bool isTraining;    //训练状态

    int m_quitType;     //退出类型

    E_TrainEvaluationType e_TrainType;

    WaitingDialog *m_waitingDialog;

    ActiveTrainingReport *m_trainReport;

    QTimer *waitTimer;

    E_TRAINMODE e_TrainMode; //主被动训练

    float m_activeMaxAngle; //主动训练中最大的角度
    float m_activeMinAngle; //主动训练中最小角度
};

#endif // GAMECONTROL_H
