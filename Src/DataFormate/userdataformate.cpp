#include "userdataformate.h"
#include <QVariantMap>
#include <QMapIterator>
#include <QUuid>
#include <QDebug>
extern ST_PatientMsg variantMapToPatientMsg(QVariantMap vMap)
{
    ST_PatientMsg st_PatientMsg;
    if(vMap.contains("ID"))
        st_PatientMsg.ID = vMap.value("ID").toUInt();
    if(vMap.contains("name"))
        st_PatientMsg.name = vMap.value("name").toString();
    if(vMap.contains("age"))
        st_PatientMsg.age = vMap.value("age").toUInt();
    if(vMap.contains("sex"))
        st_PatientMsg.sex = vMap.value("sex").toUInt();
    if(vMap.contains("height"))
        st_PatientMsg.height = vMap.value("height").toUInt();
    if(vMap.contains("weight"))
        st_PatientMsg.weight = vMap.value("weight").toUInt();
    if(vMap.contains("footIndex"))
        st_PatientMsg.footIndex = vMap.value("footIndex").toInt();
    if(vMap.contains("phone"))
        st_PatientMsg.phone = vMap.value("phone").toString();
    if(vMap.contains("address"))
        st_PatientMsg.address = vMap.value("address").toString();
    if(vMap.contains("remarkMessage"))
        st_PatientMsg.remarkMessage = vMap.value("remarkMessage").toString();
    if(vMap.contains("userInfo"))
        st_PatientMsg.userInfo = vMap.value("userInfo").toString();
    return st_PatientMsg;
}

extern ST_EvaluateRecord variantMapToEvaluateRecord(QVariantMap vMap)
{
    ST_EvaluateRecord st_EvaluateRecord;
    if(vMap.contains("UUID"))
        st_EvaluateRecord.UUID = vMap.value("UUID").toString();
    if(vMap.contains("ID"))
        st_EvaluateRecord.ID = vMap.value("ID").toUInt();
    if(vMap.contains("name"))
        st_EvaluateRecord.name = vMap.value("name").toString();
    if(vMap.contains("startTime"))
        st_EvaluateRecord.startTime = vMap.value("startTime").toDateTime();
    if(vMap.contains("endTime"))
        st_EvaluateRecord.startTime = vMap.value("endTime").toDateTime();
    if(vMap.contains("evaluateMode"))
        st_EvaluateRecord.evaluateMode = vMap.value("evaluateMode").toUInt();
    if(vMap.contains("highMargin"))
        st_EvaluateRecord.highMargin = vMap.value("highMargin").toUInt();
    if(vMap.contains("lowMargin"))
        st_EvaluateRecord.lowMargin = vMap.value("lowMargin").toUInt();
    return st_EvaluateRecord;
}

extern ST_TrainReport variantMapToTrainReport(QVariantMap vMap)
{
    ST_TrainReport st_TrainReport;
    if(vMap.contains("UUID"))
        st_TrainReport.UUID = vMap.value("UUID").toString();
    if(vMap.contains("ID"))
        st_TrainReport.ID = vMap.value("ID").toInt();
    if(vMap.contains("name"))
        st_TrainReport.name = vMap.value("name").toString();
    if(vMap.contains("level"))
        st_TrainReport.level = vMap.value("level").toInt();
    if(vMap.contains("startTime"))
        st_TrainReport.startTime = vMap.value("startTime").toDateTime();
    if(vMap.contains("endTime"))
        st_TrainReport.endTime = vMap.value("endTime").toDateTime();
    if(vMap.contains("trainMode"))
        st_TrainReport.trainMode = (E_TRAINMODE)vMap.value("trainMode").toInt();
//    if(vMap.contains("trainTrack"))
//        st_TrainReport.trainTrack = vMap.value("trainTrack").toInt();
    if(vMap.contains("highMargin"))
        st_TrainReport.highMargin = vMap.value("highMargin").toInt();
    if(vMap.contains("duration"))
        st_TrainReport.duration = vMap.value("duration").toInt();
    if(vMap.contains("lowMargin"))
        st_TrainReport.lowMargin = vMap.value("lowMargin").toInt();
    if(vMap.contains("score"))
        st_TrainReport.score = vMap.value("score").toInt();
    if(vMap.contains("duration"))
        st_TrainReport.duration = vMap.value("duration").toInt();
    if(vMap.contains("totalDuration"))
        st_TrainReport.totalDuration = vMap.value("totalDuration").toInt();
    if(vMap.contains("markMsg"))
        st_TrainReport.markMsg = vMap.value("markMsg").toString();
    if(vMap.contains("resultImagePath"))
        st_TrainReport.resultImagePath = vMap.value("resultImagePath").toString();
    if(vMap.contains("game"))
        st_TrainReport.gameName = vMap.value("game").toString();
    if(vMap.contains("trainType"))
        st_TrainReport.trainTypeE = (E_TrainEvaluationType)vMap.value("trainType").toUInt();
    if(vMap.contains("times"))
        st_TrainReport.times = vMap.value("times").toUInt();
    if(vMap.contains("speed"))
        st_TrainReport.speed = vMap.value("speed").toFloat();
    return st_TrainReport;
}

