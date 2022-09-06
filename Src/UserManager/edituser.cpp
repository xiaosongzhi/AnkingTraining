#include "edituser.h"
#include "ui_edituser.h"
#include <qvalidator.h>
#include "cdatabaseinterface.h"
#include <QDebug>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QIntValidator>
#include <QSettings>
#include <QApplication>
#include <QDir>
#include "controlmaskdialog.h"
#include "currentuserdata.h"
#include <QMessageBox>
#include "cpassworddialog.h"
#include "mainwindowpagecontrol.h"
#include "resetwidget.h"
#pragma execution_character_set("utf-8")

EditUser::EditUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditUser)
  , m_resetWidget(NULL)
  , m_footIndex(0)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(/*Qt::WindowStaysOnTopHint |*/ Qt::FramelessWindowHint);//设置无边框
    setAttribute(Qt::WA_TranslucentBackground,true);//设置透明
    setLineEditFormat();
    initWidgetUI();
    m_resetWidget = new ResetWidget();
//    connect(this,SIGNAL(signalCloseEvent()),this,SLOT(slotCloseBtn()));

    ui->userInfo_LineEdit->setReadOnly(true);
    on_leftSide_Btn_clicked();
}

EditUser::~EditUser()
{
    if(m_resetWidget)
        delete m_resetWidget;
    delete ui;
}

//设置输入限制
void EditUser::setLineEditFormat()
{
    //名字
    //ID号 由系统分配，全局唯一
    //年龄、身高、体重
    QIntValidator *pIntValidator = new QIntValidator(this);
    pIntValidator->setRange(1, 150);    //年龄
    ui->age_LineEdit->setValidator(pIntValidator);
    pIntValidator->setRange(1, 250);    //身高
    ui->height_LineEdit->setValidator(pIntValidator);
    pIntValidator->setRange(1, 300);
    ui->weight_LineEdit->setValidator(pIntValidator);
    //电话
    QRegExp reg("[0-9]+$");
    QRegExpValidator *pValidator = new QRegExpValidator(this);
    pValidator->setRegExp(reg);
    ui->userTel_LineEdit->setMaxLength(11);
    ui->userInfo_LineEdit->setMaxLength(10);
    ui->userInfo_LineEdit->setValidator(pValidator);
    //家庭住址
    ui->address_LineEdit->setMaxLength(30);
    //备注信息
    ui->remark_LineEdit->setMaxLength(100);
    //证件号
    QRegExp regNum("[a-zA-Z0-9]+$");
    pValidator->setRegExp(regNum);

}

void EditUser::switchPageType(QString ID,int type)
{
    switch(type)
    {
    case 0:     //添加新用户
        addUserPage();
        setTitile(tr("新建用户"));
        break;
    case 1:     //编辑用户
        editUserPage(ID);
        setTitile(tr("编辑用户"));
        break;
    default:
        break;
    }
}

void EditUser::addUserPage()
{
    ui->delete_Btn->setVisible(false);
    ui->modify_Btn->setVisible(false);
    ui->confirm_Btn->setVisible(false);
    ui->save_Btn->setVisible(true);
    clearUserData();
    //默认为男性
    on_male_RadioButton_clicked();
    //读取当前用户ID
    readSetting();
    ui->ID_LineEdit->setText(QString::number(m_currentID+1));

}
void EditUser::editUserPage(QString ID)
{
    ui->delete_Btn->setVisible(true);
    ui->modify_Btn->setVisible(true);
    ui->confirm_Btn->setVisible(true);
    ui->save_Btn->setVisible(false);

    QString query = QString("select * from PatientMsg where ID = '%1'").arg(ID);

    if(CDatabaseInterface::getInstance()->exec(query))
    {
        if(int count = CDatabaseInterface::getInstance()->getValuesSize() > 0)
        {
            QList<QVariantMap>valueList = CDatabaseInterface::getInstance()->getValues(0,count);
            ST_PatientMsg st_patientMsg = variantMapToPatientMsg(valueList.at(0));
            setUserData(st_patientMsg);
        }
    }
    else
    {
        qDebug()<<CDatabaseInterface::getInstance()->getLastError();
        QMessageBox::warning(NULL,tr("警告"),tr("未找到该用户"));
    }
}

void EditUser::on_delete_Btn_clicked()
{
    CPasswordDialog m_dialog;
    int result = m_dialog.exec();
    if(result != 3)
        return;
    //从数据库中将该患者信息删除
    QString ID = ui->ID_LineEdit->text();
    QString dataBaseName("PatientMsg");
    if(!CDatabaseInterface::getInstance()->deleteRowTable(dataBaseName,"ID",ID))
    {
        qDebug()<<"删除失败"<<CDatabaseInterface::getInstance()->getLastError();
        emit signalUserSelected(0);
        return;
    }
    this->close();
    //返回用户管理界面
    emit signalUserSelected(0);
    //更新用户列表
    emit signalUpdateUserList();
}

