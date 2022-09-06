#ifndef PASSIVETRAINREPORTSUBSECTION_H
#define PASSIVETRAINREPORTSUBSECTION_H

#include <QWidget>

namespace Ui {
class PassiveTrainReportSubsection;
}

class PassiveTrainReportSubsection : public QWidget
{
    Q_OBJECT

public:
    explicit PassiveTrainReportSubsection(QWidget *parent = nullptr);
    ~PassiveTrainReportSubsection();

private:
    Ui::PassiveTrainReportSubsection *ui;
};

#endif // PASSIVETRAINREPORTSUBSECTION_H
