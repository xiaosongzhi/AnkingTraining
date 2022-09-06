#include "evaluatetipdialog.h"
#include "ui_evaluatetipdialog.h"

EvaluateTipDialog::EvaluateTipDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EvaluateTipDialog)
{
    ui->setupUi(this);
}

EvaluateTipDialog::~EvaluateTipDialog()
{
    delete ui;
}
