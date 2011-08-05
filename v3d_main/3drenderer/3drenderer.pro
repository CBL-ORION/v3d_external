# #####################################################################
# Automatically generated by qmake (2.01a) Thu Aug 7 00:14:01 2008
# #####################################################################
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

DEFINES += test_main_cpp

QT_DIR = $$dirname(QMAKE_QMAKE)/.. #c:/qt/4.4.0
win32 {
	WINGW_DIR = c:/mingw
	#LOCAL_DIR = c:/msys/local
	LOCAL_DIR = ../common_lib
	CONFIG += release
	CONFIG += windows
}

# DEFINES += QT_NO_DEBUG_OUTPUT
CONFIG += warn_off  # only work for complier
#macx: QMAKE_LFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden # turn off this type warnings
#macx: QMAKE_CXXFLAGS += -gdwarf-2 # turn off visibility warnings
# need Qt 4.5.0 above and reCreate Makefile, this will be automatic.

QT += opengl
#macx:LIBS += -framework GLUT
#win32:LIBS += -lglut32
#CONFIG += qtestlib

#CONFIG += precompile_header # NOT success
#PRECOMPILED_HEADER += v3dr_common.h

# Input
LIBS += -L../jba/c++ -lv3dnewmat
macx:LIBS += -framework CoreServices #Snow leopard GLee_r.o requires CoreServices framework

HEADERS += GLee_r.h \
    renderer.h \
    renderer_tex2.h \
    Renderer_gl2.h \
    v3dr_mainwindow.h \
    v3dr_glwidget.h \
    V3dr_surfaceDialog.h \
    ItemEditor.h \
    V3dr_colormapDialog.h \
    gradients.h hoverpoints.h \
    barFigureDialog.h
SOURCES += GLee_r.c \
    renderer.cpp \
    renderer_tex2.cpp \
    renderer_obj2.cpp \
    renderer_hit2.cpp \
    renderer_labelfield.cpp \
    Renderer_gl2.cpp \
    test_main.cpp \
    v3dr_mainwindow.cpp \
    v3dr_control_signal.cpp \
    v3dr_glwidget.cpp \
    V3dr_surfaceDialog.cpp \
    ItemEditor.cpp \
    V3dr_colormapDialog.cpp \
    gradients.cpp hoverpoints.cpp \
    barFigureDialog.cpp
RESOURCES += 3drenderer.qrc

CONFIG += $$find(VCONFIG,"debug") $$find(VCONFIG,"release")

win32 {
	BUILD_TIME = $$system(time/t) $$system(date/t)
} else {
	BUILD_TIME = $$system(date)
} 
message(BUILD_TIME=$$BUILD_TIME)

