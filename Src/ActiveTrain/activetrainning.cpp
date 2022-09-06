#include "activetrainning.h"
#include "ui_activetrainning.h"
#include <QButtonGroup>
#include "ccommunicateapi.h"
#include "switchactivepassivewidget.h"
#include "gamecontrol.h"
#include <QtEndian>
#include <QTimer>
#pragma execution_character_set("utf-8")

ActiveTrainning::ActiveTrainning(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActiveTrainning),
    m_activetrainreport(NULL),
    m_activeTipsDialog(NULL),
    isTraining(false),
    buttonGroup(NULL),
    isUseRealData(false),
    basePitch(-5)

{
    ui->setupUi(this);
    buttonGroup = new QButtonGroup();

    buttonGroup->addButton(ui->plantarFlexion_RadioButton,1);
    buttonGroup->addButton(ui->adductionAbduction_RadioButton,2);
    buttonGroup->addButton(ui->varusValgus_RadioButton,3);
    buttonGroup->addButton(ui->comprehensive_RadioButton,0);
    buttonGroup->addButton(ui->slopePlantar_RadioButton,4);
    m_activetrainreport=new ActiveTrainingReport;
    connect(CCommunicateAPI::getInstance(),SIGNAL(signalReadyRead(QByteArray)),this,SLOT(receiveCommData(QByteArray)));
    connect(buttonGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotButtonGoupClicked(int)));

    m_activeTipsDialog = new SwitchActivePassiveWidget();
    connect(m_activeTipsDialog,SIGNAL(signalConfirmed(E_VideoTipsType)),this,SLOT(slotConfirmed(E_VideoTipsType)));
    connect(GameControl::getInstance(),SIGNAL(signalGameState(E_TRAINMODE,E_GameState)),this,SLOT(slotGameStateChanged(E_TRAINMODE,E_GameState)));

    evaluateTypeE = PLANTAR_FLEXION_E;
    loadOnGame();

    //将内番外翻进行阉割
    ui->label_11->setVisible(false);
    ui->label_15->setVisible(false);
    ui->adductionAbduction_RadioButton->setVisible(false);
    ui->trainScope_GroupBox->setVisible(false);

    ui->tips_Label->setText(tr("将滑块移动至中间位置，并将锁止销解锁，将离合器掰至被动位置！"));
}

ActiveTrainning::~ActiveTrainning()
{
    if(m_activetrainreport)
        delete m_activetrainreport;
    if(m_activeTipsDialog)
        delete m_activeTipsDialog;
    if(buttonGroup)
        delete buttonGroup;
    delete ui;
}

void ActiveTrainning::slotButtonGoupClicked(int id)
{
    evaluateTypeE = (E_TrainEvaluationType)id;
    //刷新游戏列表
    loadOnGame();
}

