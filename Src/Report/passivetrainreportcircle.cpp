#include "passivetrainreportcircle.h"
#include "ui_passivetrainreportcircle.h"

PassiveTrainReportCircle::PassiveTrainReportCircle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PassiveTrainReportCircle)
{
    ui->setupUi(this);
}

PassiveTrainReportCircle::~PassiveTrainReportCircle()
{
    delete ui;
}

void PassiveTrainReportCircle::on_noSave_btn_clicked()
{

}
