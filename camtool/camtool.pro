

CONFIG += qt

ICON = Nikon_D40.icns

# On the mac, we use the ImageCapture framework. For linking purposes,
# qmake does the righ thing but we need to add the -F flag to CXX flags
# so that header include paths are set up properly.
QMAKE_CXXFLAGS += -F/System/Library/Frameworks/Carbon.framework/Frameworks

# User interface files
FORMS   += camtool.ui
HEADERS += CamtoolMain.h
SOURCES += main.cpp CamtoolMain.cpp grab_camera.cpp camera_images.cpp

# The CameraControl base class
HEADERS += CameraControl.h
SOURCES += CameraControl.cpp

# The Mac ICA implementation of CameraControl
HEADERS += MacICACameraControl.h
SOURCES += MacICACameraControl.cpp MacICA_inventory.cpp MacICA_debug.cpp MacPTPCameraControl.cpp
