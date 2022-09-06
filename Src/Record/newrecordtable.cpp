#include "newrecordtable.h"
#include "ui_newrecordtable.h"
#include "activetrainingreport.h"
#include "currentuserdata.h"
#include "cdatabaseinterface.h"

NewRecordTable::NewRecordTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewRecordTable),
    m_perPageCount(7),
    m_trainTableCurrentPage(1),
    m_evaluateTableCurrentPage(1),
    m_trainRecordTotalPage(1),
    m_evaluateRecordTotalPage(1),
    viewReportDialog(NULL)
{
    ui->setupUi(this);
    initWidget();
    viewReportDialog = new ActiveTrainingReport();
    initTrainRecordTable();
    initEvaluateRecordTable();
    ui->stackedWidget->setCurrentIndex(0);
}

NewRecordTable::~NewRecordTable()
{
    if(viewReportDialog)
        delete viewReportDialog;
    delete ui;
}


//初始化控件
void NewRecordTable::initWidget()
{
    ui->evaluationFrontPage_btn->setStyleSheet("border-image: url(:/images/frontPage.png);");
    ui->trainFrontPage_btn->setStyleSheet("border-image: url(:/images/frontPage.png);");

    ui->evaluationBackPage_btn->setStyleSheet("border-image: url(:/images/backPage.png);");
    ui->trainBackPage_btn->setStyleSheet("border-image: url(:/images/backPage.png);");
}
//初始化训练表格
void NewRecordTable::initTrainRecordTable()
{
    ui->train_TableWidget->setColumnCount(8);
    ui->train_TableWidget->setRowCount(7);

    ui->train_TableWidget->setShowGrid(false);
    //设置列宽
    ui->train_TableWidget->setColumnWidth(0,350);
    ui->train_TableWidget->setColumnWidth(1,230);
    ui->train_TableWidget->setColumnWidth(2,150);
    ui->train_TableWidget->setColumnWidth(3,180);
    ui->train_TableWidget->setColumnWidth(4,180);
    ui->train_TableWidget->setColumnWidth(5,140);
    ui->train_TableWidget->setColumnWidth(6,140);
    ui->train_TableWidget->setColumnWidth(7,140);
    //设置行高
    for(int i = 0;i < 7;i++)
    ui->train_TableWidget->setRowHeight(i,72);

    ui->train_TableWidget->setAlternatingRowColors(true);
    ui->train_TableWidget->setPalette(QPalette(Qt::lightGray));

    //在默认情况下，表格里的字符是可以更改的，这样设置只读。禁止用户修改
    ui->train_TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置整行选中
    ui->train_TableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //隐藏列表头
    ui->train_TableWidget->verticalHeader()->setVisible(false);
    //隐藏行表头
    ui->train_TableWidget->horizontalHeader()->setVisible(false);
}

//初始化评估表
void NewRecordTable::initEvaluateRecordTable()
{
    ui->evaluate_TableWidget->setColumnCount(5);
    ui->evaluate_TableWidget->setRowCount(7);

    //设置表头里面的内容
    ui->evaluate_TableWidget->setColumnWidth(0,300);
    ui->evaluate_TableWidget->setColumnWidth(1,290);
    ui->evaluate_TableWidget->setColumnWidth(2,290);
    ui->evaluate_TableWidget->setColumnWidth(3,350);
    ui->evaluate_TableWidget->setColumnWidth(4,290);
    ui->evaluate_TableWidget->verticalHeader()->setDefaultSectionSize(72);

    ui->evaluate_TableWidget->setAlternatingRowColors(true);
    ui->evaluate_TableWidget->setPalette(QPalette(Qt::lightGray));

    ui->evaluate_TableWidget->setShowGrid(false);

    //在默认情况下，表格里的字符是可以更改的，这样设置只读。禁止用户修改
    ui->evaluate_TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置整行选中
    ui->evaluate_TableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //隐藏列表头
    ui->evaluate_TableWidget->verticalHeader()->setVisible(false);
    //隐藏行表头
    ui->evaluate_TableWidget->horizontalHeader()->setVisible(false);
}

void NewRecordTable::showEvent(QShowEvent *event)
{
    fillTrainTable(1);
    fillEvaluateTable(1);
}

