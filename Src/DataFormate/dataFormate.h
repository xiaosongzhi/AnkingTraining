#ifndef DATAFORMATE_H
#define DATAFORMATE_H
#include <QWidget>
#include <QMap>
#define WIDTH 630
#define HEIGHT 450

#define PACKHEAD 0xFE
typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

//用于生成报表数据
typedef  QMap<QDateTime,int> ReportData;
typedef  QList<ReportData> ReportDataList;

//绘图类型
typedef enum
{
    AreaChart_E = 0,
    BarChart_E,
    PieChart_E,
    LineChart_E,
    SplineChart_E,
    ScatterChart_E
}E_ChartType;
typedef enum
{
    X_Axis = 0,
    Y_Axis
}E_AxisType;
typedef enum
{
    INITWAITING,    //加载中
    LOADINGWITING,  //复位中
}E_WaitingType;

typedef enum
{
    GameStart_E = 0,    //启动游戏
    GameEnd_E           //游戏停止
}E_GameState;

//页面切换
typedef enum
{
    ActiveTrainingPage_E = 0,   //主动训练
    PassiveTrainingPage_E,      //被动训练
    EvaluationPage_E,           //评估界面
    RecordPage_E,               //记录界面
    SettingPage_E,              //设置界面
    EmptyPage_E
}E_PAGENAME;

//通信状态
typedef enum
{
    UnconnectedState_E = 0,
    HostLookupState_E,
    ConnectingState_E,
    ConnectedState_E,
    BoundState_E,
    ClosingState_E,
    ListeningState_E

}E_DeviceState;


//评估/训练类型
typedef enum
{
    COMPREHENSIVE_E  = 0,	//综合主动||评估(圆周)
    PLANTAR_FLEXION_E,		//跖屈背屈主动||评估
    ADDUCTION_ABDUCTION_E,	//内收外展主动||评估
    VARUS_VALGUS_E,			//内翻外翻主动||评估
    SLOPE_PLANTAR_FLEXION_E,//斜跖屈背屈主动||评估
    PASSIVE_TRAIN_CIRCLE,   //被动训练圆周
    PASSIVE_TRAIN_SUB       //被动训练分段

}E_TrainEvaluationType;

//视频提示类型
typedef enum
{
    COMPREHENSIVE_TIPS_E  = 0,	 //综合(圆周)【主动】
    PLANTAR_FLEXION_TIPS_E,		 //跖屈背屈【被动】
    ADDUCTION_ABDUCTION_TIPS_E,	 //内收外展【被动】
    VARUS_VALGUS_TIPS_E,		 //内翻外翻【被动】
    SLOPE_PLANTAR_FLEXION_TIPS_E,//斜跖屈背屈【被动】
    PASSIVE_TRAIN_TIPS,          //被动训练提示【被动】
    ACTIVE_TRAIN_TIPS,           //主动训练提示【主动】
    INITING_TIPS,                //初始化提示【被动】
    GYROSCOPE_CHECK_TIPS         //陀螺仪校准
}E_VideoTipsType;

//页面切换 2021.11.15
typedef enum
{
    ERROR_EMERGENCY = 0X0000,           //急停报警
    ERROR_CHECKRESULT = 0X0001,         //复位按钮
    ERROR_DEVICEERROR = 0X0002,         //设备报警
    ERROR_SPASMWARNING = 0X0003,        //痉挛报警
    SPASM_ERROR_RESET = 0X0004,         //痉挛复位
    CONTROL_PARAMSET = 0x0100,          //评估模式设定
    MACHINE_ZERO_CHECK= 0x0100,         //机械零点校准
    GYROSCOPE_CHECK = 0X0101,           //陀螺仪校准
    GET_GYROSCOPE_DATA =0X0102,         //获取陀螺仪数据
    GIVE_GYROSCOPE_DATA = 0X0103,       //给出陀螺仪数据
    GET_CIRCLE_ANGLE = 0x0104,          //获取圆周角度
    EVALUATE_CONTROL = 0X1000,          //评估控制
    ARRIVE_EVALUATE_POSITION = 0X1001,  //到达指定评估位置
    SWITCH_PASSIVE_TRAINNING = 0X2000,  //被动训练开启、关闭
    ARRIVED_PASSIVE_POSITION = 0X2001,  //下位机到达被动训练位置
    SET_PASSIVE_PARAMETER = 0X2002,     //设置被动训练参数
    PASSIVE_DEST_POSITION=0X2003,       //被动训练目标数据
    SWITCH_ACTIVE_TRAINNING = 0X3000,   //主动训练开启、关闭
    SET_ACTIVE_TRAINNING_TYPE = 0X3001, //设置主动训练类型值
    SET_ACTIVE_PARAMETER = 0X3002,      //设置主动训练参数
    ARRIVED_ACTIVE_POSITION = 0x3003,   //到达主动训练位置
    DEBUG_CMD = 0x3004,                 //调试指令（被动退出）
    HEART_BEAT_CMD = 0XF000,             //心跳
    QUIT_CMD = 0XF001                    //退出各种状态
}E_CMDID;

