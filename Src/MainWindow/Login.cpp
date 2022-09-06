#include "login.h"
#include "ui_Login.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //设置为模态对话框
    setModal(true);
    connect(this,&Login::signalResult,this,&Login::done);
    ui->userName_Label->setVisible(false);
    ui->userPassword_Label->setVisible(false);
    ui->userName_lineEdit->setText("JCR");
    ui->userPassword_lineEdit->setText("123456");
    ui->userPassword_lineEdit->setEchoMode(QLineEdit::Password);

}

Login::~Login()
{
    delete ui;
}

void Login::on_confirm_btn_clicked()
{
    ui->userName_Label->setVisible(false);
    ui->userPassword_Label->setVisible(false);

    if(ui->userName_lineEdit->text() != "JCR")
        ui->userName_Label->setVisible(true);
    if(ui->userPassword_lineEdit->text() != "123456")
        ui->userPassword_Label->setVisible(true);

    if(ui->userName_lineEdit->text() == "JCR" && ui->userPassword_lineEdit->text() == "123456")
    {
        emit signalResult(3);
        this->close();
    }
}
