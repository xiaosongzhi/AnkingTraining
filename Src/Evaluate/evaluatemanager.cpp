#include "evaluatemanager.h"
#include "ui_evaluatemanager.h"
#include <QDebug>
#include "ccommunicateapi.h"

//#pragma execution_character_set("utf-8")

EvaluateManager::EvaluateManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EvaluateManager)
{
    ui->setupUi(this);
    m_evaluatepage=new EvaluatePage;
    m_videoTipsWidget = new SwitchActivePassiveWidget();
    connect(m_videoTipsWidget,SIGNAL(signalConfirmed(E_VideoTipsType)),this,SLOT(slotVideoTipsType(E_VideoTipsType)));

    m_circleTipsWidget = new CircleTipsWidget();
    connect(m_circleTipsWidget,SIGNAL(signalConfirmed(E_VideoTipsType)),this,SLOT(slotVideoTipsType(E_VideoTipsType)));

//    ui->adductionAbduction_Btn->setVisible(false);
}

EvaluateManager::~EvaluateManager()
{
    if(m_videoTipsWidget)
        delete m_videoTipsWidget;
    if(m_circleTipsWidget)
        delete m_circleTipsWidget;
    if(m_evaluatepage)
        delete m_evaluatepage;
    delete ui;
}

void EvaluateManager::on_plantarFlexion_Btn_clicked()
{
    m_videoTipsWidget->switchTipsType(PLANTAR_FLEXION_TIPS_E);
    m_videoTipsWidget->show();
}

void EvaluateManager::on_adductionAbduction_Btn_clicked()
{
    m_videoTipsWidget->switchTipsType(ADDUCTION_ABDUCTION_TIPS_E);
    m_videoTipsWidget->show();
}

void EvaluateManager::on_varusValgus_Btn_clicked()
{
    m_videoTipsWidget->switchTipsType(VARUS_VALGUS_TIPS_E);
    m_videoTipsWidget->show();
}

void EvaluateManager::on_comprehenSive_Btn_clicked()
{
    m_circleTipsWidget->switchTipsType(COMPREHENSIVE_TIPS_E);
    m_circleTipsWidget->show();
}

//斜跖屈背屈
void EvaluateManager::on_xiePlantarFlexion_Btn_clicked()
{
    m_videoTipsWidget->switchTipsType(SLOPE_PLANTAR_FLEXION_TIPS_E);
    m_videoTipsWidget->show();
}

void EvaluateManager::slotVideoTipsType(E_VideoTipsType E_TipsType)
{
    switch(E_TipsType)
    {
    case COMPREHENSIVE_TIPS_E:
        m_evaluatepage->switchEvaluatePage(COMPREHENSIVE_E);
        break;
    case PLANTAR_FLEXION_TIPS_E:
        m_evaluatepage->switchEvaluatePage(PLANTAR_FLEXION_E);
        break;
    case ADDUCTION_ABDUCTION_TIPS_E:
        m_evaluatepage->switchEvaluatePage(ADDUCTION_ABDUCTION_E);
        break;
    case VARUS_VALGUS_TIPS_E:
        m_evaluatepage->switchEvaluatePage(VARUS_VALGUS_E);
        break;
    case SLOPE_PLANTAR_FLEXION_TIPS_E:
        m_evaluatepage->switchEvaluatePage(SLOPE_PLANTAR_FLEXION_E);
        break;
    case PASSIVE_TRAIN_TIPS:
        break;
    case ACTIVE_TRAIN_TIPS:
        break;
    case INITING_TIPS:
        break;
    case GYROSCOPE_CHECK_TIPS:
        break;
    }
    m_evaluatepage->show();
}