//填充评估表
void NewRecordTable::fillEvaluateTable(int pageIndex)
{
    ST_PatientMsg st_PatientMsg;
    st_PatientMsg = CurrentUserData::getInstace()->getCurrentPatientMsg();
    QString query(QString("select * from EvaluateRecord where ID = '%1' order by startTime DESC").arg(st_PatientMsg.ID));
    CDatabaseInterface *dataBase = CDatabaseInterface::getInstance();
    if(dataBase->exec(query))
    {
        int count = CDatabaseInterface::getInstance()->getValuesSize();
        //总页数
        int displayPage = count/m_perPageCount;

        //如果不可以整除则加1
        if(count%m_perPageCount != 0)
            displayPage += 1;

        m_evaluateRecordTotalPage = displayPage;
        //当前页要显示的条数
        int displayCount = 0;
        //满额显示

        if(pageIndex < displayPage)
        {
            displayCount = 7;
        }
        else if(pageIndex == displayPage)
        {
            displayCount = count - (pageIndex-1)*7;
        }
        else
        {
            return;
        }

        //设置页数
        ui->evaluateIndexPage_Label->setText(QString::number(pageIndex)+"/"+QString::number(m_evaluateRecordTotalPage)+"页");

        QList<QVariantMap> variantMap = CDatabaseInterface::getInstance()->getValues((pageIndex-1)*7 + 1,displayCount);
        evaluateVariantMap = variantMap;
        //填充表格数据
        ui->evaluate_TableWidget->clearContents();

        for(int i = 0;i < variantMap.size();i++)
        {
            //填充删除按钮
            QPushButton *delete_evaluation_btn=new QPushButton();
            delete_evaluation_btn->setStyleSheet("border-image: url(:/images/delete.png);");
            delete_evaluation_btn->setMaximumSize(50,50);
            connect(delete_evaluation_btn,&QPushButton::clicked,[this](){
                int rowIndex=ui->evaluate_TableWidget->currentRow();
                if(rowIndex!=-1)
                {
                    //删除是删除当前记录并不是删除表格的一行
                    ST_EvaluateRecord st_evaluateRecord = variantMapToEvaluateRecord(evaluateVariantMap.at(rowIndex));
                    QString tableName = "EvaluateRecord";
                    QString columName = "UUID";
                    if(!CDatabaseInterface::getInstance()->deleteRowTable(tableName,columName,st_evaluateRecord.UUID))
                        qDebug()<<CDatabaseInterface::getInstance()->getLastError();
                    else
                        qDebug()<<"删除成功";
                    fillEvaluateTable(m_evaluateTableCurrentPage);
                }
            });
            ui->evaluate_TableWidget->setCellWidget(i,4,delete_evaluation_btn);
            //填充表格内容
            ST_EvaluateRecord st_evaluateRecord = variantMapToEvaluateRecord(variantMap.at(i));
            //评估日期
            ui->evaluate_TableWidget->setItem(i,0,new QTableWidgetItem());
            ui->evaluate_TableWidget->item(i,0)->setTextAlignment(Qt::AlignCenter);
            ui->evaluate_TableWidget->item(i,0)->setText(st_evaluateRecord.startTime.toString("yyyy-MM-dd hh:mm:ss"));

            //评估类型
            QString evaluateType;
            switch(st_evaluateRecord.evaluateMode)
            {
            case COMPREHENSIVE_E:
                evaluateType = tr("综合评估");
                break;
            case PLANTAR_FLEXION_E:
                evaluateType = tr("跖屈背屈");
                break;
            case ADDUCTION_ABDUCTION_E:
                evaluateType = tr("内收外展");
                break;
            case VARUS_VALGUS_E:
                evaluateType = tr("内翻外翻");
                break;
            case SLOPE_PLANTAR_FLEXION_E:
                evaluateType = tr("斜跖屈背屈");
                break;
            }
            ui->evaluate_TableWidget->setItem(i,1,new QTableWidgetItem());
            ui->evaluate_TableWidget->item(i,1)->setTextAlignment(Qt::AlignCenter);
            ui->evaluate_TableWidget->item(i,1)->setText(evaluateType);
            //评估时长
            int periodTime = 0;
            periodTime = st_evaluateRecord.startTime.secsTo(st_evaluateRecord.endTime)/60;
            if(periodTime < 1)
                periodTime = 1;
            ui->evaluate_TableWidget->setItem(i,2,new QTableWidgetItem());
            ui->evaluate_TableWidget->item(i,2)->setTextAlignment(Qt::AlignCenter);
            ui->evaluate_TableWidget->item(i,2)->setText(QString::number(periodTime));
            //评估范围
            QString evaluteScope;
            evaluteScope = QString::number(st_evaluateRecord.lowMargin) + "°~ " + QString::number(st_evaluateRecord.highMargin) + "°";
            ui->evaluate_TableWidget->setItem(i,3,new QTableWidgetItem());
            ui->evaluate_TableWidget->item(i,3)->setTextAlignment(Qt::AlignCenter);
            ui->evaluate_TableWidget->item(i,3)->setText(evaluteScope);
        }
    }
    else
    {
        qDebug()<<dataBase->getLastError();
    }
}
//填充训练表
void NewRecordTable::fillTrainTable(int pageIndex)
{
    ST_PatientMsg st_PatientMsg;
    st_PatientMsg = CurrentUserData::getInstace()->getCurrentPatientMsg();
    QString query(QString("select * from TrainReport where ID = '%1' order by startTime DESC").arg(st_PatientMsg.ID));
    CDatabaseInterface *dataBase = CDatabaseInterface::getInstance();
    if(dataBase->exec(query))
    {
        int count = CDatabaseInterface::getInstance()->getValuesSize();
        //总页数
        int displayPage = count/m_perPageCount;
        //如果不可以整除则加1
        if(count%m_perPageCount)
            displayPage += 1;
        m_trainRecordTotalPage = displayPage;
        //当前页要显示的条数
        int displayCount = 0;
        //满额显示
        if(pageIndex < displayPage)
        {
            displayCount = 7;
        }
        else if(pageIndex == displayPage)
        {
            displayCount = count - (pageIndex-1)*7;
        }
        else
        {
            return;
        }

        ui->trainIndexPage_Label->setText(QString::number(pageIndex)+"/"+QString::number(m_trainRecordTotalPage)+"页");
        trainVariantMap.clear();
        QList<QVariantMap> variantMap = CDatabaseInterface::getInstance()->getValues((pageIndex-1)*7 + 1,displayCount);
        trainVariantMap = variantMap;
        ui->train_TableWidget->clearContents();

        for(int i = 0;i < variantMap.size();i++)
        {
            //查看按钮
            QPushButton *view_btn=new QPushButton();
            view_btn->setStyleSheet("border-image: url(:/images/check.png);");
            view_btn->setMaximumSize(50,50);
            ui->train_TableWidget->setCellWidget(i,5,view_btn);
            connect(view_btn,&QPushButton::clicked,[this](){
                int rowIndex=ui->train_TableWidget->currentRow();
            ST_TrainReport st_trainReport = variantMapToTrainReport(trainVariantMap.at(rowIndex));
            if(0 == st_trainReport.trainMode)
                viewReportDialog->setReportType(tr("被动训练报告"));
            else if(1 == st_trainReport.trainMode)
                viewReportDialog->setReportType(tr("主动训练报告"));
            viewReportDialog->setTrainReport(st_trainReport);
            });
            //打印按钮
            QPushButton *print_train_btn=new QPushButton();
            print_train_btn->setStyleSheet("border-image: url(:/images/printDisable.png);");
            print_train_btn->setMaximumSize(50,50);
            ui->train_TableWidget->setCellWidget(i,6,print_train_btn);
            //删除按钮
            QPushButton *delete_train_btn=new QPushButton();
            delete_train_btn->setStyleSheet("border-image: url(:/images/delete.png);");
            delete_train_btn->setMaximumSize(50,50);
            connect(delete_train_btn,&QPushButton::clicked,[this](){
                int rowIndex=ui->train_TableWidget->currentRow();
                if(rowIndex!=-1)
                {
                    ST_TrainReport st_trainReport = variantMapToTrainReport(trainVariantMap.at(rowIndex));
                    QString tableName = "TrainReport";
                    QString columName = "UUID";
                    if(!CDatabaseInterface::getInstance()->deleteRowTable(tableName,columName,st_trainReport.UUID))
                        qDebug()<<CDatabaseInterface::getInstance()->getLastError();
                    else
                        qDebug()<<"删除成功";
                    fillTrainTable(m_trainTableCurrentPage);
                }
            });
            ui->train_TableWidget->setCellWidget(i,7,delete_train_btn);


            ST_TrainReport st_trainReport = variantMapToTrainReport(variantMap.at(i));
            //训练日期
            ui->train_TableWidget->setItem(i,0,new QTableWidgetItem());
            ui->train_TableWidget->item(i,0)->setTextAlignment(Qt::AlignCenter);
            ui->train_TableWidget->item(i,0)->setText(st_trainReport.startTime.toString("yyyy-MM-dd hh:mm:ss"));
            //训练类型
            QString trainType;
            switch(st_trainReport.trainTypeE)
            {
            case COMPREHENSIVE_E:
                trainType = tr("综合评估");
                break;
            case PLANTAR_FLEXION_E:
                trainType = tr("跖屈背屈");
                break;
            case ADDUCTION_ABDUCTION_E:
                trainType = tr("内收外展");
                break;
            case VARUS_VALGUS_E:
                trainType = tr("内翻外翻");
                break;
            case SLOPE_PLANTAR_FLEXION_E:
                trainType = tr("斜跖屈背屈");
                break;
            case PASSIVE_TRAIN_CIRCLE:
                trainType = tr("被动圆周");
                break;
            case PASSIVE_TRAIN_SUB:
                trainType = tr("被动分段");
                break;
            }
            ui->train_TableWidget->setItem(i,1,new QTableWidgetItem());
            ui->train_TableWidget->item(i,1)->setTextAlignment(Qt::AlignCenter);
            ui->train_TableWidget->item(i,1)->setText(trainType);
            //训练时长
            int trainTime = st_trainReport.startTime.secsTo(st_trainReport.endTime)/60;
            ui->train_TableWidget->setItem(i,2,new QTableWidgetItem());
            ui->train_TableWidget->item(i,2)->setTextAlignment(Qt::AlignCenter);
            ui->train_TableWidget->item(i,2)->setText(QString::number(trainTime));
            //游戏
            ui->train_TableWidget->setItem(i,3,new QTableWidgetItem());
            ui->train_TableWidget->item(i,3)->setTextAlignment(Qt::AlignCenter);
            ui->train_TableWidget->item(i,3)->setText(st_trainReport.gameName);
            //得分
            ui->train_TableWidget->setItem(i,4,new QTableWidgetItem());
            ui->train_TableWidget->item(i,4)->setTextAlignment(Qt::AlignCenter);
            ui->train_TableWidget->item(i,4)->setText(QString::number(st_trainReport.score));
        }
    }
    else
    {
        qDebug()<<dataBase->getLastError();
    }
}
void NewRecordTable::on_trainRecord_Btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->trainRecord_Btn->setStyleSheet("background: #0D9DDB;border-radius: 4px;color:white;");
    ui->evaluationRecord_Btn->setStyleSheet("background: #EFEFEF;border-radius: 4px;color:#333333;");
}

