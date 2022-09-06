#include "gamecontrol.h"
#include <QFile>
#include <QTextStream>
#include "logger.h"
#include "readconfig.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDateTime>
#include "currentuserdata.h"
#include "cdatabaseinterface.h"
#include <QTimer>

//LOG4QT_DECLARE_STATIC_LOGGER(logger, GameControl)

GameControl* GameControl::m_GameParamControl = NULL;

GameControl::GameControl(QWidget *parent)
    : QWidget(parent),
      m_gameSocket(NULL),
      m_waitingDialog(NULL),
      m_trainReport(NULL),
      waitTimer(NULL),
      m_activeMaxAngle(0),
      m_activeMinAngle(0)
{
    m_Process = new QProcess();
    initGameSocket();
    m_waitingDialog = new WaitingDialog();
    m_waitingDialog->setWaitingType(INITWAITING);
    m_trainReport = new ActiveTrainingReport();

    waitTimer = new QTimer();
    waitTimer->setInterval(500);
    waitTimer->setSingleShot(true);
    connect(waitTimer,SIGNAL(timeout()),this,SLOT(slotWaitTimer()));
}

void GameControl::initGameSocket()
{
    m_gameSocket = new QUdpSocket();
    int16_t port;
    QString IP;
    ReadConfig::getInstance()->getGameSeverAddress(port,IP);
    if(m_gameSocket->bind(port))
    {
        qDebug()<<(QString("游戏服务端口%1").arg(port));
    }
    connect(m_gameSocket,&QUdpSocket::readyRead,this,&GameControl::slotReceiveGameData);
}

GameControl* GameControl::getInstance()
{
    if(!m_GameParamControl)
    {
        m_GameParamControl = new GameControl();
    }
    return m_GameParamControl;
}

void GameControl::updateXml(ST_GameParam& st_GameParam)
{
    QFile file("./gameExecutable/gameConfig.xml");
    if(!file.open(QFile::ReadOnly))
        return;
    QDomDocument doc;

    if(!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();

    QDomElement root = doc.documentElement();

    if(1)
    {
        QDomNodeList timeList = root.elementsByTagName("GameTime");
        QDomNode timeNode = timeList.at(0);
        QDomNode oldnode=timeNode.firstChild();
        timeNode.firstChild().setNodeValue(QString::number(st_GameParam.gameTime));
        QDomNode newNode = timeNode.firstChild();
        timeNode.replaceChild(newNode,oldnode);
    }
    if(1)
    {
        QDomNodeList levelList = root.elementsByTagName("Level");
        QDomNode levelNode = levelList.at(0);
        QDomNode oldLevelnode=levelNode.firstChild();
        levelNode.firstChild().setNodeValue(QString::number(st_GameParam.hardLevel));
        QDomNode newLevelNode = levelNode.firstChild();
        levelNode.replaceChild(newLevelNode,oldLevelnode);
    }
    if(1)
    {
        QDomNodeList List = root.elementsByTagName("Speed");
        QDomNode node = List.at(0);
        QDomNode oldLevelnode=node.firstChild();
        node.firstChild().setNodeValue(QString::number(st_GameParam.speed));
        QDomNode newLevelNode = node.firstChild();
        node.replaceChild(newLevelNode,oldLevelnode);
    }
    if(1)
    {
        qDebug()<<"st_GameParam.trainType"<<st_GameParam.trainType;
        QDomNodeList List = root.elementsByTagName("TrainType");
        QDomNode node = List.at(0);
        QDomNode oldLevelnode=node.firstChild();
        node.firstChild().setNodeValue(QString::number(st_GameParam.trainType));
        QDomNode newLevelNode = node.firstChild();
        node.replaceChild(newLevelNode,oldLevelnode);
    }
    if(1)
    {
        qDebug()<<"AngleVlue"<<st_GameParam.angleValue;
        QDomNodeList List = root.elementsByTagName("AngleVlue");
        QDomNode node = List.at(0);
        QDomNode oldLevelnode=node.firstChild();
        node.firstChild().setNodeValue(QString::number(st_GameParam.angleValue));
        QDomNode newLevelNode = node.firstChild();
        node.replaceChild(newLevelNode,oldLevelnode);
    }
    if(1)
    {
        QDomNodeList List = root.elementsByTagName("WaitTime");
        QDomNode node = List.at(0);
        QDomNode oldLevelnode=node.firstChild();
        node.firstChild().setNodeValue(QString::number(st_GameParam.waitTime));
        QDomNode newLevelNode = node.firstChild();
        node.replaceChild(newLevelNode,oldLevelnode);
    }
    if(1)
    {
        QDomNodeList List = root.elementsByTagName("MaxCircle");
        QDomNode node = List.at(0);
        QDomNode oldLevelnode=node.firstChild();
        node.firstChild().setNodeValue(QString::number(st_GameParam.maxCircle));
        QDomNode newLevelNode = node.firstChild();
        node.replaceChild(newLevelNode,oldLevelnode);
    }
    if(1)
    {
        QDomNodeList List = root.elementsByTagName("ReadPosTable");
        QDomNode node = List.at(0);
        QDomNode oldLevelnode=node.firstChild();
        QString ok("false");
        if(st_GameParam.readPosTable)
            ok = "true";
        else
            ok = "false";
        node.firstChild().setNodeValue(ok);
        QDomNode newLevelNode = node.firstChild();
        node.replaceChild(newLevelNode,oldLevelnode);
    }

    QDomNode n = root.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName() == "TrainingTrack")
            {
                e.setAttribute("path",st_GameParam.trainTrackFilePath);
            }
        }
        n = n.nextSibling();
    }

    if(!file.open(QFile::WriteOnly|QFile::Truncate))
        return;
    //输出到文件
    QTextStream out_stream(&file);
    doc.save(out_stream,4); //缩进4格
    file.close();
}

