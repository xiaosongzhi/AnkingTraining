#include "activetrainingreport.h"
#include "ui_activeTrainingReport.h"
#include <QMapIterator>
#include "currentuserdata.h"
#include "cdatabaseinterface.h"
#include <QPainter>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QPixmap>
//#pragma execution_character_set("utf-8")

ActiveTrainingReport::ActiveTrainingReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActiveTrainingReport),
    m_chart(NULL),
    m_chartview(NULL),
    m_axisX(NULL),
    m_axisY(NULL),
    m_boxlayout(NULL)

{
    ui->setupUi(this);
    m_chart=new QChart;
    m_chartview=new QChartView(m_chart);
    m_axisX=new QValueAxis;
    m_axisY=new QValueAxis;

    m_boxlayout=new QVBoxLayout(this);

    this->setWindowFlags(Qt::FramelessWindowHint);      //设置无边框
    setAttribute(Qt::WA_TranslucentBackground,true);    //设置透明
    ui->close_btn->setStyleSheet("border-image: url(:/images/close.png);");

    m_boxlayout->addWidget(m_chartview);
    ui->chart_widget->setLayout(m_boxlayout);
}

ActiveTrainingReport::~ActiveTrainingReport()
{
    if(m_chart)
        delete m_chart;
    if(m_chartview)
        delete m_chartview;
    if(m_axisX)
        delete m_axisX;
    if(m_axisY)
        delete m_axisY;
    if(m_boxlayout)
        delete m_boxlayout;
    delete ui;
}

void ActiveTrainingReport::setReportType(QString type)
{
    ui->reportType_Label->setText(type);
}

void ActiveTrainingReport::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(),QColor(0,0,0,100));
}

void ActiveTrainingReport::trainTimeChart(const QList<QPair<QDateTime,int>> &pointList)
{
    m_chart->removeAllSeries();
    QLineSeries *m_series = new QLineSeries();//实例化一个QSplineSeries对象;
    m_series->setColor(QColor(0,100,255));
    int yMaxValue = 0;

    for(int i = 0;i < pointList.size();i++)
    {
        QPointF point;
        point.setX(i);
        point.setY(pointList.at(i).second);
        m_series->append(point);
        if(yMaxValue < point.y())
            yMaxValue = point.y();
    }
    //添加最新一次的得分
    QPointF newScore;
    newScore.setX(pointList.size());
    newScore.setY(ui->score_label->text().toUInt());
    m_series->append(newScore);

    if(yMaxValue < newScore.y())
        yMaxValue = newScore.y();

    m_series->setVisible(true);
    m_series->setPointLabelsFormat("(@xPoint,@yPoint)");
    m_series->setPointLabelsVisible(false);
    m_chart->setTheme(QChart::ChartThemeLight);//设置白色主题
    m_chart->setDropShadowEnabled(true);//背景阴影
    m_chart->setAutoFillBackground(true);//设置背景自动填充
    m_chart->addSeries(m_series);

    //创建x轴和y轴
    //    QValueAxis *m_axisX=new QValueAxis;
    m_axisX->setRange(0,10);//默认坐标是动态计算大小的
    m_axisX->setLabelFormat("%d");

    m_axisY->setRange(0,yMaxValue);

    //设置轴单位
    m_axisX->setTitleText(tr("次数"));
    m_axisY->setTitleText(tr("分数"));

    m_axisX->setMinorTickCount(3);

    m_chart->setAxisX(m_axisX,m_series);
    m_chart->setAxisY(m_axisY,m_series);

    //修改说明样式
    m_chart->legend()->setVisible(false);
    m_chartview->setRenderHint(QPainter::Antialiasing);

}

void ActiveTrainingReport::on_close_btn_clicked()
{
    this->close();
}

void ActiveTrainingReport::on_noSave_btn_clicked()
{
    this->close();
}

void ActiveTrainingReport::on_save_btn_clicked()
{
    //将用户添加到数据库中
    st_trainreport.markMsg = ui->markMsg_TextEdit->toPlainText();
    //将报告的图片存储在本地
    QString dirPath(QApplication::applicationDirPath() + "/ReportDialog/");
    QDir dir(dirPath);
    if(!dir.exists())
        dir.mkdir(dirPath);
    //设置保存的图片的名字以及路径
    QString filePath = dirPath + QString::number(st_trainreport.ID) + "_" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".png";
    QPixmap pixmap = ui->groupBox_2->grab();
    st_trainreport.resultImagePath = filePath;
    if(pixmap.save(filePath) && saveReportTable(st_trainreport))
    {
        QMessageBox::warning(NULL,tr("提示"),tr("保存成功"));
    }
    else
        QMessageBox::warning(NULL,tr("提示"),tr("保存失败"));

}

void ActiveTrainingReport::on_print_btn_clicked()
{



}

