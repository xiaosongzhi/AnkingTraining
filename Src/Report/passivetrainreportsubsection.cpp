#include "passivetrainreportsubsection.h"
#include "ui_passivetrainreportsubsection.h"

PassiveTrainReportSubsection::PassiveTrainReportSubsection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PassiveTrainReportSubsection)
{
    ui->setupUi(this);
}

PassiveTrainReportSubsection::~PassiveTrainReportSubsection()
{
    delete ui;
}
