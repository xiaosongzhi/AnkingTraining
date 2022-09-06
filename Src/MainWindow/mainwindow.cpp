#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include "logger.h"
#include <QButtonGroup>
#include <QAbstractButton>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include "userdataformate.h"
#include "mainwindowpagecontrol.h"
#include "ccommunicateapi.h"
#include <QDebug>
#include <QMapIterator>
#include "resetwidget.h"
#include "currentuserdata.h"

//#pragma execution_character_set("utf-8")

//LOG4QT_DECLARE_STATIC_LOGGER(logger, MainWindow)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_buttonGroup(NULL)
    , m_userManager(NULL)
    , m_emerTimer(NULL)
    , m_resetTimer(NULL)
{
    ui->setupUi(this);
    m_userManager=new UserManager;
    setAttribute(Qt::WA_QuitOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint);
    connect(ui->stackedWidget,SIGNAL(currentChanged(int)),this,SLOT(slotCurrentStackWidgetChanged(int)));
    //管理用户初始化

    //初始化按钮组
    initButtonGroup();
    initWidgetUI();

    ui->stackedWidget->setCurrentIndex(2);
    m_emergencyLabel = new QLabel(this);
    m_emergencyLabel->move(1550,70);
    m_emergencyLabel->resize(120,120);
    m_emergencyLabel->setWindowFlags(Qt::FramelessWindowHint);
    QPixmap pixmap("./source/dialog/emergency.png");
    m_emergencyLabel->setPixmap(pixmap);
    m_emergencyLabel->hide();
    m_emerTimer = new QTimer();
    m_emerTimer->setInterval(500);
    connect(m_emerTimer,SIGNAL(timeout()),this,SLOT(slotEmergencyBlink()));
    connect(CurrentUserData::getInstace(),SIGNAL(signalUserChanged()),this,SLOT(slotCurrentUserChanged()));
    //切换页面
    connect(MainWindowPageControl::getInstance(),SIGNAL(signalSwitchPage(int)),this,SLOT(slotSwitchPage(int)));
    slotBtnGroupClicked(2);
    connect(CCommunicateAPI::getInstance(),SIGNAL(signalReadyRead(QByteArray)),this,SLOT(receiveCommData(QByteArray)));

    m_resetTimer = new QTimer();
    connect(m_resetTimer,SIGNAL(timeout()),this,SLOT(slotResetDialog()));
    m_resetTimer->setInterval(50);
    m_resetTimer->setSingleShot(true);
    m_resetTimer->start();
}

MainWindow::~MainWindow()
{
    if(m_emerTimer)
    {
        m_emerTimer->stop();
        delete  m_emerTimer;
        m_emerTimer = NULL;
    }
    if(m_resetTimer)
        delete m_resetTimer;
    if(m_userManager)
        delete m_userManager;

    delete ui;

    if(m_buttonGroup)
        delete m_buttonGroup;
}

void MainWindow::initButtonGroup()
{
    m_buttonGroup = new QButtonGroup();
    //设置互斥开启
    QPair<QString,QString> qssBtn;
    m_buttonGroup->setExclusive(true);

    m_buttonGroup->addButton(ui->passiveTrain_Btn,0);
    qssBtn.first = "border-image:url(:/images/button/passiveSelect.png);";
    qssBtn.second = "border-image:url(:/images/button/passiveNoSelect.png);";
    m_btnQssList.insert(0,qssBtn);

    m_buttonGroup->addButton(ui->activeTrain_Btn,1);
    qssBtn.first = "border-image:url(:/images/button/activiteSelect.png);";
    qssBtn.second = "border-image:url(:/images/button/activiteNoSelect.png);";
    m_btnQssList.insert(1,qssBtn);

    m_buttonGroup->addButton(ui->evaluate_Btn,2);
    qssBtn.first = "border-image:url(:/images/button/evaluteSelect.png);";
    qssBtn.second = "border-image:url(:/images/button/evaluteNoSelect.png);";
    m_btnQssList.insert(2,qssBtn);

    m_buttonGroup->addButton(ui->record_Btn,3);
    qssBtn.first = "border-image:url(:/images/button/historySelect.png);";
    qssBtn.second = "border-image:url(:/images/button/historyNoSelect.png);";
    m_btnQssList.insert(3,qssBtn);

    m_buttonGroup->addButton(ui->setting_Btn,4);
    qssBtn.first = "border-image:url(:/images/button/setSelect.png);";
    qssBtn.second = "border-image:url(:/images/button/setNoSelect.png);";
    m_btnQssList.insert(4,qssBtn);

    m_buttonGroup->addButton(ui->shutDown_Btn,5);
    qssBtn.first = "border-image:url(:/images/button/quitSelect.png);";
    qssBtn.second = "border-image:url(:/images/button/quitNoSelect.png);";
    m_btnQssList.insert(5,qssBtn);

    connect(m_buttonGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnGroupClicked(int)));
}

