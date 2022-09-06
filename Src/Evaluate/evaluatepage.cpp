#include "evaluatepage.h"
#include "ui_evaluatepage.h"
#include "ccommunicateapi.h"
#include <QDebug>
#include <QPainter>
#include "cdatabaseinterface.h"
#include "currentuserdata.h"
//#pragma execution_character_set("utf-8")

EvaluatePage::EvaluatePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EvaluatePage),
    maxValue(0),
    minValue(0),
    maxCircleAngle(0),
    isEvaluating(false),
    basePitch(-5),
    isGetFirstData(false)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);//设置无边框
    connect(CCommunicateAPI::getInstance(),SIGNAL(signalReadyRead(QByteArray)),this,SLOT(receiveCommData(QByteArray)));
//    ui->check_Btn->setVisible(false);
    clearDisplayLable();

}

EvaluatePage::~EvaluatePage()
{
    delete ui;
}

void EvaluatePage::setEvaluateControlParam(ST_EvaluateControl st_evaluateParam)
{
    uint16_t cmdId = EVALUATE_CONTROL;
    uint8_t seq = 0;
    uint8_t id = 1;
    QByteArray array;
    array.resize(sizeof(ST_EvaluateControl));
    memcpy(array.data(),&st_evaluateParam,sizeof(ST_EvaluateControl));
    CCommunicateAPI::getInstance()->sendData(seq,id,cmdId,array);

}

void EvaluatePage::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    m_footIndex = CurrentUserData::getInstace()->getCurrentPatientMsg().footIndex;
}

//接收数据
void EvaluatePage::receiveCommData(QByteArray receiveData)
{
    if(isEvaluating)
    {
        int length = receiveData[1];
        QByteArray data = receiveData.mid(6,length);
        uint16_t comID;

        memcpy(&comID,receiveData.data()+4,2);

        switch(comID)
        {
        case GIVE_GYROSCOPE_DATA://给出陀螺仪数据
        {
            memcpy(&st_gyroscope,data.data(),sizeof(ST_GyroscopeData));
            displayGyroscopeData(st_gyroscope);
        }
            break;
        case ARRIVE_EVALUATE_POSITION:
        {
            qDebug()<<"到达评估位置";
        }
            break;
        }
    }
}

void EvaluatePage::on_startEvaluate_Btn_clicked()
{
    if(ui->startEvaluate_Btn->text() == tr("开始"))
    {
        isGetFirstData = true;
        ST_EvaluateControl st_evaluateControl;
        st_evaluateControl.evaluateType = evaluateTypeE;
        st_evaluateControl.switchEvaluate = 1;
        //开启评估指令
        setEvaluateControlParam(st_evaluateControl);
        Sleep(100);
        //打开陀螺仪数据
        CCommunicateAPI::getInstance()->switchEularData(true);
        ui->startEvaluate_Btn->setText(tr("完成"));
        isEvaluating = true;
        st_evaluaterecord.startTime = QDateTime::currentDateTime();
    }
    else if(ui->startEvaluate_Btn->text() == tr("完成"))
    {
        ui->startEvaluate_Btn->setText(tr("开始"));
        st_evaluaterecord.ID = CurrentUserData::getInstace()->getCurrentUserID();
        st_evaluaterecord.name = CurrentUserData::getInstace()->getCurrentPatientMsg().name;
        st_evaluaterecord.evaluateMode = evaluateTypeE;
        st_evaluaterecord.lowMargin = ui->minValue_Label->text().remove("°").toInt();
        st_evaluaterecord.highMargin = ui->maxValue_Label->text().remove("°").toInt();
        st_evaluaterecord.endTime = QDateTime::currentDateTime();
        isEvaluating = false;
        CCommunicateAPI::getInstance()->switchEularData(false);
        Sleep(100);
        //退出状态
        ST_EvaluateControl st_evaluateControl;
        st_evaluateControl.evaluateType = evaluateTypeE;
        st_evaluateControl.switchEvaluate = 0;
        setEvaluateControlParam(st_evaluateControl);

        clearDisplayLable();
        //存储工作
        setEvaluateTable(st_evaluaterecord);
        this->hide();
    }
}

void EvaluatePage::clearDisplayLable()
{
    QPixmap pixmap;
    QString fileName("");
    ui->display_Label->setPixmap(pixmap);
}

