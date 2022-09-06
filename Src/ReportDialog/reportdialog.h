#if 0
//#ifndef REPORTDIALOG_H
//#define REPORTDIALOG_H

#include <QWidget>
#include "userdataformate.h"
#include "cbasedialog.h"
#include <QChart>
#include <QChartView>
class CPrinter;

namespace Ui {
class ReportDialog;
}
using namespace QtCharts;
class ReportDialog : public CBaseDialog
{
    Q_OBJECT

public:
    explicit ReportDialog(QWidget *parent = nullptr);
    ~ReportDialog();
    //用于生成报告
    void setTrainReport(const ST_TrainReport&,QList<QPoint>&,quint8 type);
    //用于查看报告
    void setTrainReport(const ST_TrainReport&,quint8 type);

private slots:
    void on_delete_Btn_clicked();

    void on_save_Btn_clicked();

    void on_print_Btn_clicked();
protected:
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);
private:
    //生成图表
    void createCharts();
    //主动占比曲线
    void activePercentChart(QList<QVariantMap> &reportValueList);
    //得分记录曲线
    void scoreRecordChart(QList<QVariantMap> &reportValueList);
    //运动轨迹曲线
    void moveTrackChart();
    //从文件中读取数据
    QList<QPoint> getPointListFromFile(QString filePath);
    //将点写入文件
    bool writePointsToFile(QList<QPoint>&,QString filePath);

    /****显示报告结果的类型
     * @quint8 type  0-保存报告界面  1-查看报告界面
     ******/
    void setShowType(quint8 type);

    //将排行数据保存到数据库
    void saveRankingData(ST_TrainRanking&);

    void loadStyleSheetFile();
private:
    Ui::ReportDialog *ui;
    ST_TrainReport m_st_TrainReport;
    CPrinter *m_printer;
    QList<QPoint> m_pointList;

    QChart *m_scoreChart;
    QChartView *m_scoreChartView;

};

#endif // REPORTDIALOG_H