void NewRecordTable::on_evaluationRecord_Btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->evaluationRecord_Btn->setStyleSheet("background: #0D9DDB;border-radius: 4px;color:white;");
    ui->trainRecord_Btn->setStyleSheet("background: #EFEFEF;border-radius: 4px;color:#333333;");
}

void NewRecordTable::on_evaluationFrontPage_btn_clicked()
{
    m_evaluateTableCurrentPage--;
    if(m_evaluateTableCurrentPage < 1)
    {
        m_evaluateTableCurrentPage = 1;
        return;
    }
    fillEvaluateTable(m_evaluateTableCurrentPage);
}

void NewRecordTable::on_evaluationBackPage_btn_clicked()
{
    m_evaluateTableCurrentPage++;
    if(m_evaluateTableCurrentPage > m_evaluateRecordTotalPage)
    {
        m_evaluateTableCurrentPage = m_evaluateRecordTotalPage;
        return;
    }
    fillEvaluateTable(m_evaluateTableCurrentPage);
}

void NewRecordTable::on_evaluateGo_btn_clicked()
{
    int destPage = ui->evaluateDestPage_LineEdit->text().toUInt();
    if(destPage > 0 && destPage <= m_evaluateRecordTotalPage)
    {
        m_evaluateTableCurrentPage = destPage;
        fillEvaluateTable(destPage);
    }
}

void NewRecordTable::on_trainFrontPage_btn_clicked()
{
    m_trainTableCurrentPage--;
    if(m_trainTableCurrentPage  < 1)
    {
        m_trainTableCurrentPage = 1;
        return;
    }
    fillTrainTable(m_trainTableCurrentPage);
}

void NewRecordTable::on_trainBackPage_btn_clicked()
{
    m_trainTableCurrentPage++;
    if(m_trainTableCurrentPage > m_trainRecordTotalPage)
        m_trainTableCurrentPage = m_trainRecordTotalPage;
    fillTrainTable(m_trainTableCurrentPage);
}

void NewRecordTable::on_trainGo_btn_clicked()
{
    int destPage = ui->trainDestPage_LineEdit->text().toUInt();
    if(destPage > 0 && destPage <= m_trainRecordTotalPage)
    {
        m_trainTableCurrentPage = destPage;
        fillTrainTable(destPage);
    }
}




