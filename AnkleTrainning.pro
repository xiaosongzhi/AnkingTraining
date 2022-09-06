QT       += core gui serialport network xml sql widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
UI_DIR = ./UI
CONFIG += c++11
#DEFINES += UTF8
#Qt Creater based on Qt5.12.10(Mingw32)
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD/Src/log4j/include \
               $$PWD/Src/log4j/src \
               $$PWD/Src/log4j/src/log4qt


INCLUDEPATH += $$PWD/Src/MainWindow \
               $$PWD/Src/communication \
               $$PWD/Src/DataFormate \
               $$PWD/Src/ActiveTrain \
               $$PWD/Src/PassiveTrain \
               $$PWD/Src/DataFormate \
               $$PWD/Src/GameControl \
               $$PWD/Src/DataBaseControl \
               $$PWD/Src/UserManager \
               $$PWD/Src/SharedModule \
               $$PWD/Src/Evaluate \
               $$PWD/Src/Record \
               $$PWD/Src/Setting \
               $$PWD/Src/Report \

SOURCES += \
    Src/ActiveTrain/activetrainning.cpp \
    Src/DataBaseControl/cdatabaseinterface.cpp \
    Src/DataBaseControl/mysqldatabase.cpp \
    Src/DataFormate/dataFormate.cpp \
    Src/DataFormate/userdataformate.cpp \
    Src/Evaluate/evaluatemanager.cpp \
    Src/Evaluate/evaluatepage.cpp \
    Src/Evaluate/evaluatetipdialog.cpp \
    Src/GameControl/gamecontrol.cpp \
    Src/MainWindow/Login.cpp \
    Src/MainWindow/mainwindowpagecontrol.cpp \
    Src/MainWindow/resetwidget.cpp \
    Src/PassiveTrain/passivetraining.cpp \
    Src/Record/newrecordtable.cpp \
    Src/Report/activetrainingreport.cpp \
    Src/Setting/Setting.cpp \
    Src/SharedModule/cbasedialog.cpp \
    Src/SharedModule/circletipswidget.cpp \
    Src/SharedModule/cmaskdialog.cpp \
    Src/SharedModule/controlmaskdialog.cpp \
    Src/SharedModule/cpassworddialog.cpp \
    Src/SharedModule/picturetipswidget.cpp \
    Src/SharedModule/switchactivepassivewidget.cpp \
    Src/SharedModule/waitingdialog.cpp \
    Src/UserManager/currentuserdata.cpp \
    Src/UserManager/edituser.cpp \
    Src/UserManager/usermanager.cpp \
    Src/communication/ccommunicateapi.cpp \
    Src/communication/ccommunicationinterface.cpp \
    Src/communication/cserialportinterface.cpp \
    Src/communication/ctcpsocketinterface.cpp \
    Src/communication/cudpinterface.cpp \
    Src/communication/globalqueue.cpp \
    Src/communication/readconfig.cpp \
    Src/MainWindow/main.cpp \
    Src/MainWindow/mainwindow.cpp

HEADERS += \
    Src/ActiveTrain/activetrainning.h \
    Src/DataBaseControl/cdatabaseinterface.h \
    Src/DataBaseControl/mysqldatabase.h \
    Src/DataFormate/dataFormate.h \
    Src/DataFormate/userdataformate.h \
    Src/Evaluate/evaluatemanager.h \
    Src/Evaluate/evaluatepage.h \
    Src/Evaluate/evaluatetipdialog.h \
    Src/GameControl/gamecontrol.h \
    Src/MainWindow/login.h \
    Src/MainWindow/mainwindowpagecontrol.h \
    Src/MainWindow/resetwidget.h \
    Src/PassiveTrain/passivetraining.h \
    Src/Record/newrecordtable.h \
    Src/Report/activetrainingreport.h \
    Src/Setting/Setting.h \
    Src/SharedModule/cbasedialog.h \
    Src/SharedModule/circletipswidget.h \
    Src/SharedModule/cmaskdialog.h \
    Src/SharedModule/controlmaskdialog.h \
    Src/SharedModule/cpassworddialog.h \
    Src/SharedModule/picturetipswidget.h \
    Src/SharedModule/switchactivepassivewidget.h \
    Src/SharedModule/waitingdialog.h \
    Src/UserManager/currentuserdata.h \
    Src/UserManager/edituser.h \
    Src/UserManager/usermanager.h \
    Src/communication/ccommunicateapi.h \
    Src/communication/ccommunicationinterface.h \
    Src/communication/cserialportinterface.h \
    Src/communication/ctcpsocketinterface.h \
    Src/communication/cudpinterface.h \
    Src/communication/globalqueue.h \
    Src/communication/readconfig.h \
    Src/MainWindow/mainwindow.h

FORMS += \
    Src/ActiveTrain/activetrainning.ui \
    Src/Evaluate/evaluatemanager.ui \
    Src/Evaluate/evaluatepage.ui \
    Src/Evaluate/evaluatetipdialog.ui \
    Src/MainWindow/Login.ui \
    Src/MainWindow/mainwindow.ui \
    Src/MainWindow/resetwidget.ui \
    Src/PassiveTrain/passivetraining.ui \
    Src/Record/newrecordtable.ui \
    Src/Report/activetrainingreport.ui \
    Src/Setting/Setting.ui \
    Src/SharedModule/cbasedialog.ui \
    Src/SharedModule/circletipswidget.ui \
    Src/SharedModule/cpassworddialog.ui \
    Src/SharedModule/picturetipswidget.ui \
    Src/SharedModule/switchactivepassivewidget.ui \
    Src/SharedModule/waitingdialog.ui \
    Src/UserManager/edituser.ui \
    Src/UserManager/usermanager.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
LIBS += -L$$PWD/Src/log4j/bin -llog4qt

#vlc相关库
VLC_INSTALL_PATH = $$PWD/vlc/
#CONFIG(debug,debug|release){
#    LIBS += -L$$VLC_INSTALL_PATH/lib -llibVLCQtCored -llibVLCQtWidgetsd
#}else{
#    LIBS += -L$$VLC_INSTALL_PATH/lib -llibVLCQtCore -llibVLCQtWidgets
#}
INCLUDEPATH += $$VLC_INSTALL_PATH/include
#使用中文的编码配置
QMAKE_CXXFLAGS += /execution-charset:utf-8   /source-charset:utf-8

TRANSLATIONS += AnkleTrainning_EN.ts \
                AnkleTrainning_CN.ts

RESOURCES += \
    images.qrc \
    images.qrc

DISTFILES += \
    images/activity.png \
    images/passive.png \
    images/record.png \
    images/set.png \
    images/shutdown.png \
    images/trainselect/neifanwaifan.png \
    images/trainselect/neishouwaizhan.png \
    images/trainselect/xiezhiqubeiqu.png \
    images/trainselect/yuanzhou.png \
    images/trainselect/跖屈背屈.png


RC_ICONS = logo_ICO.ico