void ActiveTrainning::switchActive(bool isOn)
{
    uint16_t cmdId = SWITCH_ACTIVE_TRAINNING;
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
//发送主动训练参数
void ActiveTrainning::on_startTrain_Btn_clicked()
{
    if(!isTraining)
    {
        //首先校准陀螺仪
        isTraining = true;
        //告知下位机开启主动训练
        switchActive(true);
        Sleep(100);
        switchGyroscope(true);
        //设置主动训练类型
        Sleep(100);
        setActiveType();
        //告知游戏训练参数
        int time = ui->time_lineEdit->text().toInt() *60;

        ST_ActiveTrainParam st_activeParam;
        st_activeParam.trainTime = time;
        st_activeParam.trackType = evaluateTypeE;
        st_activeParam.minAngle = ui->leftMaxAngle_lineEdit->text().remove("°").toUInt();
        st_activeParam.maxAngle = ui->rightMaxAngle_lineEdit->text().remove("°").toUInt();
        st_activeParam.gameName = m_currentText;

        m_minAngle = st_activeParam.minAngle;
        m_maxAngle = st_activeParam.maxAngle;
        GameControl::getInstance()->setActiveParam(st_activeParam);

        ST_GameMsg st_game = GameControl::getInstance()->getGameMsgByName(m_currentText);
        GameControl::getInstance()->startGame(st_game.gamePath);
    }
}

void ActiveTrainning::on_timeAdd_btn_clicked()
{
    int trainMaxTime  = ui->time_lineEdit->text().toInt();
    trainMaxTime++;
    if(trainMaxTime > 30)
        trainMaxTime = 30;
    ui->time_lineEdit->setText(QString::number(trainMaxTime));
}

void ActiveTrainning::on_timeSubtract_btn_clicked()
{
    int time = ui->time_lineEdit->text().toInt();
    time--;
    if(time < 1)
        time = 1;
    ui->time_lineEdit->setText(QString::number(time));
}
//接收数据
void ActiveTrainning::receiveCommData(QByteArray receiveData)
{
    if(isTraining)
    {
        int length = receiveData[1];
        QByteArray data = receiveData.mid(6,length);
        uint16_t comID;
        memcpy(&comID,receiveData.data()+4,2);
        switch(comID)
        {
        case GIVE_GYROSCOPE_DATA://给出陀螺仪数据
        {
            //此处要小于60Hz，否则游戏接收会出现问题
            memcpy(&st_gyroscope,data.data(),sizeof(ST_GyroscopeData));
            displayGyroscopeData(st_gyroscope);
            if(COMPREHENSIVE_E == evaluateTypeE )
                GameControl::getInstance()->sendCurrentAngleToGame(360-st_gyroscope.coderAngle,0,360);
        }
            break;
        case ARRIVED_ACTIVE_POSITION://到达主动训练位置
        {
            //到达主动训练位置，开启游戏
            qDebug()<<"到达主动训练位置";
        }
            break;
        }
    }
}

void ActiveTrainning::displayGyroscopeData(const ST_GyroscopeData& gyroscopeData)
{
    switch(evaluateTypeE)
    {
    case PLANTAR_FLEXION_E://跖屈背屈 对应pitch
    {
        int pitch;
        if(gyroscopeData.st_euler.pitch < 0)//向上为正(背屈)
        {
            pitch = -1*(gyroscopeData.st_euler.pitch + 180 -basePitch);
        }
        else
        {
            pitch = (180 - gyroscopeData.st_euler.pitch + basePitch);
        }
        if(!isUseRealData)
            GameControl::getInstance()->sendCurrentAngleToGame(pitch,-20,20);
        else
            GameControl::getInstance()->sendCurrentAngleToGame(pitch,m_minAngle*(-1),m_maxAngle);
    }
        break;
    case ADDUCTION_ABDUCTION_E://内收外展 对应yaw
    {
        if(!isUseRealData)
            GameControl::getInstance()->sendCurrentAngleToGame(gyroscopeData.st_euler.yaw*(-1),-20,20);
        else
            GameControl::getInstance()->sendCurrentAngleToGame(gyroscopeData.st_euler.yaw*(-1),m_minAngle*(-1),m_maxAngle);
    }
        break;
    case VARUS_VALGUS_E://内翻外翻 对应roll
    {
        if(!isUseRealData)
            GameControl::getInstance()->sendCurrentAngleToGame(gyroscopeData.st_euler.roll,-20,20);
        else
            GameControl::getInstance()->sendCurrentAngleToGame(gyroscopeData.st_euler.roll,m_minAngle*(-1),m_maxAngle);
    }
        break;
    case SLOPE_PLANTAR_FLEXION_E://斜跖屈背屈
    {
        int pitch;
        if(gyroscopeData.st_euler.pitch < 0)//向上为正(背屈)
        {
            pitch = (gyroscopeData.st_euler.pitch + 180 -basePitch);
            if(m_currentText != "小黄鸭")
                pitch*=(-1);
        }
        else
        {
            pitch = (-1)*(180 - gyroscopeData.st_euler.pitch + basePitch);
            if(m_currentText != "小黄鸭")
                pitch*=(-1);

        }
        if(!isUseRealData)
            GameControl::getInstance()->sendCurrentAngleToGame(pitch,-15,15);
        else
            GameControl::getInstance()->sendCurrentAngleToGame(pitch,m_minAngle*(-1),m_maxAngle);
    }
        break;
    }
}
//获取陀螺仪数据开关
void ActiveTrainning::switchGyroscope(bool isOn)
{
    uint16_t cmdId = GET_GYROSCOPE_DATA;
    uint8_t seq = 0;
    uint8_t id = 1;
    QByteArray array;
    array.resize(sizeof(uint8_t));
    uint8_t flag;
    if(isOn)
        flag = 1;
    else
        flag = 2;
    memcpy(array.data(),&flag,sizeof(flag));
    CCommunicateAPI::getInstance()->sendData(seq,id,cmdId,array);
}

//设置主动训练类型
void ActiveTrainning::setActiveType()
{
    uint16_t cmdId = SET_ACTIVE_TRAINNING_TYPE;
    uint8_t seq = 0;
    uint8_t id = 1;
    QByteArray array;
    uint8_t E_TrainEvaluationType;
    array.resize(sizeof(uint8_t));

    if(ui->adductionAbduction_RadioButton->isChecked()) //内收外展
    {
        E_TrainEvaluationType = ADDUCTION_ABDUCTION_E;
    }
    else if(ui->comprehensive_RadioButton->isChecked()) //综合（圆周）
    {
        E_TrainEvaluationType = COMPREHENSIVE_E;
    }
    else if(ui->plantarFlexion_RadioButton->isChecked()) //跖屈背屈
    {
        E_TrainEvaluationType = PLANTAR_FLEXION_E;
    }
    else if(ui->varusValgus_RadioButton->isChecked()) //内翻外翻
    {
        E_TrainEvaluationType = VARUS_VALGUS_E;
    }
    else if(ui->slopePlantar_RadioButton->isChecked()) //斜跖屈背屈
    {
        E_TrainEvaluationType = SLOPE_PLANTAR_FLEXION_E;
    }
    memcpy(array.data(),&E_TrainEvaluationType,sizeof(E_TrainEvaluationType));

    CCommunicateAPI::getInstance()->sendData(seq,id,cmdId,array);
}

void ActiveTrainning::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    m_activeTipsDialog->switchTipsType(ACTIVE_TRAIN_TIPS);
    m_activeTipsDialog->show();
}