void ActiveTrainingReport::setTrainReport(const ST_TrainReport&st_TrainReport)
{
    st_trainreport = st_TrainReport;
    ST_PatientMsg st_PatientMsg;
    st_PatientMsg = CurrentUserData::getInstace()->getCurrentPatientMsg();
    ui->score_label->setText(QString::number(st_TrainReport.score));

    ui->progressBar->setValue(st_TrainReport.level*10);
    //训练模式
    switch(st_TrainReport.trainMode)
    {
    case TRAINMODE_PASSIVE_E://被动模式
        ui->trainMode_label->setText(tr("被动模式"));
        //训练速度
        ui->trainSpeed_label->setText(QString::number(st_TrainReport.speed));
        ui->speed_GroupBox->setVisible(true);
        break;
    case TRAINMODE_ACTIVE_E://主动模式
        ui->trainMode_label->setText(tr("主动模式"));
        ui->speed_GroupBox->setVisible(false);
        break;
    }
    //运动范围
    ui->highMargin_label->setText(QString::number(st_TrainReport.highMargin) + "°");
    ui->lowMargin_label->setText(QString::number(st_TrainReport.lowMargin) + "°");

    //训练选项
    switch (st_TrainReport.trainTypeE)
    {
    case COMPREHENSIVE_E:
        ui->trainOption_label->setText(tr("综合训练"));
        break;
    case PLANTAR_FLEXION_E:
        ui->trainOption_label->setText(tr("跖屈背屈"));
        break;
    case ADDUCTION_ABDUCTION_E:
        ui->trainOption_label->setText(tr("内收外展"));
        break;
    case VARUS_VALGUS_E:
        ui->trainOption_label->setText(tr("内翻外翻"));
        break;
    case SLOPE_PLANTAR_FLEXION_E:
        ui->trainOption_label->setText(tr("斜跖屈背屈"));
        break;
    case PASSIVE_TRAIN_CIRCLE:
        ui->trainOption_label->setText(tr("圆周运动"));
        break;
    case PASSIVE_TRAIN_SUB:
        ui->trainOption_label->setText(tr("分段运动"));
        break;
    }
    ui->name_label->setText(st_PatientMsg.name);
    ui->sex_label->setText(QString::number(st_PatientMsg.sex));
    QPixmap pixmap;
    if(0 == st_PatientMsg.sex)
    {
        ui->sex_label->setText(tr("男"));
        pixmap.load(":/images/photo_man.png");
    }
    else if(1 == st_PatientMsg.sex )
    {
        ui->sex_label->setText(tr("女"));
        pixmap.load(":/images/photo_woman.png");
    }
    ui->profile_Label->setPixmap(pixmap);

    ui->age_label->setText(QString::number(st_PatientMsg.age));
    ui->phoneNumber_label->setText(st_PatientMsg.phone);
    ui->trainTime_label->setText(st_TrainReport.startTime.toString("yyyy-MM-dd hh:mm"));

    ui->markMsg_TextEdit->setText(st_TrainReport.markMsg);
    //周期次数（根据得分计算）
    ui->cyclesNumber_label->setText(QString::number(st_TrainReport.score/10));
    //训练时长
    int totalTime = st_TrainReport.duration/60;
    if(totalTime < 1)
        totalTime = 1;

    ui->trainSumTime_label->setText(QString::number(totalTime));

    //查询数据库获取最近十次的得分
    QString query(QString("select * from TrainReport where ID = '%1' order by startTime").arg(st_trainreport.ID));
    CDatabaseInterface *dataBase = CDatabaseInterface::getInstance();
    if(dataBase->exec(query))
    {
        int count = CDatabaseInterface::getInstance()->getValuesSize();
        if(count > 10)
            count = 10;
        QList<QVariantMap> valueList = dataBase->getValues(0,count);

        ST_TrainReport st_trainReport;
        QList<QPair<QDateTime,int>> pointList;
        for(int i = 0;i < valueList.size();i++)
        {
             st_trainReport = variantMapToTrainReport(valueList.at(i));
             QPair<QDateTime,int> scorePair;
             scorePair.first = st_trainReport.startTime;
             scorePair.second = st_trainReport.score;
             pointList.append(scorePair);
        }

        trainTimeChart(pointList);
    }
    else
    {
        qDebug()<<dataBase->getLastError();
    }
    this->show();
}

//报告表数据接口
bool ActiveTrainingReport::saveReportTable(ST_TrainReport st_trainreport)
{
    QString tableName("TrainReport");
    QVariantMap vMap = trainReportToVariantMap(st_trainreport);
    if(!CDatabaseInterface::getInstance()->insertRowTable(tableName,vMap))
    {
        qDebug()<<CDatabaseInterface::getInstance()->getLastError();
        return false;
    }
    return true;
}
