#ifndef CURRENTUSERDATA_H
#define CURRENTUSERDATA_H

#include <QObject>
#include "userdataformate.h"


class CurrentUserData : public QObject
{
    Q_OBJECT
public:
    static CurrentUserData* getInstace();

    //设置当前用户ID,在选择用户的时候调用
    void setCurrentUserID(uint32_t ID);
    uint32_t getCurrentUserID();

    //获取患者信息
    ST_PatientMsg getCurrentPatientMsg();
    void setCurrentUserMsg(ST_PatientMsg);

    //更新训练用户
    void updateTrainUser();
signals:
    //当前用户显示发生变化
    void signalUserChanged();
    //训练用户发生变化
    void signalTrainUserChanged();

private:
    void updateCurrentPatientMsg();

private:
    explicit CurrentUserData(QObject *parent = nullptr);

    static CurrentUserData* m_currentUserData;
    //当前用户ID
    uint32_t m_CurrentUserID;

    //当前用户信息
    ST_PatientMsg st_CurrentUserData;

};

#endif // CURRENTUSERDATA_H
