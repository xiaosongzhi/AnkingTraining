#ifndef RESETWIDGET_H
#define RESETWIDGET_H

#include <QWidget>

namespace Ui {
class ResetWidget;
}

class ResetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResetWidget(QWidget *parent = nullptr);
    ~ResetWidget();
protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *event);
private slots:
    void receiveCommData(QByteArray);
    void on_confirm_Btn_clicked();
    void on_close_Btn_clicked();
private:
    //复位设备
    void resetDevice();
    //复位前
    void setBeforeResetState();
    //复位中
    void setResettingState();
private:
    Ui::ResetWidget *ui;
};

#endif // RESETWIDGET_H