void GameControl::setGamParam(ST_GameParam& st_GameParam)
{
    //将本地参数转化成游戏中用到的参数
    if(5 == st_GameParam.trainType)
        st_GameParam.trainType = 1;
    if(6 == st_GameParam.trainType)
        st_GameParam.trainType = 2;
    updateXml(st_GameParam);
}

QList<ST_GameMsg> GameControl::getGameMsgs()
{
    QList<ST_GameMsg> gameMsgList;
    QFile file("./ConfigFile/gameListConfig.xml");

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"read gameListConfig failed";
        return gameMsgList;
    }
    QDomDocument doc;
    QString error;
    int row,colum;
    if(!doc.setContent(&file,&error,&row,&colum))
    {
        file.close();
        return gameMsgList;
    }
    file.close();

    QDomElement root=doc.documentElement(); //返回根节点

    QDomNode node=root.firstChild(); //获得第一个子节点

    while(!node.isNull())
    {
        if(node.isElement()) //如果节点是元素
        {
            ST_GameMsg st_GameMsg;
            QDomElement e=node.toElement();
            st_GameMsg.gameID = e.attribute("ID").toInt();
            st_GameMsg.gameName = e.attribute("name");
            st_GameMsg.gamePath = e.attribute("path");
            st_GameMsg.iconPath = e.attribute("iconPath");
            QString suitType = e.attribute("suitType");
            st_GameMsg.suitTypeList = suitType.split(',');
            gameMsgList.append(st_GameMsg);
            m_mapGameName.insert(st_GameMsg.gameName,st_GameMsg);
        }
        node = node.nextSibling();
    }
    return gameMsgList;
}

ST_GameMsg GameControl::getGameMsgByName(QString name)
{
    ST_GameMsg st_gameMsg;
    QMapIterator<QString, ST_GameMsg> i(m_mapGameName);

    while(i.hasNext())
    {
        i.next();
    }

    if(m_mapGameName.contains(name))
    {
        st_gameMsg = m_mapGameName.value(name);
    }
    return st_gameMsg;
}

void GameControl::startGame(QString path)
{
    //1.开启游戏进程
    if(path.isEmpty())
        return;
    QString hardDisk = path.mid(0,2);
    hardDisk.append("\n\r");

    QString gameName = path.mid(path.lastIndexOf('/')+1);
    gameName.prepend("start ");
    gameName.append("\n\r");
    QString gamePath = path.mid(0,path.lastIndexOf('/'));
    gamePath.prepend("cd ");
    gamePath.append("\n\r");
    m_Process->start("cmd.exe");
    //切换盘符
    m_Process->write(hardDisk.toLatin1());
    //进入文件夹
    m_Process->write(gamePath.toLatin1());
    //开启进程
    m_Process->write(gameName.toLatin1());
    m_Process->write("exit\n\r");
    m_Process->waitForFinished();
    m_Process->close();
    //2.关闭设备复位中的界面
}

//游戏数据接收
void GameControl::slotReceiveGameData()
{
    while(m_gameSocket->hasPendingDatagrams())
    {
        QByteArray buf;
        buf.resize(m_gameSocket->pendingDatagramSize());
        m_gameSocket->readDatagram(buf.data(),buf.size());
        parseGameMsg(buf);
    }
}

