#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
signals:
    void signalResult(int);
private slots:
    void on_confirm_btn_clicked();

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