void MainWindow::initWidgetUI()
{
    QGraphicsDropShadowEffect * shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setYOffset(2);
    shadowEffect->setBlurRadius(12);
    shadowEffect->setColor(QColor(0,0,0,51));
    ui->title_Widget->setGraphicsEffect(shadowEffect);
    //提升层级 提升控件的Z值
    ui->title_Widget->raise();
}

void MainWindow::slotBtnGroupClicked(int btnId)
{
    //设置当前按下的按钮为选中状态，其他的为未选中状态
    for(int i = 0;i < m_buttonGroup->buttons().size();i++)
    {
        QPushButton *Btn = qobject_cast<QPushButton*>(m_buttonGroup->button(i));
        if(i == btnId)
            Btn->setStyleSheet(m_btnQssList.at(i).first);
        else
            Btn->setStyleSheet(m_btnQssList.at(i).second);
    }
}

void MainWindow::slotCurrentStackWidgetChanged(int index)
{


}

void MainWindow::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
}

void MainWindow::slotSwitchPage(int index)
{
    switch(index)
    {
    case 0: //ActiveTrainingPage_E
        ui->stackedWidget->setCurrentIndex(ActiveTrainingPage_E);
        break;
    case 1: //PassiveTrainingPage_E
        ui->stackedWidget->setCurrentIndex(PassiveTrainingPage_E);
        break;
    case 2: //EvaluationPage_E
        ui->stackedWidget->setCurrentIndex(EvaluationPage_E);
        break;
    case 3: //RecordPage_E
        ui->stackedWidget->setCurrentIndex(RecordPage_E);
        break;
    case 4: //SettingPage_E
        ui->stackedWidget->setCurrentIndex(SettingPage_E);
        break;
    }

    slotBtnGroupClicked(index);
}

//切换到用户管理界面
void MainWindow::on_switchUser_Btn_clicked()
{
    m_userManager->show();
}

void MainWindow::slotCurrentUserChanged()
{
    setCurrentUser();
}

//更新当前用户信息
void MainWindow::setCurrentUser()
{
    ST_PatientMsg st_PatientMsg = CurrentUserData::getInstace()->getCurrentPatientMsg();
    QString pixPath;
    if(0 == st_PatientMsg.sex)//male
        pixPath = ":/images/photo_man.png";
    else if(1 == st_PatientMsg.sex)//female
        pixPath = ":/images/photo_woman.png";
    QPixmap pix(pixPath);
    ui->portrait_Label->setPixmap(pix);

    ui->ID_Label->setText(QString("ID号:%1").arg(st_PatientMsg.ID));
    ui->name_Label->setText(QString("姓名:%1").arg(st_PatientMsg.name));
}

void MainWindow::enterUserManagerPage()
{
    on_switchUser_Btn_clicked();
}

//显示急停闪烁效果
void MainWindow::showEmergency(bool isShow)
{
    if(isShow)
    {
        m_emerTimer->start();
        m_emergencyLabel->show();
    }
    else
    {
        m_emerTimer->stop();
        m_emergencyLabel->hide();
    }
}

//显示急停效果槽
void MainWindow::slotShowEmergency(bool isShow)
{
    showEmergency(isShow);
}

void MainWindow::slotEmergencyBlink()
{
    if(m_emergencyLabel->isVisible())
        m_emergencyLabel->hide();
    else
        m_emergencyLabel->show();
}

void MainWindow::on_passiveTrain_Btn_clicked()
{
    MainWindowPageControl::getInstance()->setCurrentPage(PassiveTrainingPage_E);
}

void MainWindow::on_activeTrain_Btn_clicked()
{
    MainWindowPageControl::getInstance()->setCurrentPage(ActiveTrainingPage_E);
}

void MainWindow::on_evaluate_Btn_clicked()
{
    MainWindowPageControl::getInstance()->setCurrentPage(EvaluationPage_E);
}

void MainWindow::on_record_Btn_clicked()
{
    MainWindowPageControl::getInstance()->setCurrentPage(RecordPage_E);
}

void MainWindow::on_setting_Btn_clicked()
{
    MainWindowPageControl::getInstance()->setCurrentPage(SettingPage_E);
}

void MainWindow::on_shutDown_Btn_clicked()
{
    this->close();
}

void MainWindow::receiveCommData(QByteArray receiveData)
{
    int length = receiveData[1];
    QByteArray data = receiveData.mid(6,length);
    uint16_t comID;

    memcpy(&comID,receiveData.data()+4,2);

    switch(comID)
    {
    case ERROR_SPASMWARNING:  //痉挛报警
    {
        int8_t spasmwarning;
        memcpy(&spasmwarning,data.data(),sizeof(int8_t));
    }
        break;

    case ERROR_DEVICEERROR:  //设备报警
    {
        int8_t deviceerror;
        memcpy(&deviceerror,data.data(),sizeof(int8_t));
    }
        break;
    }
}

void MainWindow::slotResetDialog()
{
    on_switchUser_Btn_clicked();
}
//心跳槽函数
void MainWindow::slotHeartBeatSlot()
{
    CCommunicateAPI::getInstance()->sendHeartBeat();
}