void GameControl::parseGameMsg(QByteArray jsonArray)
{
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(jsonArray, &jsonError);  // 转化为 JSON 文档
 #if 1
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if(doucment.isObject())
        {
            QJsonObject object = doucment.object();  // 转化为对象
            if(object.contains("msgID"))
            {
                int msgID = object.value("msgID").toInt();
                switch(msgID)
                {
                case 1: //游戏开始信号
                {
                    m_spasmTimes = 0;
                    isTraining = true;
                    waitTimer->start();
                    st_TrainReport.startTime = QDateTime::currentDateTime();
                    emit signalGameState(e_TrainMode,GameStart_E);
                }
                    break;
                case 2: //游戏给出的目标位置
                {
                    {
                        if(object.contains("m_xv"))
                        {
                            QJsonObject xObject = object.value("m_xv").toObject();
                            int current = 0;
                            int max = 1;
                            if(xObject.contains("current"))
                                current = xObject.value("current").toInt();
                            if(xObject.contains("max"))
                                max = xObject.value("max").toInt();
                        }
                        if(object.contains("m_yv"))
                        {
                            QJsonObject yObject = object.value("m_yv").toObject();
                            int current = 0;
                            int max = 1;
                            if(yObject.contains("current"))
                                current = yObject.value("current").toInt();
                            if(yObject.contains("max"))
                                max = yObject.value("max").toInt();
                        }
                        if(object.contains("tarAngle"))
                        {
                            float angle = 0;
                            angle = object.value("tarAngle").toDouble();
//                            emit signalSendDestPosition(angle);
                        }
                    }

                }
                    break;
                case 3: //结束信息
                {
                    isTraining = false;
                    if(object.contains("quitType"))
                    {
                        //告知下位机训练完成
                        sendStopCmd();
                        m_quitType = object.value("quitType").toInt();
                        //正常退出
                        if(1 == m_quitType)
                        {
                            emit signalGameState(e_TrainMode,GameEnd_E);
                            st_TrainReport.endTime = QDateTime::currentDateTime();
                            if(object.contains("totalTime"))
                                st_TrainReport.duration = object.value("totalTime").toInt();
                            if(object.contains("totalScore"))
                                st_TrainReport.score = object.value("totalScore").toInt();
                        }
                        else
                        {
                            emit signalGameState(e_TrainMode,GameEnd_E);
                            st_TrainReport.endTime = QDateTime::currentDateTime();
                            if(object.contains("totalTime"))
                                st_TrainReport.duration = object.value("totalTime").toInt();
                            if(object.contains("totalScore"))
                                st_TrainReport.score = object.value("totalScore").toInt();
                        }
                        //生成训练报告
                        createTrainReport();
                    }
                }
                    break;
                }
            }
        }
    }
    else
    {

//        logger()->debug(jsonError.errorString());
        //此处不该在此处停止心跳
    }
#endif
}

void GameControl::createTrainReport()
{
    ST_PatientMsg st_currentPatient = CurrentUserData::getInstace()->getCurrentPatientMsg();
    st_TrainReport.ID = st_currentPatient.ID;
    st_TrainReport.name = st_currentPatient.name;

    //从数据库中查询总时间
    int lastTotalDuration;
    QString query("select * from  TrainReport order by startTime DESC;");
    if(!CDatabaseInterface::getInstance()->exec(query))
        qDebug()<<CDatabaseInterface::getInstance()->getLastError()<<"ERROR";
    int count = CDatabaseInterface::getInstance()->getValuesSize();
    if(count >0)
    {
       QList<QVariantMap> maplist = CDatabaseInterface::getInstance()->getValues(0,1);
       ST_TrainReport st_trainReport = variantMapToTrainReport(maplist.at(0));
       lastTotalDuration = st_trainReport.totalDuration;
       st_TrainReport.totalDuration = lastTotalDuration + st_TrainReport.duration;
    }
    st_TrainReport.level = 1;
    if(0 == st_TrainReport.trainMode)
        m_trainReport->setReportType(tr("被动训练报告"));
    else if(1 == st_TrainReport.trainMode)
        m_trainReport->setReportType(tr("主动训练报告"));

    m_trainReport->setTrainReport(st_TrainReport);


}

