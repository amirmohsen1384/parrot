QT += gui
QT += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++2a

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
