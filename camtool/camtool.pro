

INCLUDEPATH += ../libcamio
LIBS += -L../libcamio -lcamio

CONFIG += qt
FORMS   += camtool.ui
HEADERS += CamtoolMain.h
SOURCES += main.cpp CamtoolMain.cpp grab_camera.cpp