void EditUser::on_modify_Btn_clicked()
{
    CPasswordDialog m_dialog;
    int result = m_dialog.exec();
    if(result != 3)
        return;
    //从数据库中将该患者信息进行更新
    QString tableName("PatientMsg");
    QVariantMap vMap = patientMsgToVariantMap(getUserData());
    if(!CDatabaseInterface::getInstance()->updateRowTable(tableName,"ID",vMap))
    {
        qDebug()<<CDatabaseInterface::getInstance()->getLastError();
        //进入用户管理界面
        emit signalUserSelected(0);
    }
    //返回用户管理界面
    emit signalUserSelected(0);

    //更新当前用户数据
    emit signalUpdateUserList();

    this->close();
}

void EditUser::on_confirm_Btn_clicked()
{
    //更新当前用户数据
    //将该用户添加到数据库中
    ST_PatientMsg st_PatientMsg;
    //此处需要查询数据库ID

    st_PatientMsg.ID = ui->ID_LineEdit->text().toInt();
    st_PatientMsg.name = ui->name_LineEdit->text();
    if(st_PatientMsg.name.isEmpty())
    {
        QMessageBox::warning(NULL,tr("提示"),tr("姓名不能为空！"));
        return;
    }
    st_PatientMsg.age = ui->age_LineEdit->text().toInt();
    if(ui->male_RadioButton->isChecked())
        st_PatientMsg.sex = 0;
    else if(ui->female_RadioButton->isChecked())
        st_PatientMsg.sex = 1;
    st_PatientMsg.height = ui->height_LineEdit->text().toInt();
    st_PatientMsg.weight = ui->weight_LineEdit->text().toInt();
    st_PatientMsg.phone = ui->userTel_LineEdit->text();
    st_PatientMsg.address = ui->address_LineEdit->text();
    st_PatientMsg.remarkMessage = ui->remark_LineEdit->text();
    st_PatientMsg.userInfo = ui->userInfo_LineEdit->text();
    st_PatientMsg.footIndex = m_footIndex;

    //更新当前用户信息
    CurrentUserData::getInstace()->setCurrentUserMsg(st_PatientMsg);
    this->close();

    //进入评估界面
    MainWindowPageControl::getInstance()->setCurrentPage(EvaluationPage_E);
    m_resetWidget->show();
}

void EditUser::on_save_Btn_clicked()
{
    ST_PatientMsg st_PatientMsg;
    st_PatientMsg.ID=ui->ID_LineEdit->text().toInt();
    st_PatientMsg.name = ui->name_LineEdit->text();
    if(st_PatientMsg.name.isEmpty())
    {
        QMessageBox::warning(NULL,tr("提示"),tr("姓名不能为空！"));
        return;
    }
    st_PatientMsg.age = ui->age_LineEdit->text().toInt();
    if(ui->male_RadioButton->isChecked())
        st_PatientMsg.sex = 0;
    else if(ui->female_RadioButton->isChecked())
        st_PatientMsg.sex = 1;
    st_PatientMsg.height = ui->height_LineEdit->text().toInt();
    st_PatientMsg.weight = ui->weight_LineEdit->text().toInt();
    st_PatientMsg.phone = ui->userTel_LineEdit->text();
    st_PatientMsg.userInfo=ui->userInfo_LineEdit->text().toInt();
    st_PatientMsg.address = ui->address_LineEdit->text();
    st_PatientMsg.remarkMessage = ui->remark_LineEdit->text();
    st_PatientMsg.userInfo=ui->userInfo_LineEdit->text();
    st_PatientMsg.footIndex = m_footIndex;

    writeSetting();
    //将用户数据存库
    setUserTable(st_PatientMsg);
    CurrentUserData::getInstace()->setCurrentUserMsg(st_PatientMsg);

    this->close();
    MainWindowPageControl::getInstance()->setCurrentPage(EvaluationPage_E);
    m_resetWidget->show();
}

