#ifndef PASSIVETRAINREPORTCIRCLE_H
#define PASSIVETRAINREPORTCIRCLE_H

#include <QWidget>

namespace Ui {
class PassiveTrainReportCircle;
}

class PassiveTrainReportCircle : public QWidget
{
    Q_OBJECT

public:
    explicit PassiveTrainReportCircle(QWidget *parent = nullptr);
    ~PassiveTrainReportCircle();

private slots:
    void on_noSave_btn_clicked();

private:
    Ui::PassiveTrainReportCircle *ui;
};

#endif // PASSIVETRAINREPORTCIRCLE_H