//提示窗口已确认
void ActiveTrainning::slotConfirmed(E_VideoTipsType type)
{
    //将消息发送给下位机使其到达指定位置
}

void ActiveTrainning::on_leftAngleSubtract_btn_clicked()
{
    int angle = ui->leftMaxAngle_lineEdit->text().remove("°").toInt();
    angle--;
    if(angle < 0)
        angle = 0;
    ui->leftMaxAngle_lineEdit->setText(QString::number(angle)+"°");
}

void ActiveTrainning::on_leftAngleAdd_btn_clicked()
{
    int angle = ui->leftMaxAngle_lineEdit->text().remove("°").toInt();
    angle++;
    if(angle >25)
        angle = 25;
    if(angle > 0)
        ui->leftMaxAngle_lineEdit->setText(QString::number(angle)+"°");
}

void ActiveTrainning::on_rightAngleSubtract_btn_clicked()
{
    int angle = ui->rightMaxAngle_lineEdit->text().remove("°").toInt();
    angle--;
    if(angle < 0)
        angle = 0;
    ui->rightMaxAngle_lineEdit->setText(QString::number(angle) + "°");
}

void ActiveTrainning::on_rightAngleAdd_btn_clicked()
{
    int angle = ui->rightMaxAngle_lineEdit->text().remove("°").toInt();
    angle++;
    if(angle >25)
        angle = 25;
    if(angle > 0)
        ui->rightMaxAngle_lineEdit->setText(QString::number(angle)+"°");
}

