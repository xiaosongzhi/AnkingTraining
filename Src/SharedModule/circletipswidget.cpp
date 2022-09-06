#include "circletipswidget.h"
#include "ui_circletipswidget.h"
#include <QPainter>
#include <QTimer>
#include <QDir>
#include <QFileInfoList>
CircleTipsWidget::CircleTipsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CircleTipsWidget),
    step1Index(0),
    step2Index(0),
    step3Index(0)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground,true);//设置无边框

    playerTimer = new QTimer();
    playerTimer->setInterval(300);
    connect(playerTimer,SIGNAL(timeout()),this,SLOT(slotPlayerTimer()));
}

CircleTipsWidget::~CircleTipsWidget()
{
    if(playerTimer)
    {
        playerTimer->stop();
        delete playerTimer;
    }
    delete ui;
}
void CircleTipsWidget::switchTipsType(E_VideoTipsType type)
{
    m_videoTipsType = type;
    switch(type)
    {
    case PASSIVE_TRAIN_TIPS://被动训练
    {
        setStepPictureList(1,"./Source/activepassive/active2passive");
        setStepPictureList(2,"./Source/resistance/red2green");
        setStepPictureList(3,"./Source/silder");
        setTipsMessage(1,tr("请将离合切换到被动位置"));
        setTipsMessage(2,tr("请将旋钮由红色旋转至绿色位置"));
        setTipsMessage(3,tr("请将滑块移至中央位置"));
    }
        break;
    case COMPREHENSIVE_TIPS_E://综合评估
    {
        setStepPictureList(1,"./Source/activepassive/passive2active");
        setStepPictureList(2,"./Source/resistance/red2green");
        setStepPictureList(3,"./Source/silder");
        setTipsMessage(1,tr("请将离合切换到主动位置"));
        setTipsMessage(2,tr("请将旋钮由红色旋转至绿色位置"));
        setTipsMessage(3,tr("请将滑块移至中央位置"));
    }
        break;
    }
    switchPlay(true);
}

void CircleTipsWidget::slotPlayerTimer()
{
    QPixmap step1Pixmap,step2Pixmap,step3Pixmap;
    if(step1PictureList.size() > step1Index)
    {
        step1Pixmap.load(step1PictureList.at(step1Index));
        ui->step1Picture_Label->setPixmap(step1Pixmap);
        step1Index++;
        if(step1Index >= step1PictureList.size())
            step1Index = 0;
    }

    if(step2PictureList.size() > step2Index)
    {
        step2Pixmap.load(step2PictureList.at(step2Index));
        ui->step2Picture_Label->setPixmap(step2Pixmap);
        step2Index++;
        if(step2Index >= step2PictureList.size())
            step2Index = 0;
    }

    if(step3PictureList.size() > step3Index)
    {
        step3Pixmap.load(step3PictureList.at(step3Index));
        ui->step3Picture_Label->setPixmap(step3Pixmap);
        step3Index++;
        if(step3Index >= step3PictureList.size())
            step3Index = 0;
    }
}

void CircleTipsWidget::setStepPictureList(int index,QString dirPath)
{
    QDir dir(dirPath);
    dir.setSorting(QDir::NoSort);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);

    switch(index)
    {
    case 1: //左侧
    {
        step1PictureList.clear();
        for(int i = 0;i < fileList.size();i++)
            step1PictureList.append(fileList.at(i).absoluteFilePath());
    }
        break;
    case 2: //中侧
    {
        step2PictureList.clear();
        for(int i = 0;i < fileList.size();i++)
            step2PictureList.append(fileList.at(i).absoluteFilePath());
    }
        break;
    case 3: //右侧
    {
        step3PictureList.clear();
        for(int i = 0;i < fileList.size();i++)
            step3PictureList.append(fileList.at(i).absoluteFilePath());
    }
        break;
    }
}

void CircleTipsWidget::switchPlay(bool isPlay)
{
    if(isPlay)
    {
        step1Index = 0;
        step2Index = 0;
        step3Index = 0;
        playerTimer->start();
    }
    else
    {
        playerTimer->stop();
    }
}

void CircleTipsWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

}

void CircleTipsWidget::setTipsMessage(int index,QString msg)
{
    if(1 == index)
    {
        ui->tip1_Label->setText(msg);
    }
    else if(2== index)
    {
        ui->tip2_Label->setText(msg);
    }
    else if(3 == index)
    {
        ui->tip3_Label->setText(msg);
    }
}

void CircleTipsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(),QColor(0,0,0,100));
}

void CircleTipsWidget::on_confirm_Btn_clicked()
{
    switchPlay(false);

    emit signalConfirmed(m_videoTipsType);
    this->hide();
}
