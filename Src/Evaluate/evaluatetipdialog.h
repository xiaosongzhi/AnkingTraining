#ifndef EVALUATETIPDIALOG_H
#define EVALUATETIPDIALOG_H

#include <QWidget>

namespace Ui {
class EvaluateTipDialog;
}

class EvaluateTipDialog : public QWidget
{
    Q_OBJECT

public:
    explicit EvaluateTipDialog(QWidget *parent = nullptr);
    ~EvaluateTipDialog();

private:
    Ui::EvaluateTipDialog *ui;
};

#endif // EVALUATETIPDIALOG_H
