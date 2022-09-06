#ifndef SWITCHACTIVEPASSIVEWIDGET_H
#define SWITCHACTIVEPASSIVEWIDGET_H

#include <QWidget>
#include "dataFormate.h"
namespace Ui {
class SwitchActivePassiveWidget;
}

class SwitchActivePassiveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SwitchActivePassiveWidget(QWidget *parent = nullptr);
    ~SwitchActivePassiveWidget();

    //切换主被动提示
    void switchTipsType(E_VideoTipsType);
signals:
    //点击确认
    void signalConfirmed(E_VideoTipsType);

protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_confirm_Btn_clicked();

private:
    Ui::SwitchActivePassiveWidget *ui;
    E_VideoTipsType m_videoTipsType;
};

#endif // SWITCHACTIVEPASSIVEWIDGET_H