//设置用户信息
void EditUser::setUserData(ST_PatientMsg st_PatientMsg)
{
    ui->ID_LineEdit->setText(QString::number(st_PatientMsg.ID));
    ui->name_LineEdit->setText(st_PatientMsg.name);
    ui->age_LineEdit->setText(QString::number(st_PatientMsg.age));

    QString pixPath;

    if(0 == st_PatientMsg.sex)
    {
        ui->male_RadioButton->setChecked(true);
        ui->female_RadioButton->setChecked(false);
        pixPath = ":/images/photo_man.png";
    }
    else
    {
        ui->male_RadioButton->setChecked(false);
        ui->female_RadioButton->setChecked(true);
        pixPath = ":/images/photo_woman.png";
    }
    ui->portrait_Label->setPixmap(pixPath);

    ui->height_LineEdit->setText(QString::number(st_PatientMsg.height));
    ui->weight_LineEdit->setText(QString::number(st_PatientMsg.weight));
    ui->userTel_LineEdit->setText(st_PatientMsg.phone);
    ui->address_LineEdit->setText(st_PatientMsg.address);
    ui->remark_LineEdit->setText(st_PatientMsg.remarkMessage);

}

ST_PatientMsg EditUser::getUserData()
{
    return st_CurrentUserData;
}

void EditUser::clearUserData()
{
    ui->ID_LineEdit->clear();
    ui->name_LineEdit->clear();
    ui->age_LineEdit->clear();

    ui->male_RadioButton->setChecked(true);
    ui->female_RadioButton->setChecked(false);

    ui->height_LineEdit->clear();
    ui->weight_LineEdit->clear();
    ui->userTel_LineEdit->clear();
    ui->address_LineEdit->clear();
    ui->remark_LineEdit->clear();
    ui->userInfo_LineEdit->clear();

}

void EditUser::closeEvent(QCloseEvent *event)
{
    qDebug()<<"EditUser::closeEvent";
    Q_UNUSED(event)
}

void EditUser::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    qDebug()<<"EditUser::showEvent";
}

void EditUser::initWidgetUI()
{
    QGraphicsDropShadowEffect * shadowEffect = new QGraphicsDropShadowEffect();
    //向四周均匀扩展阴影
    shadowEffect->setOffset(0,0);
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor("#BBBBBB"));
    ui->groupBox->setGraphicsEffect(shadowEffect);

    ui->ID_LineEdit->setEnabled(false);
}
void EditUser::setUserTable(ST_PatientMsg st_PatientMsg)
{
    //新建用户的添加，先将数据保存到内存中
    QString tableName("PatientMsg");
    QVariantMap vMap = patientMsgToVariantMap(st_PatientMsg);

    if(!CDatabaseInterface::getInstance()->insertRowTable(tableName,vMap))
    {
        qDebug()<<CDatabaseInterface::getInstance()->getLastError();
        //进入用户管理界面
        emit signalUserSelected(0);
    }
    else
    {
        //数据插入成功
        emit signalUpdateUserList();
        //进入评估界面
        MainWindowPageControl::getInstance()->setCurrentPage(2);
    }
}

//写配置
void EditUser::writeSetting()
{
    m_currentID++;
    QString dirPath = QApplication::applicationDirPath() + "/conf/";
    QDir confdir(dirPath);

    if(!confdir.exists())
        confdir.mkdir(dirPath);
    QString confFile(dirPath + "IDconf.ini");
    QSettings iniSetting(confFile, QSettings::IniFormat);
    iniSetting.setValue("number",m_currentID);

}
//读配置
void EditUser::readSetting()
{
    QString dirPath = QApplication::applicationDirPath() + "/conf/";
    QDir confdir(dirPath);
    if(!confdir.exists())
        confdir.mkdir(dirPath);
    QString confFile(dirPath + "IDconf.ini");
    QSettings iniSetting(confFile, QSettings::IniFormat);
    m_currentID = iniSetting.value("number").toUInt();
}

void EditUser::on_male_RadioButton_clicked()
{
    QString pixPath = ":/images/photo_man.png";
    ui->portrait_Label->setPixmap(pixPath);
}

void EditUser::on_female_RadioButton_clicked()
{
    QString pixPath = ":/images/photo_woman.png";
    ui->portrait_Label->setPixmap(pixPath);
}

void EditUser::slotCloseBtn()
{
    static int times = 0;
    times++;
    if(times == 2)
    {
        qDebug()<<"x close";
        this->close();
        emit signalUserSelected(0);
        times = 0;
    }
}

//返回到本页面
void EditUser::slotBackToEditUserPage()
{
    switchPageType(tr("编辑用户"),1);
    this->show();
}

void EditUser::on_leftSide_Btn_clicked()
{
    ui->userInfo_LineEdit->setText(tr("左侧"));
    m_footIndex = 0;
}

void EditUser::on_rightSide_Btn_clicked()
{
    ui->userInfo_LineEdit->setText(tr("右侧"));
    m_footIndex = 1;
}

void EditUser::setTitile(QString title)
{
    ui->title_Label->setText(title);
}

void EditUser::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(),QColor(0,0,0,100));
}

void EditUser::on_close_Btn_clicked()
{
    this->close();
}
