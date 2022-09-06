#include "currentuserdata.h"
#include "cdatabaseinterface.h"
#include <QDebug>


#pragma execution_character_set("utf-8")


CurrentUserData* CurrentUserData::m_currentUserData = NULL;

CurrentUserData::CurrentUserData(QObject *parent)
    : QObject(parent)

{

}

CurrentUserData* CurrentUserData::getInstace()
{
    if(!m_currentUserData)
    {
        m_currentUserData = new CurrentUserData();
    }
    return m_currentUserData;
}

//设置当前用户ID,在选择用户的时候调用
void CurrentUserData::setCurrentUserID(uint32_t ID)
{
    m_CurrentUserID = ID;

    qDebug()<<"更新显示用户";
    emit signalUserChanged();
}

//获取患者信息
ST_PatientMsg CurrentUserData::getCurrentPatientMsg()
{
    updateCurrentPatientMsg();
    return st_CurrentUserData;
}

void CurrentUserData::updateCurrentPatientMsg()
{
    //从数据库中查询数据
    QString queryStr= QString("select * from PatientMsg where ID = '%1'").arg(m_CurrentUserID);

    if(CDatabaseInterface::getInstance()->exec(queryStr))
    {
        if(CDatabaseInterface::getInstance()->getValuesSize() > 0)
        {
            QList<QVariantMap> valueMapList;
            valueMapList = CDatabaseInterface::getInstance()->getValues(0,1);
            st_CurrentUserData = variantMapToPatientMsg(valueMapList.at(0));
            //更新训练用户
            qDebug()<<"更新训练用户";
        }
        else
        {
            qDebug()<<"未查询到符合条件的数据";
        }
    }
    else
    {
        qDebug()<<"updateCurrentPatientMsg failed"<<CDatabaseInterface::getInstance()->getLastError();
    }
}

void CurrentUserData::updateTrainUser()
{
    emit signalTrainUserChanged();
}

void CurrentUserData::setCurrentUserMsg(ST_PatientMsg st_patientMsg)
{
    setCurrentUserID(st_patientMsg.ID);
}

uint32_t CurrentUserData::getCurrentUserID()
{
    return m_CurrentUserID;
}


