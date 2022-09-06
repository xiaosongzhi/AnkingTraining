#include "passivetraining.h"
#include "ui_passivetraining.h"
#include <QButtonGroup>
#include "ccommunicateapi.h"
#include <QDebug>
#include "switchactivepassivewidget.h"
#include "gamecontrol.h"
//#pragma execution_character_set("utf-8")

PassiveTraining::PassiveTraining(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PassiveTraining),
    isTraining(false),
    m_passiveTipsDialog(NULL),
    m_destDirection(1),
    m_trainPeriod(1000),
    m_arrivedTimes(0),
    m_sendDestTimer(NULL)
{
    ui->setupUi(this);
    connect(CCommunicateAPI::getInstance(),SIGNAL(signalReadyRead(QByteArray)),this,SLOT(receiveCommData(QByteArray)));
    connect(GameControl::getInstance(),SIGNAL(signalGameState(E_TRAINMODE,E_GameState)),this,SLOT(slotGameStateChanged(E_TRAINMODE,E_GameState)));
    m_passiveTipsDialog = new SwitchActivePassiveWidget();
    connect(m_passiveTipsDialog,SIGNAL(signalConfirmed(E_VideoTipsType)),this,SLOT(slotConfirmed(E_VideoTipsType)));
    on_circle_RadioButton_clicked();
    m_sendDestTimer = new QTimer();
    m_sendDestTimer->setSingleShot(true);
    connect(m_sendDestTimer,SIGNAL(timeout()),this,SLOT(slotSendDestTimer()));
}

PassiveTraining::~PassiveTraining()
{
    if(m_passiveTipsDialog)
        delete m_passiveTipsDialog;
    delete ui;
}
//发送被动训练参数
void PassiveTraining::on_startTrain_Btn_clicked()
{
    if(!isTraining)
    {
        //告知下位机开启被动训练
        switchPassive(true);
        //将训练参数返送给游戏
        isTraining = true;
        m_arrivedTimes = 0;
        m_destDirection = 1;

        st_passiveTrainParam.circleNum = ui->maxCircle_lineEdit->text().toUInt();
        st_passiveTrainParam.maxAngle = ui->maxAngle_lineEdit->text().remove("°").toUInt();
        if(ui->circle_RadioButton->isChecked())
            st_passiveTrainParam.maxAngle = st_passiveTrainParam.circleNum*360;
        st_passiveTrainParam.minAngle = 0;//默认值
        st_passiveTrainParam.intervalTime = 0;
        if(ui->level1_RadioButton->isChecked())
            st_passiveTrainParam.speed = 1;
        if(ui->level2_RadioButton->isChecked())
            st_passiveTrainParam.speed = 2;
        if(ui->level3_RadioButton->isChecked())
            st_passiveTrainParam.speed = 3;
        if(ui->level4_RadioButton->isChecked())
            st_passiveTrainParam.speed = 4;
        if(ui->level5_RadioButton->isChecked())
            st_passiveTrainParam.speed = 5;
        if(ui->level6_RadioButton->isChecked())
            st_passiveTrainParam.speed = 6;
        st_passiveTrainParam.trainTime = ui->totalTime_lineEdit->text().toUInt()*60;

        //设置游戏的被动训练参数
        st_passiveTrainParam.gameName = m_currentText;
        GameControl::getInstance()->setPassvieParam(st_passiveTrainParam);

        Sleep(100);
        //打开陀螺仪数据
        CCommunicateAPI::getInstance()->switchEularData(true);
    }

}

void PassiveTraining::slotButtonGoupClicked(int id)
{
    st_passiveTrainParam.speed = id;
}

void PassiveTraining::sendTrainParam(const ST_PassiveTrainParam& param)
{
    uint16_t cmdId = SET_PASSIVE_PARAMETER;
    uint8_t seq = 0;
    uint8_t id = 1;
    QByteArray array;
    array.resize(sizeof(ST_PassiveTrainParam));
    memcpy(array.data(),&param,sizeof(param));
    CCommunicateAPI::getInstance()->sendData(seq,id,cmdId,array);
}

void PassiveTraining::switchPassive(bool isOn)
{
    uint16_t cmdId = SWITCH_PASSIVE_TRAINNING;
    uint8_t seq = 0;
    uint8_t id = 1;
    QByteArray array;
    array.resize(sizeof(uint8_t));
    uint8_t flag;
    if(isOn)
        flag = 1;
    else
        flag = 0;
    memcpy(array.data(),&flag,sizeof(flag));
    CCommunicateAPI::getInstance()->sendData(seq,id,cmdId,array);
}