void GameControl::sendStopCmd()
{
    QJsonObject object;
    object.insert("msgID",4);
    QJsonDocument document;
    document.setObject(object);
    QByteArray sendArray = document.toJson();
    QString ip("127.0.0.1");
    int16_t port = 12000;
    for(int i = 0;i < 5;i++)
    {
        m_gameSocket->writeDatagram(sendArray,QHostAddress(ip),port);
        Sleep(100);
    }
}

void GameControl::sendCurrentPointToGame(QPoint point)
{
    QJsonObject Xobject;
    Xobject.insert("current",point.x());
    Xobject.insert("min",0);

    QJsonObject Yobject;
    Yobject.insert("current",point.y());
    Yobject.insert("min",0);

    QJsonObject object;
    object.insert("msgID",2);
    object.insert("TypeID",1);
    object.insert("m_xv",Xobject);
    object.insert("m_yv",Yobject);
    QJsonDocument document;
    document.setObject(object);
    QByteArray sendArray = document.toJson();
    QString ip("127.0.0.1");
    int16_t port = 12000;
    m_gameSocket->writeDatagram(sendArray,QHostAddress(ip),port);
}

void GameControl::sendCurrentAngleToGame(float value,float minValue,float maxValue)
{
    //主动训练
    if(1 == st_TrainReport.trainMode)
    {
        if(m_activeMaxAngle < value)
            m_activeMaxAngle = value;
        if(m_activeMinAngle > value)
            m_activeMinAngle = value;

        st_TrainReport.highMargin = m_activeMaxAngle;
        st_TrainReport.lowMargin = m_activeMinAngle;
    }

    float tarAngle = 50;
    QJsonObject Xobject;

    Xobject.insert("current",value);
    Xobject.insert("min",minValue);
    Xobject.insert("max",maxValue);
    QJsonObject object;
    object.insert("msgID",2);
    object.insert("TypeID",1);
    object.insert("m_angle",Xobject);
    object.insert("tarAngle",tarAngle);
    QJsonDocument document;
    document.setObject(object);
    QByteArray sendArray = document.toJson();

    QString ip("127.0.0.1");
    int16_t port = 12000;
    m_gameSocket->writeDatagram(sendArray,QHostAddress(ip),port);
}

//设置主动训练参数
void GameControl::setActiveParam(const ST_ActiveTrainParam &st_activeParam)
{
    e_TrainMode = TRAINMODE_ACTIVE_E;
    //打开游戏载入界面
    m_waitingDialog->setDialogCloseState(false);

    st_TrainReport.gameName = st_activeParam.gameName;
    st_TrainReport.trainMode = 1;
    st_TrainReport.trainTypeE = st_activeParam.trackType;
    st_TrainReport.highMargin = st_activeParam.maxAngle;
    st_TrainReport.lowMargin = st_activeParam.minAngle;

    ST_GameParam st_gameParam;
    st_gameParam.gameTime = st_activeParam.trainTime;
    st_gameParam.angleValue = st_activeParam.maxAngle;
    st_gameParam.maxCircle = 0;
    st_gameParam.hardLevel = 2;
    st_gameParam.waitTime = 0;
    st_gameParam.trainType = 5;

    m_activeMaxAngle = 0;
    m_activeMinAngle = 0;

    setGamParam(st_gameParam);
}

//设置被动训练参数
void GameControl::setPassvieParam(const ST_PassiveTrainParam &st_passiveParam)
{
    e_TrainMode = TRAINMODE_PASSIVE_E;
    //打开游戏载入界面
    m_waitingDialog->setDialogCloseState(false);
    st_TrainReport.gameName = st_passiveParam.gameName;
    st_TrainReport.trainMode = 0;
    st_TrainReport.trainTypeE = st_passiveParam.trackType;
    st_TrainReport.highMargin = st_passiveParam.maxAngle;
    st_TrainReport.lowMargin = st_passiveParam.minAngle;

    st_TrainReport.speed = (4+st_passiveParam.speed)/10.f * 16;

    ST_GameParam st_gameParam;
    st_gameParam.angleValue = st_passiveParam.maxAngle;
    st_gameParam.gameTime = st_passiveParam.trainTime;
    st_gameParam.waitTime = st_passiveParam.intervalTime;
    st_gameParam.maxCircle = st_passiveParam.circleNum;
    st_gameParam.trainType = st_passiveParam.trackType;

    setGamParam(st_gameParam);
}

//游戏界面关闭时间
void GameControl::slotWaitTimer()
{
    //关闭等待界面
    m_waitingDialog->setDialogCloseState(true);
}

//关闭游戏
void GameControl::stopGame()
{
    sendStopCmd();
}
