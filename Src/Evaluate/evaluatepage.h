#ifndef EVALUATEPAGE_H
#define EVALUATEPAGE_H

#include <QWidget>
#include "dataFormate.h"
#include <QImage>
#include <QtGui>
#include <QSlider>
#include "userdataformate.h"
namespace Ui {
class EvaluatePage;
}

class EvaluatePage : public QWidget
{
    Q_OBJECT

public:
    explicit EvaluatePage(QWidget *parent = nullptr);
    ~EvaluatePage();

    //切换评估方式
    void switchEvaluatePage(E_TrainEvaluationType);

protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *event);
private:
    //设置评估的类型以及开关
    void setEvaluateControlParam(ST_EvaluateControl);
    //显示陀螺仪数据
    void displayGyroscopeData(const ST_GyroscopeData&);
    /****可视化显示陀螺仪角度****/
    //圆周可视化
    void displayComprehensive(int);
    //跖屈背屈可视化
    void displayPlantarFlexion(int);
    //内收外展可视化
    void displayAdductionAbduction(float);
    //内翻外翻可视化
    void displayVarusValgus(float);
    //斜跖屈背屈
    void displaySlopePlantarFlexion(float);

    //评估表数据库接口
    void setEvaluateTable(ST_EvaluateRecord);
    // 清理显示界面
    void clearDisplayLable();
private slots:
    void receiveCommData(QByteArray);

    void on_startEvaluate_Btn_clicked();

    void on_close_Btn_clicked();

    void on_check_Btn_clicked();

private:
    Ui::EvaluatePage *ui;
    ST_GyroscopeData st_gyroscope;

    E_TrainEvaluationType evaluateTypeE;
    int maxValue,minValue;  //最大最小角度值
    int maxCircleAngle;     //圆周最大角度
    bool isEvaluating;      //评估状态

    float basePitch;    //基准俯仰角
    uint32_t m_currentID;
    ST_EvaluateRecord st_evaluaterecord;

    int m_footIndex;

    bool isGetFirstData;
};

#endif // EVALUATEPAGE_H