void PassiveTraining::on_angleSubstract_btn_clicked()
{
    int angle = ui->maxAngle_lineEdit->text().remove("°").toInt();
    angle--;
    if(angle < 0)
        angle = 0;
    ui->maxAngle_lineEdit->setText(QString::number(angle)+"°");
}

void PassiveTraining::on_angleAdd_btn_clicked()
{
    int angle = ui->maxAngle_lineEdit->text().remove("°").toInt();
    angle++;
    if(angle >500)
        angle = 500;
    ui->maxAngle_lineEdit->setText(QString::number(angle)+"°");
}

//间歇时间
void PassiveTraining::on_secondSubtract_btn_clicked()
{
    int time = ui->interval_lineEdit->text().toInt();
    time--;
    if(time < 1)
        time = 1;
    ui->interval_lineEdit->setText(QString::number(time));
    m_trainPeriod = time*1000;
}

void PassiveTraining::on_secondAdd_btn_clicked()
{
    int time = ui->interval_lineEdit->text().toInt();
    time++;
    if(time > 10)
        time = 10;
    ui->interval_lineEdit->setText(QString::number(time));
    m_trainPeriod = time*1000;
}

//训练时间
void PassiveTraining::on_timeSubstract_btn_clicked()
{
    int time = ui->totalTime_lineEdit->text().toUInt();
    time--;
    if(time < 1)
        time = 1;
    ui->totalTime_lineEdit->setText(QString::number(time));
}

void PassiveTraining::on_timeAdd_btn_clicked()
{
    int time = ui->totalTime_lineEdit->text().toUInt();
    time++;
    if(time > 99 )
        time = 99;
    ui->totalTime_lineEdit->setText(QString::number(time));
}

void PassiveTraining::receiveCommData(QByteArray receiveData)
{
    if(isTraining)
    {
        int length = receiveData[1];
        QByteArray data = receiveData.mid(6,length);
        uint16_t comID;
        memcpy(&comID,receiveData.data()+4,2);
        switch(comID)
        {
        case ERROR_CHECKRESULT: //初始化完成，到达被动训练点
        {
            //开启游戏
            ST_GameMsg st_game = GameControl::getInstance()->getGameMsgByName(m_currentText);
            GameControl::getInstance()->startGame(st_game.gamePath);
        }
            break;
        case GIVE_GYROSCOPE_DATA://给出陀螺仪数据
        {
            memcpy(&st_gyroscope,data.data(),sizeof(ST_GyroscopeData));
            GameControl::getInstance()->sendCurrentAngleToGame(360-st_gyroscope.coderAngle,0,360);
        }
            break;
        case ARRIVED_PASSIVE_POSITION:
        {
            static QDateTime lastDateTime;
            QDateTime currentDateTime = QDateTime::currentDateTime();
            int period = lastDateTime.msecsTo(currentDateTime);
            if(period< 500 && period >0)
                return;
            lastDateTime = currentDateTime;
            m_arrivedTimes++;
            if(m_arrivedTimes%2 == 0)
                m_destDirection = 1;
            else
                m_destDirection = -1;
            m_sendDestTimer->start(m_trainPeriod);
        }
            break;
        case DEBUG_CMD: //下位机收到退出指令后回复消息
        {
            int8_t sendTimes = 0;
            memcpy(&sendTimes,data.data(),sizeof(int8_t));
        }
            break;
        }
    }
}

void PassiveTraining::sendPassivePosition(ST_PassiveParam st_param)
{
    uint16_t cmdId = SET_PASSIVE_PARAMETER;
    uint8_t seq = 0;
    uint8_t id = 1;
    QByteArray array;
    array.resize(sizeof(st_param));
    memcpy(array.data(),&st_param,sizeof(st_param));
    CCommunicateAPI::getInstance()->sendData(seq,id,cmdId,array);
}

void PassiveTraining::on_circle_RadioButton_clicked()
{
    st_passiveTrainParam.trackType = PASSIVE_TRAIN_CIRCLE;
    ui->trainScope_StackedWidget->setCurrentIndex(1);
    loadOnGame();
}

void PassiveTraining::on_subCircle_RadioButton_clicked()
{
    st_passiveTrainParam.trackType = PASSIVE_TRAIN_SUB;
    ui->trainScope_StackedWidget->setCurrentIndex(0);
    loadOnGame();
}

