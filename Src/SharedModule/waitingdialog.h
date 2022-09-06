#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>
#include "dataFormate.h"

class QMovie;
namespace Ui {
class WaitingDialog;
}

class WaitingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitingDialog(QWidget *parent = nullptr);
    ~WaitingDialog();

    void setWaitingType(E_WaitingType);

    void setDialogCloseState(bool isClose);
private:
    void initGif();
    void loadStyleSheetFile();
protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
private slots:
    void on_pushButton_clicked();

private:
    Ui::WaitingDialog *ui;
    QMovie *m_movie;
};

#endif // WAITINGDIALOG_H
