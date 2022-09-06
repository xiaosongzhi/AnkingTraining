#ifndef NEWRECORDTABLE_H
#define NEWRECORDTABLE_H

#include <QWidget>
class ActiveTrainingReport;
namespace Ui {
class NewRecordTable;
}

class NewRecordTable : public QWidget
{
    Q_OBJECT

public:
    explicit NewRecordTable(QWidget *parent = nullptr);
    ~NewRecordTable();

private:
    //填充评估表
    void fillEvaluateTable(int pageIndex);
    //填充训练表
    void fillTrainTable(int pageIndex);
    //初始化控件
    void initWidget();
    //初始化训练表格
    void initTrainRecordTable();
    //初始化评估表
    void initEvaluateRecordTable();

protected:
    virtual void showEvent(QShowEvent *event);

private slots:
    void on_evaluationFrontPage_btn_clicked();

    void on_evaluationBackPage_btn_clicked();

    void on_evaluateGo_btn_clicked();

    void on_trainFrontPage_btn_clicked();

    void on_trainBackPage_btn_clicked();

    void on_trainGo_btn_clicked();

    void on_trainRecord_Btn_clicked();

    void on_evaluationRecord_Btn_clicked();

private:
    Ui::NewRecordTable *ui;

    const int m_perPageCount;       //每页显示的个数
    int m_trainTableCurrentPage;    //tablewidget当前的页码，从1开始
    int m_evaluateTableCurrentPage; //evaluateTablewidget当前的页码，从1开始

    int m_trainRecordTotalPage;     //训练记录总页数
    int m_evaluateRecordTotalPage;  //评估记录总页数

    QList<QVariantMap> trainVariantMap;
    QList<QVariantMap> evaluateVariantMap;
    ActiveTrainingReport *viewReportDialog;

};

#endif // NEWRECORDTABLE_H