extern QVariantMap patientMsgToVariantMap(ST_PatientMsg st_PatientMsg)
{
    QVariantMap vMap;
    vMap.insert("ID",st_PatientMsg.ID);
    vMap.insert("name",st_PatientMsg.name);
    vMap.insert("age",st_PatientMsg.age);
    vMap.insert("sex",st_PatientMsg.sex);
    vMap.insert("height",st_PatientMsg.height);
    vMap.insert("weight",st_PatientMsg.weight);
    vMap.insert("footIndex",st_PatientMsg.footIndex);
    vMap.insert("phone",st_PatientMsg.phone);
    vMap.insert("address",st_PatientMsg.address);
    vMap.insert("remarkMessage",st_PatientMsg.remarkMessage);
    vMap.insert("userInfo",st_PatientMsg.userInfo);
    return vMap;
}

extern QVariantMap trainReportToVariantMap(ST_TrainReport st_TrainReport)
{
    QVariantMap vMap;
    //添加UUID
    QUuid id = QUuid::createUuid();
    QString strId = id.toString(QUuid::Id128);
    vMap.insert("UUID",strId);
    vMap.insert("ID",st_TrainReport.ID);
    vMap.insert("name",st_TrainReport.name);
    vMap.insert("level",st_TrainReport.level);
    vMap.insert("startTime",st_TrainReport.startTime);
    vMap.insert("endTime",st_TrainReport.endTime);
    vMap.insert("trainMode",st_TrainReport.trainMode);
//    vMap.insert("trainTrack",st_TrainReport.trainTrack);
    vMap.insert("highMargin",st_TrainReport.highMargin);
    vMap.insert("lowMargin",st_TrainReport.lowMargin);
    vMap.insert("score",st_TrainReport.score);
    vMap.insert("duration",st_TrainReport.duration);
    vMap.insert("totalDuration",st_TrainReport.totalDuration);
    vMap.insert("markMsg",st_TrainReport.markMsg);
    vMap.insert("resultImagePath",st_TrainReport.resultImagePath);
    vMap.insert("game",st_TrainReport.gameName);
    vMap.insert("trainType",(int)st_TrainReport.trainTypeE);
    vMap.insert("times",st_TrainReport.times);
    vMap.insert("speed",st_TrainReport.speed);
    return vMap;
}

extern QVariantMap evaluateRecordToVariantMap(ST_EvaluateRecord st_EvaluateRecord)
{
    QVariantMap vMap;
    //添加UUID
    QUuid id = QUuid::createUuid();
    QString strId = id.toString(QUuid::Id128);
    vMap.insert("UUID",strId);
    vMap.insert("ID",st_EvaluateRecord.ID);
    vMap.insert("name",st_EvaluateRecord.name);
    vMap.insert("startTime",st_EvaluateRecord.startTime);
    vMap.insert("endTime",st_EvaluateRecord.endTime);
    vMap.insert("evaluateMode",st_EvaluateRecord.evaluateMode);
    vMap.insert("highMargin",st_EvaluateRecord.highMargin);
    vMap.insert("lowMargin",st_EvaluateRecord.lowMargin);

    return vMap;
}


