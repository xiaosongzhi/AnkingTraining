#ifndef ACTIVETRAININGREPORT_H
#define ACTIVETRAININGREPORT_H

#include <QWidget>
#include <QtCharts>
#include <QProgressBar>
#include <QLabel>
#include <QImage>
#include <QSlider>
#include <QtGui>
#include "userdataformate.h"
QT_CHARTS_USE_NAMESPACE
namespace Ui {
class ActiveTrainingReport;
}

class ActiveTrainingReport : public QWidget
{
    Q_OBJECT

public:
    explicit ActiveTrainingReport(QWidget *parent = nullptr);
    ~ActiveTrainingReport();
    //设置报告类型
    void setReportType(QString type);
    //用于生成报告
    void setTrainReport(const ST_TrainReport&);
protected:
    void paintEvent(QPaintEvent *event);
private slots:

    void on_close_btn_clicked();

    void on_noSave_btn_clicked();

    void on_save_btn_clicked();

    void on_print_btn_clicked();

private:
    //绘制训练时间曲线图的函数(从数据库中获取)
    void trainTimeChart(const QList<QPair<QDateTime,int>> &pointList);


   //报告表数据库接口
    bool saveReportTable(ST_TrainReport);

private:
    Ui::ActiveTrainingReport *ui;
    QChart *m_chart;
    QChartView *m_chartview;

    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
    QVBoxLayout *m_boxlayout;//设置布局
    ST_TrainReport st_trainreport;
    QList<QPoint> m_listpoint;
    ST_PatientMsg st_patientmsg;
    uint32_t m_currentID; //系统给用户分配的ID

};

#endif // ACTIVETRAININGREPORT_H
