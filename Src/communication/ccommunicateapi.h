#ifndef CCOMMUNICATEAPI_H
#define CCOMMUNICATEAPI_H

#include <QObject>
#include "dataFormate.h"
class CCommunicationInterface;
class CCommunicateAPI : public QObject
{
    Q_OBJECT
public:
    static CCommunicateAPI *getInstance();

    void sendData(QByteArray);

    /************************************
    *说明:发送数据函数【重载函数】
    *参数：
    *@uint8_t seq：包的序号,默认为0，当发送的数据需要拆包时才会使用
    *@uint8_t id ：自身设备ID号
    *@uint8_t cmdID：协议ID号
    *@QByteArray arrayData：有效数据
    *返回值：无
    ***********************************/
    void sendData(uint8_t seq,uint8_t id,uint16_t cmdID,QByteArray arrayData);

    //故障复位
    void resetFault();

    //心跳开关
    void sendHeartBeat();

    //陀螺仪数据开关
    void switchEularData(bool isOn);

    //陀螺仪校准
    void TLYCheck();

    //退出当前状态
    void SetquitCmd();
signals:
    //接收到实际数据
    void signalReadyRead(QByteArray);
private:

    explicit CCommunicateAPI(QObject *parent = nullptr);
    ~CCommunicateAPI();
    void init();

private:
    static CCommunicateAPI *m_API;
    int commuType;
    CCommunicationInterface *m_interface;
};

#endif // CCOMMUNICATEAPI_H