void EvaluatePage::switchEvaluatePage(E_TrainEvaluationType evaluateType)
{
    maxValue = 0;
    minValue = 0;
    maxCircleAngle = 0;
    evaluateTypeE = evaluateType;

    ui->minValue_Label->setText("0°");
    ui->maxValue_Label->setText("0°");
//    ui->check_Btn->setVisible(false);
    switch(evaluateType)
    {
    case COMPREHENSIVE_E: //综合(圆周)
    {
        ui->title_Label->setText(tr("综合评估"));
        ui->name1_Label->setText(tr("起始角度"));
        ui->name2_Label->setText(tr("当前角度"));
    }
        break;
    case PLANTAR_FLEXION_E://跖屈背屈
    {
        ui->title_Label->setText(tr("跖屈背屈评估"));
        ui->name1_Label->setText(tr("背屈角度"));
        ui->name2_Label->setText(tr("跖屈角度"));
    }
        break;
    case ADDUCTION_ABDUCTION_E://内收外展
    {
        ui->title_Label->setText(tr("内收外展评估"));
//        ui->check_Btn->setVisible(false);
        ui->name1_Label->setText(tr("内收角度"));
        ui->name2_Label->setText(tr("外展角度"));
    }
        break;
    case VARUS_VALGUS_E://内翻外翻
    {
        ui->title_Label->setText(tr("内翻外翻评估"));
        ui->name1_Label->setText(tr("内翻角度"));
        ui->name2_Label->setText(tr("外翻角度"));
    }
        break;
    case SLOPE_PLANTAR_FLEXION_E://斜跖屈背屈
    {
        ui->title_Label->setText(tr("斜跖屈背屈评估"));
        ui->name2_Label->setText(tr("斜跖屈角度"));
        ui->name1_Label->setText(tr("斜背屈角度"));
    }
        break;
    }
}

void EvaluatePage::displayGyroscopeData(const ST_GyroscopeData& gyroscopeData)
{

    switch(evaluateTypeE)
    {
    case PLANTAR_FLEXION_E://跖屈背屈 对应pitch(完成)
    {
        int pitch;

        if(gyroscopeData.st_euler.pitch < 0)//向上为正(背屈)
        {
            pitch = gyroscopeData.st_euler.pitch + 180-basePitch;
        }
        else
        {
            pitch = -1*(180 - gyroscopeData.st_euler.pitch+basePitch);
        }
        displayPlantarFlexion(pitch*(-1));
        if(pitch < 0)
        {
            if(maxValue < qAbs(pitch))
                maxValue = qAbs(pitch);
        }
        else
        {
            if(minValue < pitch)
                minValue = pitch;
        }
    }
        break;
    case ADDUCTION_ABDUCTION_E://内收外展 对应yaw
    {
        displayAdductionAbduction(gyroscopeData.st_euler.yaw*(-1));
        if(gyroscopeData.st_euler.yaw < 0)
        {
            if(minValue > gyroscopeData.st_euler.yaw)
                minValue = gyroscopeData.st_euler.yaw;
        }
        else
        {
            if(maxValue < gyroscopeData.st_euler.yaw)
                maxValue = gyroscopeData.st_euler.yaw;
        }
    }
        break;
    case VARUS_VALGUS_E://内翻外翻 对应roll
    {
        displayVarusValgus(gyroscopeData.st_euler.roll);
        if(gyroscopeData.st_euler.roll < 0)
        {
            if(minValue > gyroscopeData.st_euler.roll)
                minValue = gyroscopeData.st_euler.roll;
        }
        else
        {
            if(maxValue < gyroscopeData.st_euler.roll)
                maxValue = gyroscopeData.st_euler.roll;
        }
    }
        break;
    case SLOPE_PLANTAR_FLEXION_E://斜跖屈背屈
    {
        int pitch;
        if(gyroscopeData.st_euler.pitch < 0)//向上为正(背屈)
            pitch = gyroscopeData.st_euler.pitch + 180-basePitch;
        else
            pitch = -1*(180 - gyroscopeData.st_euler.pitch+basePitch);
        //图片显示
        displaySlopePlantarFlexion(pitch);

        if(pitch < 0)
        {
            if(maxValue < qAbs(pitch))
                maxValue = qAbs(pitch);
        }
        else
        {
            if(minValue < pitch)
                minValue = pitch;
        }
    }
        break;
    case COMPREHENSIVE_E:   //圆周角度
    {
        //显示图片
        displayComprehensive(gyroscopeData.coderAngle);
    }
        break;
    }

    if(evaluateTypeE !=COMPREHENSIVE_E)
    {
        ui->minValue_Label->setText(QString::number(maxValue)+"°");
        if(evaluateTypeE == PLANTAR_FLEXION_E)
            ui->maxValue_Label->setText(QString::number(minValue)+"°");
        else
            ui->maxValue_Label->setText(QString::number(minValue*(-1))+"°");
    }
}

void EvaluatePage::on_close_Btn_clicked()
{
    CCommunicateAPI::getInstance()->switchEularData(false);
    this->hide();
}