//跖屈背屈
void ActiveTrainning::on_plantarFlexion_RadioButton_clicked()
{
    setScopeDisplay(true);
    ui->left_Label->setText(tr("趾屈"));
    ui->right_Label->setText(tr("背屈"));

    ui->tips_Label->setText(tr("将滑块移动至中间位置，并将锁止销解锁，将离合器掰至被动位置！"));
}
//内收外展
void ActiveTrainning::on_adductionAbduction_RadioButton_clicked()
{
    setScopeDisplay(true);
    ui->left_Label->setText(tr("内收"));
    ui->right_Label->setText(tr("外展"));
}
//内翻外翻
void ActiveTrainning::on_varusValgus_RadioButton_clicked()
{
    setScopeDisplay(true);
    ui->left_Label->setText(tr("内翻"));
    ui->right_Label->setText(tr("外翻"));
    ui->tips_Label->setText(tr("将滑块移动至中间位置，并将锁定锁止销，将离合器掰至被动位置！"));
}
//综合(圆周)
void ActiveTrainning::on_comprehensive_RadioButton_clicked()
{
    //隐藏掉范围显示
    setScopeDisplay(false);
    ui->tips_Label->setText(tr("将滑块移动边缘，并将锁定锁止销，将离合器掰至主动位置！"));
}
//斜跖屈背屈
void ActiveTrainning::on_slopePlantar_RadioButton_clicked()
{
    setScopeDisplay(true);
    ui->left_Label->setText(tr("斜跖屈"));
    ui->right_Label->setText(tr("斜背屈"));
    ui->tips_Label->setText(tr("将滑块移动至中间位置，并将锁止销解锁，将离合器掰至被动位置！"));
}

void ActiveTrainning::setScopeDisplay(bool isShow)
{
    ui->minValue_GroupBox->setVisible(isShow);
    ui->maxValue_GroupBox->setVisible(isShow);
}

void ActiveTrainning::setGameList(QList<ST_GameMsg>& gameList)
{
    ui->gameType_ListWidget->setSpacing(10);
    ui->gameType_ListWidget->setMovement(QListView::Static);
    ui->gameType_ListWidget->setViewMode(QListView::IconMode);
    ui->gameType_ListWidget->setIconSize(QSize(60,60));
    ui->gameType_ListWidget->setResizeMode(QListView::Adjust);

    connect(ui->gameType_ListWidget,SIGNAL(currentTextChanged(QString)),this,SLOT(slotGetCurrentText(QString)));

    ui->gameType_ListWidget->clear();

    for(int i = 0;i < gameList.size();i++)
    {
        if(COMPREHENSIVE_E != evaluateTypeE)
        {
            if(gameList.at(i).suitTypeList.contains("2"))
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
        else if(COMPREHENSIVE_E == evaluateTypeE )
        {
            if(gameList.at(i).suitTypeList.contains("5") || gameList.at(i).suitTypeList.contains("3"))
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

    }
    if(ui->gameType_ListWidget->count() != 0)
    {
        ui->gameType_ListWidget->item(0)->setSelected(true);
        m_currentText = ui->gameType_ListWidget->item(0)->text();
    }
}

void ActiveTrainning::slotGetCurrentText(QString text)
{
    m_currentText = text;
}

void ActiveTrainning::loadOnGame()
{
    QList<ST_GameMsg> gameList = GameControl::getInstance()->getGameMsgs();
    setGameList(gameList);
}

void ActiveTrainning::stopTrain()
{
    switchGyroscope(false);
    Sleep(100);
    switchActive(false);
    Sleep(100);
    isTraining = false;
}

//游戏状态发生变化
void ActiveTrainning::slotGameStateChanged(E_TRAINMODE E_TrainMode,E_GameState e_gameState)
{
    if(E_TrainMode == TRAINMODE_ACTIVE_E)
    switch(e_gameState)
    {
    case GameStart_E:
        qDebug()<<"开启游戏";
        break;
    case GameEnd_E:
        stopTrain();
        break;
    }
}


