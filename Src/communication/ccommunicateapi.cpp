#include "ccommunicateapi.h"
#include "readconfig.h"
#include "ccommunicationinterface.h"
#include "cserialportinterface.h"
#include "cudpinterface.h"
#include "ctcpsocketinterface.h"
#include "dataFormate.h"
#include <QDebug>

CCommunicateAPI *CCommunicateAPI::m_API = NULL;

CCommunicateAPI::CCommunicateAPI(QObject *parent) :
    QObject(parent),
    m_interface(NULL)
{
    init();
}
CCommunicateAPI::~CCommunicateAPI()
{
    if(m_interface)
    {
        delete m_interface;
        m_interface = NULL;
    }
}

CCommunicateAPI *CCommunicateAPI::getInstance()
{
    if(!m_API)
    {
        m_API = new CCommunicateAPI();
    }
    return m_API;
}

void CCommunicateAPI::init()
{
    commuType = ReadConfig::getInstance()->getCommunicateType();
    //根据通信方式生成不同的对象
    if(0 == commuType) //串口
    {
        m_interface = new CSerialportInterface();
    }
    else if(1 == commuType) //udp
    {
        m_interface = new CUdpInterface();
    }
    else if(2 == commuType) //tcp
    {
        m_interface = new CTcpSocketInterface();
    }
    else if(3 == commuType) //can
    {
        //待添加
    }
    if(m_interface)
    {
        connect(m_interface,SIGNAL(signalReadyRead(QByteArray)),this,SIGNAL(signalReadyRead(QByteArray)));
    }
}

void CCommunicateAPI::sendData(QByteArray sendArray)
{
    sendArray.prepend(PACKHEAD);
    //添加校验
    uint8_t CRC_H = 0;
    uint8_t CRC_L = 0;
    Pressure_CheckCRC((uint8_t*)(sendArray.data()),sendArray.length(),&CRC_H,&CRC_L);
    sendArray.append(CRC_L);
    sendArray.append(CRC_H);
    m_interface->sendDataInterface(sendArray);
}

void CCommunicateAPI::sendData(uint8_t seq,uint8_t id,uint16_t cmdID,QByteArray arrayData)
{

    QByteArray fixedArray;
    fixedArray.resize(5);
    fixedArray[0] = arrayData.length();
    fixedArray[1] = seq;
    fixedArray[2] = id;
    memcpy(fixedArray.data()+3,&cmdID,sizeof(uint16_t));

    fixedArray.append(arrayData);
    sendData(fixedArray);
}
//故障复位
void CCommunicateAPI::resetFault()
{
    QByteArray sendArray;
    sendArray.resize(4);
    sendArray[0] = 0;
    sendArray[1] = 0;
    sendArray[2] = 1;
    sendArray[3] = 0x06;
    sendData(sendArray);
}

//发送心跳
void CCommunicateAPI::sendHeartBeat()
{
    uint16_t cmdId = HEART_BEAT_CMD;
    uint8_t seq = 0;
    uint8_t id = 1;
    QByteArray array;
    array.resize(0);
    sendData(seq,id,cmdId,array);
}

//陀螺仪数据开关
void CCommunicateAPI::switchEularData(bool isOn)
{
    if(isOn)
        qDebug()<<"打开陀螺仪数据";
    else
        qDebug()<<"关闭陀螺仪数据";
    uint16_t cmdId = GET_GYROSCOPE_DATA;
    uint8_t seq = 0;
    uint8_t id = 1;
    QByteArray array;
    int8_t flag = 0;
    if(isOn)
        flag = 1;
    else
        flag = 2;
    array.resize(1);
    memcpy(array.data(),&flag,sizeof(flag));
    sendData(seq,id,cmdId,array);
}

//陀螺仪校准
void CCommunicateAPI::TLYCheck()
{
    uint16_t cmdId = GYROSCOPE_CHECK;
    uint8_t seq = 0;
    uint8_t id = 1;
    QByteArray array;
    array.resize(0);
    CCommunicateAPI::getInstance()->sendData(seq,id,cmdId,array);
}

void CCommunicateAPI::SetquitCmd()
{
    uint16_t cmdId = QUIT_CMD;
    uint8_t seq = 0;
    uint8_t id = 1;
    QByteArray array;
    array.resize(0);
    CCommunicateAPI::getInstance()->sendData(seq,id,cmdId,array);
}
