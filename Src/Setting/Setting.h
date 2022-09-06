#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QButtonGroup>
#include <QTranslator>
namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();

private slots:

    void on_hospitalInformation_btn_clicked();

    void on_passwordManager_btn_clicked();

    void on_productPresentation_btn_clicked();

    void slotBtnGroupClicked(int);

    void on_save_btn_clicked();

    void on_chinese_RadioButton_toggled(bool checked);

    void on_english_RadioButton_toggled(bool checked);

    void on_systemSetting_Btn_clicked();
protected:
    virtual void changeEvent(QEvent* event);
private:
    void initButtonGroup();
private:
    Ui::Setting *ui;
    QButtonGroup *m_buttonGroup;
    QTranslator *m_translator;
};

#endif // SETTING_H
