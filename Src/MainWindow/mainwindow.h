#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "usermanager.h"
class QTimer;
class ResetWidget;
class QLabel;
class CCommunicationInterface;
class QButtonGroup;
class UserManager;
class QPushButton;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void enterUserManagerPage();
protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void on_passiveTrain_Btn_clicked();

    void on_activeTrain_Btn_clicked();

    void on_evaluate_Btn_clicked();

    void on_record_Btn_clicked();

    void on_shutDown_Btn_clicked();

    void on_setting_Btn_clicked();

    void slotCurrentStackWidgetChanged(int);

    void slotBtnGroupClicked(int);

    /****切换界面********
     * 参数：@int index  0-trainPage  1-scorpPage 2-teachPage
     *      3-historyPage 4-rankingPage 5-settingPage
     * 返回值：void
     * ************/
    void slotSwitchPage(int index);

    void on_switchUser_Btn_clicked();

    //显示急停效果槽
    void slotShowEmergency(bool);
    //急停闪烁效果
    void slotEmergencyBlink();

    //显示复位弹框
    void slotResetDialog();

public slots:
    void slotCurrentUserChanged();
    void receiveCommData(QByteArray);
    //心跳槽函数
    void slotHeartBeatSlot();
private:

    void initButtonGroup();

    void initWidgetUI();

    void setCurrentUser();

    void showEmergency(bool);

protected:
    void hideEvent(QHideEvent *event);
private:
    Ui::MainWindow *ui;
    QButtonGroup *m_buttonGroup;
    UserManager *m_userManager;
    QLabel *m_emergencyLabel;
    QTimer *m_emerTimer;

    QTimer *m_resetTimer;
    QList<QPair<QString,QString>> m_btnQssList;
};
#endif // MAINWINDOW_H