void PassiveTraining::on_circleSubstract_btn_clicked()
{
    int circleNum = ui->maxCircle_lineEdit->text().toUInt();
    --circleNum;
    if(circleNum < 1)
        circleNum = 1;
    ui->maxCircle_lineEdit->setText(QString::number(circleNum));
}

void PassiveTraining::on_circleAdd_btn_clicked()
{
    int circleNum = ui->maxCircle_lineEdit->text().toUInt();
    ++circleNum;
    if(circleNum > 99)
        circleNum = 99;
    ui->maxCircle_lineEdit->setText(QString::number(circleNum));
}

void PassiveTraining::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    m_passiveTipsDialog->switchTipsType(PASSIVE_TRAIN_TIPS);
    m_passiveTipsDialog->show();
}

//提示窗口已确认
void PassiveTraining::slotConfirmed(E_VideoTipsType type)
{
    //将消息发送给下位机使其到达指定位置
}

//游戏状态发生变化
void PassiveTraining::slotGameStateChanged(E_TRAINMODE E_TrainMode,E_GameState e_gameState)
{
    if(E_TrainMode == TRAINMODE_PASSIVE_E)
        switch(e_gameState)
        {
        case GameStart_E://游戏启动后开启训练流程
            slotSendDestTimer();
            break;
        case GameEnd_E:
            stopTrain();
            break;
        }
}

void PassiveTraining::stopTrain()
{
    m_sendDestTimer->stop();
    //关闭陀螺仪数据
    CCommunicateAPI::getInstance()->switchEularData(false);
    Sleep(100);
    switchPassive(false);
    Sleep(500);
    isTraining = false;
}

void PassiveTraining::loadOnGame()
{
    QList<ST_GameMsg> gameList = GameControl::getInstance()->getGameMsgs();
    setGameList(gameList);
}

void PassiveTraining::setGameList(QList<ST_GameMsg>& gameList)
{
    ui->gameType_ListWidget->setSpacing(10);
    ui->gameType_ListWidget->setMovement(QListView::Static);
    ui->gameType_ListWidget->setViewMode(QListView::IconMode);
    ui->gameType_ListWidget->setIconSize(QSize(60,60));
    ui->gameType_ListWidget->setResizeMode(QListView::Adjust);

    ui->gameType_ListWidget->clear();

    connect(ui->gameType_ListWidget,SIGNAL(currentTextChanged(QString)),this,SLOT(slotGetCurrentText(QString)));

    for(int i = 0;i < gameList.size();i++)
    {
        if(gameList.at(i).suitTypeList.contains(QString::number(st_passiveTrainParam.trackType)))
        {
            QString pixPath("./gameExecutable/gameIcon/");
#ifdef ABSOLUTEPATH
            pixPath = QApplication::applicationDirPath() + "/gameExecutable/gameIcon/";
#endif
            pixPath.append(gameList.at(i).iconPath);
            QListWidgetItem *addItem = new QListWidgetItem(QIcon(pixPath),gameList.at(i).gameName);

            ui->gameType_ListWidget->addItem(addItem);
        }
    }
    if(ui->gameType_ListWidget->count() != 0)
    {
        ui->gameType_ListWidget->item(0)->setSelected(true);
        m_currentText = ui->gameType_ListWidget->item(0)->text();
    }
}

void PassiveTraining::slotGetCurrentText(QString text)
{
    m_currentText = text;
}

void PassiveTraining::slotSendDestTimer()
{
    //间隔时间
    int speed = 0;
    if(ui->level1_RadioButton->isChecked())
        speed = 1;
    if(ui->level2_RadioButton->isChecked())
        speed = 2;
    if(ui->level3_RadioButton->isChecked())
        speed = 3;
    if(ui->level4_RadioButton->isChecked())
        speed = 4;
    if(ui->level5_RadioButton->isChecked())
        speed = 5;
    if(ui->level6_RadioButton->isChecked())
        speed = 6;
    int angle = 0;
    if(ui->circle_RadioButton->isChecked())
        angle = ui->maxCircle_lineEdit->text().toUInt()*360*m_destDirection;
    else if(ui->subCircle_RadioButton->isChecked())
        angle = ui->maxAngle_lineEdit->text().remove("°").toUInt()*m_destDirection;

    ST_PassiveParam st_passiveParam;
    st_passiveParam.Angle = angle;
    st_passiveParam.speed = speed;

    sendPassivePosition(st_passiveParam);

}


