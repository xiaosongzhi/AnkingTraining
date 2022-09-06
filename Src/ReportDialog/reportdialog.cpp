/*
#include "reportdialog.h"
#include "ui_reportdialog.h"
#include "cdatabaseinterface.h"
#include <QPixmap>
#include <QDebug>
#include <QMessageBox>
#include "cprinter.h"
#include "dataFormate.h"
#include "controlmaskdialog.h"
#include <QFile>
#include <QDir>
#include "currentuserdata.h"
#include <QAbstractAxis>
#include <QChart>
#include <QLineSeries>
#include <QDateTimeAxis>
#include "logger.h"
LOG4QT_DECLARE_STATIC_LOGGER(logger, ReportDialog)

ReportDialog::ReportDialog(QWidget *parent) :
    CBaseDialog(parent),
    ui(new Ui::ReportDialog),
    m_printer(NULL),
    m_scoreChart(NULL),
    m_scoreChartView(NULL)
{
    ui->setupUi(this);
    m_printer = new CPrinter();
    setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    ui->scoreRecord_Widget->initChart(SplineChart_E,"得分");
    ui->track_Widget->initChart(LineChart_E,"轨迹");
    ui->activePercent_Widget->initChart(SplineChart_E,"主动占比");
    loadStyleSheetFile();
}

ReportDialog::~ReportDialog()
{
    if(m_printer)
    {
        delete m_printer;
        m_printer = NULL;
    }
    delete ui;
}

void ReportDialog::loadStyleSheetFile()
{
    //加载样式文件s
    QFile qssFile("./QSS/reportFont.qss");
    if(!qssFile.open(QIODevice::ReadOnly))
        logger()->debug("reportFont.qss文件打开失败!");
    else
    {
        this->setStyleSheet(qssFile.readAll());
        qssFile.close();
    }
}

void ReportDialog::on_delete_Btn_clicked()
{
    this->close();
}

void ReportDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    ControlMaskDialog::getInstace()->setMaskDialogHide(true);
}

void ReportDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    ui->saveTips_Label->clear();
    ControlMaskDialog::getInstace()->setMaskDialogHide(false);
}

void ReportDialog::on_save_Btn_clicked()
{
    CDatabaseInterface *dataBase = CDatabaseInterface::getInstance();
    m_st_TrainReport.markMsg = ui->textEdit->toPlainText();
    QString filePath;
    QDir::currentPath();
    QString dirPath(QDir::currentPath() +"/report/reportList/");
    dirPath.append(QString::number(m_st_TrainReport.ID));
    QDir dir(dirPath);
    if(!dir.exists())
        dir.mkdir(dirPath);
    filePath = dirPath + "/" + m_st_TrainReport.startTime.toString("yyyyMMddhhmm") + ".txt";
    if(writePointsToFile(m_pointList,filePath))
        m_st_TrainReport.trainTrack = filePath;
    //主动占比
    QString query(QString("select * from reporttable where ID = '%1'").arg(m_st_TrainReport.ID));
    dataBase->exec(query);
    int totalCount = CDatabaseInterface::getInstance()->getValuesSize();
    QString query1(QString("select * from reporttable where ID = '%1' and trainMode = '2'").arg(m_st_TrainReport.ID));
    dataBase->exec(query1);
    int activeCount = CDatabaseInterface::getInstance()->getValuesSize();
    //如果本次为主动则增加1
    if(m_st_TrainReport.trainMode == 2)
        activeCount+=1;
    float activePercent = activeCount/(float)totalCount;
    m_st_TrainReport.activePercent = activePercent;

    QVariantMap vMap = trainReportToVariantMap(m_st_TrainReport);
    QString tableName("reporttable");

    if(!dataBase->insertRowTable(tableName,vMap))
    {
        //        qDebug()<<dataBase->getLastError();
        logger()->debug(dataBase->getLastError());
    }

    else
        ui->saveTips_Label->setText("数据保存成功");


    //将排行数据存放到数据库
    ST_TrainRanking st_TrainRanking;
    st_TrainRanking.ID = m_st_TrainReport.ID;
    st_TrainRanking.level = m_st_TrainReport.level;
    st_TrainRanking.name = m_st_TrainReport.name;
    //本次训练的时间
    st_TrainRanking.totalTime = m_st_TrainReport.trainDuration;
    //本次训练的得分
    st_TrainRanking.totalScore = m_st_TrainReport.score;
    saveRankingData(st_TrainRanking);
}

void ReportDialog::on_print_Btn_clicked()
{
    if(m_printer)
    {
        ui->save_Btn->setVisible(false);
        ui->delete_Btn->setVisible(false);
        ui->print_Btn->setVisible(false);
        QPixmap pixmap = grab();
        //                m_printer->printPreview(pixmap);
        ui->save_Btn->setVisible(true);
        ui->delete_Btn->setVisible(true);
        ui->print_Btn->setVisible(true);
    }
}

//生成图表
void ReportDialog::createCharts()
{
    //此处进行一个约定，一个游戏训练完成定义为一次训练，因为每次游戏训练完成都会产生一个报告
    //根据ID查询该用户所有训练的记录
    CDatabaseInterface *dataBase = CDatabaseInterface::getInstance();
    QString query(QString("select * from reporttable where ID = '%1' order by startTime").arg(m_st_TrainReport.ID));
    if(dataBase->exec(query))
    {
        int count = CDatabaseInterface::getInstance()->getValuesSize();
        QList<QVariantMap> reportValueList = dataBase->getValues(0,count);
        //主动占比
        activePercentChart(reportValueList);
        //得分记录--只用于记录当天的
        scoreRecordChart(reportValueList);
        //运动轨迹
        moveTrackChart();
    }
    else
        qDebug()<<"生成图表失败";
}

//主动占比曲线
void ReportDialog::activePercentChart(QList<QVariantMap> &reportValueList)
{
    DataTable dataTable;
    DataList dataList;
    for(int i = 0;i < reportValueList.size();i++)
    {
        Data pointData;
        QPoint tempPoint;
        int percent = 0;
        QDateTime dateTime;
        QVariantMap vMap = reportValueList.at(i);
        if(vMap.contains("activePercent"))
            percent = vMap.value("activePercent").toFloat()*100;
        tempPoint.setX(i);
        tempPoint.setY(percent);
        pointData.first = tempPoint;
        dataList.append(pointData);
    }
    dataTable.append(dataList);
    ui->activePercent_Widget->setChartData(SplineChart_E,dataTable);

}
//得分记录曲线
void ReportDialog::scoreRecordChart(QList<QVariantMap> &reportValueList)
{
    //根据ID查询该用户所有训练的记录

    QMap<QDateTime,int> scoreMap;

    for(int i = 0;i < reportValueList.size();i++)
    {
        int trainScore = 0;
        QDateTime dateTime;
        QVariantMap vMap = reportValueList.at(i);
        if(vMap.contains("score"))
            trainScore = vMap.value("score").toInt();
        if(vMap.contains("startTime"))
            dateTime = vMap.value("startTime").toDateTime();
        scoreMap.insert(dateTime,trainScore);
    }
    DataTable dataTable;
    DataList dataList;
    int index = 1;
    QMapIterator<QDateTime, int> i(scoreMap);
    while (i.hasNext()) {
        i.next();
        Data pointData;
        pointData.first.setX(index);
        pointData.first.setY(i.value());
        dataList.append(pointData);
        index++;
    }
    dataTable.append(dataList);

    ui->scoreRecord_Widget->setChartData(SplineChart_E,dataTable);
}
//运动轨迹曲线
void ReportDialog::moveTrackChart()
{
    DataTable dataTable;
    DataList dataList;
    foreach(QPoint point,m_pointList)
    {
        Data pointData;
        pointData.first = point;
        dataList.append(pointData);
    }
    dataTable.append(dataList);

    ui->track_Widget->setChartData(LineChart_E,dataTable);
}

void ReportDialog::setTrainReport(const ST_TrainReport &st_TrainReport,QList<QPoint>& pointList,quint8 type)
{
    setShowType(type);
    m_st_TrainReport = st_TrainReport;
    ui->score_Label->setText(QString::number(st_TrainReport.score));
    ui->progressBar->setValue(st_TrainReport.level * 20);
    //训练模式
    switch(st_TrainReport.trainMode)
    {
    case E_TRAINMODE_PASSIVE:
        ui->trainMode_Label->setText("被动模式");
        break;
    case E_TRAINMODE_BOOST:
        ui->trainMode_Label->setText("助力模式");
        break;
    case E_TRAINMODE_ACTIVE:
        ui->trainMode_Label->setText("主动模式");
        break;
    case E_TRAINMODE_AGAINST:
        ui->trainMode_Label->setText("抗阻模式");
        break;
    }
    //运动范围
    switch(st_TrainReport.trainScorp)
    {
    case 1:
        ui->trainScope_Label->setText("小");
        break;
    case 2:
        ui->trainScope_Label->setText("中");
        break;
    case 3:
        ui->trainScope_Label->setText("大");
        break;
    }
    QString pixPath(st_TrainReport.trainTrack);
    QPixmap pix(pixPath);
    ui->trackPath_Label->setPixmap(pix);

    int durationMinutes = st_TrainReport.trainDuration/60;
    int hours = durationMinutes/60;
    int minutes = durationMinutes%60;
    ui->trainTime_Label->setText(QString("%1时%2分").arg(hours).arg(minutes));
    ui->trainLength_Label->setText(QString("%1米").arg(QString::number(st_TrainReport.trainLength/1000.f,'f',3)));
    ui->trainSpeed_Label->setText(QString("%1厘米/秒").arg(QString::number(st_TrainReport.trainSpeed,'f',2)));
    ui->trainPower_Label->setText(QString("%1千卡").arg(QString::number(st_TrainReport.energy,'f',3)));
    ui->spasmTimes_Label->setText(QString::number(st_TrainReport.spasmTimes));
    ui->textEdit->setText(st_TrainReport.markMsg);
    m_pointList = pointList;
    createCharts();
    this->show();
}

void ReportDialog::setTrainReport(const ST_TrainReport& st_TrainReport,quint8 type)
{
    QList<QPoint> list = getPointListFromFile(st_TrainReport.trainTrack);
    setTrainReport(st_TrainReport,list,type);
}

QList<QPoint> ReportDialog::getPointListFromFile(QString filePath)
{
    QFile file(filePath);
    QList<QPoint> pointList;
    pointList.clear();
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);

        while(!in.atEnd())
        {
            QString str = in.readLine();
            if(str.contains("["))
                str.remove("[");
            if(str.contains("]"))
                str.remove("]");
            QStringList list = str.split(',');
            QPoint point(list.at(0).toInt(),list.at(1).toInt());

            pointList.append(point);
        }
    }
    return pointList;
}

bool ReportDialog::writePointsToFile(QList<QPoint>& pointList,QString filePath)
{
    QFile file(filePath);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        for(int i = 0;i < pointList.size();i++)
        {
            QPointF tempPoint = pointList.at(i);
            out<<"["<<tempPoint.x()<<","<<tempPoint.y()<<"]"<<endl;
        }
    }
    else
    {
        qDebug()<<"文件打开失败";
        logger()->debug(filePath+"打开失败");
        return false;
    }
    file.close();

    return true;
}

void ReportDialog::setShowType(quint8 type)
{
    switch(type)
    {
    case 0:
        ui->save_Btn->setVisible(true);
        ui->delete_Btn->setVisible(true);
        break;
    case 1:
        ui->save_Btn->setVisible(false);
        ui->delete_Btn->setVisible(false);
        break;
    }
}

void ReportDialog::saveRankingData(ST_TrainRanking& st_TrainRanking)
{
    CDatabaseInterface *dataBase = CDatabaseInterface::getInstance();
    //先查询是否有该ID
    QString table("rankingtable");
    QString query(QString("select * from rankingtable where ID = '%1'").arg(m_st_TrainReport.ID));
    dataBase->exec(query);
    int count = dataBase->getValuesSize();
    //如果有则查询出等级，得分，训练时长等数据，将该数据更新
    if(-1 != count)
    {
        QList<QVariantMap> vMapList = dataBase->getValues(0,1);
        ST_TrainRanking st_LastTrainRanking;
        if(!vMapList.isEmpty())
        {
            st_LastTrainRanking = variantMapToTrainRanking(vMapList.at(0));
            st_LastTrainRanking.totalScore += st_TrainRanking.totalScore;
            st_LastTrainRanking.totalTime += st_TrainRanking.totalTime;
            QVariantMap vMap = trainRankingToVariantMap(st_LastTrainRanking);

            if(dataBase->updateRowTable(table,"ID",vMap))
            {
                qDebug()<<"数据更新成功";
                logger()->debug("训练报告数据更新成功");
            }

        }
    }
    //如果没有则插入新的数据
    else
    {
        QVariantMap vMap = trainRankingToVariantMap(st_TrainRanking);
        QString tableName("rankingtable");
        if(!dataBase->insertRowTable(tableName,vMap))
            logger()->debug(dataBase->getLastError());
        else
            logger()->debug("训练报告数据存储成功");
    }
}
*/
