#include "Setting.h"
#include "ui_Setting.h"
#include <QDebug>
#include <QPalette>
#include "ccommunicateapi.h"
#include "cpassworddialog.h"
Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    initButtonGroup();
    ui->stackedWidget->setCurrentIndex(2);
    ui->passwordManager_btn->setVisible(false);

    m_translator = new QTranslator(this);

    ui->english_RadioButton->setVisible(false);
}

Setting::~Setting()
{
    delete ui;
}

void Setting::on_hospitalInformation_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);

}

void Setting::on_passwordManager_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Setting::on_systemSetting_Btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}
void Setting::on_productPresentation_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void Setting:: initButtonGroup()
{
    m_buttonGroup=new QButtonGroup();
    m_buttonGroup->setExclusive(true);
    m_buttonGroup->addButton(ui->hospitalInformation_btn,0);
    m_buttonGroup->addButton(ui->passwordManager_btn,1);
    m_buttonGroup->addButton(ui->productPresentation_btn,2);
    m_buttonGroup->addButton(ui->systemSetting_Btn,3);

    connect(m_buttonGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnGroupClicked(int)));
}
void Setting::slotBtnGroupClicked(int btnId)
{
    //设置当前按下的按钮为选中状态，其他的为未选中状态
    QPushButton *selectedBtn = qobject_cast<QPushButton*>(m_buttonGroup->button(btnId));

    selectedBtn->setStyleSheet("background: #0D9DDB;border-radius: 4px;color:white;");

    foreach(QAbstractButton* button,m_buttonGroup->buttons())
    {
        QPushButton *tempBtn =  qobject_cast<QPushButton*>(button);
        if(tempBtn != selectedBtn)
        {
            tempBtn->setStyleSheet("background: #EFEFEF;border-radius: 4px;color:#333333;");
        }
    }

}

void Setting::on_save_btn_clicked()
{
    CPasswordDialog m_dialog;
    int result = m_dialog.exec();
    if(result != 3)
        return;
}

void Setting::on_chinese_RadioButton_toggled(bool checked)
{
    if(checked)
    {
        if(m_translator->load("AnkleTrainning_CN.ts"))
        {
            qApp->installTranslator(m_translator);
        }

    }
}

void Setting::on_english_RadioButton_toggled(bool checked)
{
    if(checked)
    {
        if(m_translator->load("AnkleTrainning_EN.ts"))
        {
            qApp->installTranslator(m_translator);
        }

    }
}

void Setting::changeEvent(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        QWidget::changeEvent(event);
        break;
    }
}

