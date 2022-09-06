#ifndef USERDATAFORMATE_H
#define USERDATAFORMATE_H

//此处存放与数据库有关的内容

#include <QWidget>
#include <QDateTime>
#include "dataFormate.h"
#define PERCOUNT 8  //每页放8个处方

//用户信息
typedef struct PatientMsg
{
    uint32_t ID;                    //自增性ID，由系统分配控制
    QString name;                   //患者姓名 必填
    int age;                        //患者年龄
    int sex;                        //0-男性 1-女性
    int height;                     //患者身高
    int weight;                     //患者体重
    int footIndex;                  //左右脚选择 0-左脚  1-右脚
    QString phone;                  //患者电话
    QString address;                //家庭住址
    QString remarkMessage;          //备注信息
    QString userInfo;               //用户信息

}ST_PatientMsg;

//训练报告
typedef struct TrainReport
{
    QString UUID;
    int ID;                 //系统ID
    QString name;           //患者姓名
    int level;              //当前等级
    QDateTime startTime;    //开始时间
    QDateTime endTime;      //结束时间
    int trainMode;          //训练模式  0-被动训练 1-主动训练
//    int trainTrack;       //训练轨迹 E_TrainEvaluationType
    int highMargin;         //上边界（可代表圆周也可代表非圆周）
    int lowMargin;          //下边界（可代表圆周也可代表非圆周）
    int score;              //得分
    int duration;           //本次训练时长(分钟)
    int totalDuration;      //训练总时长 最近一次训练的+本次时长
    QString markMsg;        //备注
    QString resultImagePath;//图片结果路径
    QString gameName;       //游戏
    E_TrainEvaluationType trainTypeE; //训练选项枚举
    int times;              //训练次数
    float speed;            //训练速度
}ST_TrainReport;

//评估记录
typedef struct EvaluateRecord
{
    QString UUID;
    int ID;                 //系统ID
    QString name;           //患者姓名
    QDateTime startTime;    //开始时间
    QDateTime endTime;      //结束时间
    int evaluateMode;       //评估模式
    int highMargin;         //上边界（可代表圆周也可代表非圆周）
    int lowMargin;          //下边界（可代表圆周也可代表非圆周）

}ST_EvaluateRecord;

extern ST_PatientMsg variantMapToPatientMsg(QVariantMap vMap);
extern ST_EvaluateRecord variantMapToEvaluateRecord(QVariantMap vMap);
extern ST_TrainReport variantMapToTrainReport(QVariantMap vMap);

extern QVariantMap patientMsgToVariantMap(ST_PatientMsg);
extern QVariantMap evaluateRecordToVariantMap(ST_EvaluateRecord);
extern QVariantMap trainReportToVariantMap(ST_TrainReport);


#endif // USERDATAFORMATE_H