//训练模式
typedef enum
{
    TRAINMODE_PASSIVE_E = 0,    //被动
    TRAINMODE_ACTIVE_E,         //主动
}E_TRAINMODE;

//串口配置
typedef struct ST_SerialPortConfig
{
    QString portName;   //端口名
    int32_t baud;       //波特率
    int8_t dataBits;    //数据位
    int8_t parity;      //奇偶校验
    int8_t stopBit;     //停止位
    int8_t flowControl; //流控
    ST_SerialPortConfig()
    {
        portName = "";
        baud = 0;
        dataBits = 0;
        parity = 0;
        stopBit = 0;
        flowControl = 0;
    }

}ST_SerialPortConfig;

//数据库配置
typedef struct ST_DataBaseConfig
{
    QString IP;
    int16_t port;
    QString userName;   //用户名
    QString password;   //密码
    ST_DataBaseConfig()
    {
        IP = "";
        port = 0;
        userName = "";
        password = "";
    }

}ST_DataBaseConfig;


#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为1字节对齐

//子结构体
typedef struct {
    float x;    //N roll
    float y;    //E pitch
    float z;    //U yaw
}ST_Vector3f;

typedef struct {
    float pitch;    //y  俯仰角
    float roll;     //x  横滚角
    float yaw;      //z  偏航角
}ST_Euler3f;

/********主结构体***********/
//陀螺仪数据
typedef struct
{
    ST_Euler3f  st_euler;       //欧拉角
    float coderAngle;           //编码器角度
} ST_GyroscopeData;

//评估控制
typedef struct
{
    uint8_t evaluateType;//评估类型
    uint8_t switchEvaluate;//1-开启  0-退出
}ST_EvaluateControl;

/*  被动训练参数结构体  */
typedef struct
{
    int Angle;         //运行角度 圈数可转化成角度
    int speed;           //速度档位 1~6
}ST_PassiveParam;

//被动训练参数
typedef struct
{
    E_TrainEvaluationType trackType;   //轨迹类型 5-被动圆周  6-被动分段运动
    uint circleNum;     //运动圈数（trackType=5时有效）
    int minAngle;       //最小角度（trackType=6时有效）
    int maxAngle;       //最大角度（trackType=6时有效）
    int speed;          //速度档位 1~6
    int intervalTime;   //间歇时间 单位 秒(s)
    int trainTime;      //训练时间 单位 分钟(min)
    QString gameName;   //游戏名称
}ST_PassiveTrainParam;

//主动训练参数
typedef struct
{
    E_TrainEvaluationType trackType;   //轨迹类型 E_TrainEvaluationType
    int minAngle;       //最小角度 (非圆周运动【负值】)
    int maxAngle;       //最大角度 (非圆周运动【正值】)
    int trainTime;      //训练时间 单位 分钟(min)
    QString gameName;   //游戏名称
}ST_ActiveTrainParam;

#pragma pack(pop)//恢复对齐状态

//更改游戏配置文件的参数(通信文件)
typedef  struct ST_GameParam
{
    int hardLevel;          //难度等级 1-3(此处的难度等级对应范围大小)
    int gameTime;           //游戏运行时间 单位/s
    int speed;              //运行速度 1-5
    int trainingMode;       //训练类型
    QString trainTrackFilePath;  //路径文件
    bool readPosTable;      //是否去读轨迹
    int angleValue;         //分段运动角度
    int trainType;          //被动训练类型1-圆周 2-分段
    int waitTime;           //等待时间
    int maxCircle;          //最大圈数
    ST_GameParam()
    {
        hardLevel = 1;
        gameTime = 0;
        speed = 3;
        trainingMode = 0;
        trainTrackFilePath = "";
        readPosTable = false;
    }
}ST_GameParam;

//该结构体用于读取游戏列表
typedef struct
{
    int gameID;                 //游戏ID
    QString gameName;           //游戏名称
    QString gamePath;           //游戏路径
    QString iconPath;           //游戏图标路径
    QStringList suitTypeList;   //适合的游戏类型  0-被动圆周 1-被动分段 2-主动
}ST_GameMsg;


extern void  Pressure_CheckCRC(uint8_t*buf,int len,uint8_t* CRC_H,uint8_t* CRC_L);
extern void Sleep(int msec);
#endif // DATAFORMATE_H