//圆周
void EvaluatePage::displayComprehensive(int angle)
{
    int tempAngel = 360 - angle;
    int targetAngle = 0;
    if(tempAngel >= 270)
    {
        targetAngle = qAbs(tempAngel - 270);

    }
    else if(tempAngel >=0 && tempAngel <= 90)
    {
        targetAngle = tempAngel + 90;
    }
    else if(tempAngel > 90 && tempAngel <270)
    {
        targetAngle = tempAngel + 90;
    }

    //显示角度数据
    if(evaluateTypeE == COMPREHENSIVE_E)  //综合(圆周)
    {
        if(isGetFirstData)
        {
            minValue = angle;
            isGetFirstData = false;
            //起始角度
            ui->minValue_Label->setText(QString::number(minValue)+"°");
        }
        //实时角度
        ui->maxValue_Label->setText(QString::number(targetAngle)+"°");
        //显示实时图片
        QPixmap pixmap;
        QString fileName;
        int index = (targetAngle-1)/18 + 1;
        if(m_footIndex == 0)    //右脚
            fileName = QString("./Source/evaluate/yuanzhou/%1.png").arg(index);
        else if(m_footIndex == 1)//左脚
        {

        }
        pixmap.load(fileName);
        ui->display_Label->setPixmap(pixmap);
    }
}
//跖屈背屈可视化
void EvaluatePage::displayPlantarFlexion(int angle)
{
    QPixmap pixmap;
    QString fileName;
    if(angle > 0)
    {
        int index = angle/5;
        if(m_footIndex == 0)    //右脚
            fileName = QString("./Source/zhiqubeiqu/%1.jpg").arg(index);
        else if(m_footIndex == 1)//左脚
        {}
    }
    else
    {
        int index = angle*(-1)/5;
        if(m_footIndex == 0)    //右脚
            fileName = QString("./Source/zhiqubeiqu/-%1.jpg").arg(index);
        else if(m_footIndex == 1)//左脚
        {}
    }
    pixmap.load(fileName);
    ui->display_Label->setPixmap(pixmap);
}
//内收外展可视化
void EvaluatePage::displayAdductionAbduction(float angle)
{
    QPixmap pixmap;
    QString fileName;
    if(angle > 0)
    {
        int index = angle/2.4 ;
        if(m_footIndex == 0)    //右脚
            fileName = QString("./Source/evaluate/NeishowWaizhan/%1.png").arg(index);
        else if(m_footIndex == 1)
        {}
    }
    else
    {
        int index = angle/2.4 ;
        if(m_footIndex == 0)    //右脚
            fileName = QString("./Source/evaluate/NeishowWaizhan/%1.png").arg(index);
        else if(m_footIndex == 1)
        {}
    }
    pixmap.load(fileName);
    ui->display_Label->setPixmap(pixmap);
}
//内翻外翻可视化
void EvaluatePage::displayVarusValgus(float angle)
{
    QPixmap pixmap;
    QString fileName;
    //添加范围限制
    if(qAbs(angle) >25)
        return;
    if(angle > 0)
    {
        int index = angle/2.4 ;
        if(m_footIndex == 0)
            fileName = QString("./Source/evaluate/NeifanWaifan/%1.png").arg(index);
        else if(m_footIndex == 1)
        {}
    }
    else
    {
        int index = angle/2.4;
        if(m_footIndex == 0)
            fileName = QString("./Source/evaluate/NeifanWaifan/%1.png").arg(index);
        else if(m_footIndex == 1)
        {}
    }
    pixmap.load(fileName);
    ui->display_Label->setPixmap(pixmap);
}
//斜跖屈背屈
void EvaluatePage::displaySlopePlantarFlexion(float angle)
{
    QPixmap pixmap;
    QString fileName;
    if(angle > 0)
    {
        int index = angle/1.2;
        if(index > 10)
            index = 10;
        if(m_footIndex == 0)
            fileName = QString("./Source/evaluate/XiezhiquBeiqu1/%1.png").arg(index);
        else if(m_footIndex == 1)
        {}
    }
    else
    {
        int index = angle/1.2;
        if(index < -10)
            index = -10;
        if(m_footIndex == 0)
            fileName = QString("./Source/evaluate/XiezhiquBeiqu1/%1.png").arg(index);
        else if(m_footIndex == 1)
        {}
    }
    pixmap.load(fileName);
    ui->display_Label->setPixmap(pixmap);
}

void EvaluatePage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(),QColor(0,0,0,100));
}

void EvaluatePage::on_check_Btn_clicked()
{
    CCommunicateAPI::getInstance()->TLYCheck();
    minValue = 0;
    maxValue = 0;
}

void EvaluatePage::setEvaluateTable(ST_EvaluateRecord st_evaluaterecord)
{
    QString tableName("EvaluateRecord");
    QVariantMap vMap = evaluateRecordToVariantMap(st_evaluaterecord);
    if(!CDatabaseInterface::getInstance()->insertRowTable(tableName,vMap))
    {
        qDebug()<<CDatabaseInterface::getInstance()->getLastError();
        //数据插入成功
    }
}
