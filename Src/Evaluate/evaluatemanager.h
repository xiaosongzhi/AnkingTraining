#ifndef EVALUATEMANAGER_H
#define EVALUATEMANAGER_H

#include <QWidget>
#include "evaluatepage.h"
#include "dataFormate.h"
#include "switchactivepassivewidget.h"
#include "circletipswidget.h"
namespace Ui {
class EvaluateManager;
}

class EvaluateManager : public QWidget
{
    Q_OBJECT

public:
    explicit EvaluateManager(QWidget *parent = nullptr);
    ~EvaluateManager();

private slots:

    //跖屈背屈
    void on_plantarFlexion_Btn_clicked();
    //内收外展
    void on_adductionAbduction_Btn_clicked();
    //内翻外翻
    void on_varusValgus_Btn_clicked();
    //综合（圆周）
    void on_comprehenSive_Btn_clicked();
    //斜跖屈背屈
    void on_xiePlantarFlexion_Btn_clicked();

public slots:
    void slotVideoTipsType(E_VideoTipsType);

private:
    Ui::EvaluateManager *ui;
    EvaluatePage *m_evaluatepage;
    SwitchActivePassiveWidget *m_videoTipsWidget;
    CircleTipsWidget *m_circleTipsWidget;
};

#endif // EVALUATEMANAGER_H
