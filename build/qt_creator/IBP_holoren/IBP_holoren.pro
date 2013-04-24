################################################################################
#                                                                              #
# Copyright (C) 2012-2013 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>           #
#                                                                              #
#                                                                              #
################################################################################

TEMPLATE = app      # project type
TARGET = holoren    # the application name
DESTDIR = bin       # the destination directory
MOC_DIR = obj       # meta object compoliler directory
RCC_DIR = obj       # a directory for compiled resource files
OBJECTS_DIR = obj   # a directory for temporary compile output
QT -= core gui      # exclude qt libraries
CONFIG -= qt        # no Qt
CONFIG += console   # this isn't graphical application (no graphic window)
CONFIG += link_pkgconfig
PKGCONFIG += sdl
#CONFIG += qt debug

# tweak compilation flags for gcc.
# When you are using a different compiler on unix platform
# just comment this setting out
unix:QMAKE_CXXFLAGS += \
  -std=c++11 \
  -pedantic \
  -Wextra \
  -DHOLOREN_DEBUG_PERF

unix:QMAKE_CXXFLAGS_DEBUG += \
  -DHOLOREN_DEBUG \
  -DHOLOREN_DEBUG_KERNEL

#
DEPENDPATH += .

# additional include directories
win32:INCLUDEPATH += \
  ../../../src  \
  $(MY_LIB_PATH)/libxml2-2.7.8.win32/include \
  $(MY_LIB_PATH)/iconv-1.9.2.win32/include \
  $(MY_LIB_PATH)/zlib-1.2.5/include \
  $(MY_LIB_PATH)/SDL_image-1.2.12/include \
  $(MY_LIB_PATH)/SDL-1.2.15/include \
  $(AMDAPPSDKROOT)/include

unix:INCLUDEPATH += \
  ../../../src  \
  /usr/include/libxml2 \
  $(AMDAPPSDKROOT)/include
  
# additional library directories
win32:LIBS = \
  -L$(MY_LIB_PATH)/libxml2-2.7.8.win32/lib \
  -L$(MY_LIB_PATH)/iconv-1.9.2.win32/lib \
  -L$(MY_LIB_PATH)/zlib-1.2.5/lib \
  -L$(MY_LIB_PATH)/SDL_image-1.2.12/lib/x86 \
  -L$(MY_LIB_PATH)/SDL-1.2.15/lib/x86 \
  -L$(AMDAPPSDKROOT)/lib/x86

# Additional libraries
win32:LIBS += \
  -lSDL \
  -lSDL_image \
  -llibxml2 \
  -lOpenCL
  
unix:LIBS += \
  -lSDL \
  -lSDL_image \
  -lxml2 \
  -lOpenCL

#-L$$OUT_PWD/libs/glew/lib -lGLEW -lGLU



# Header files
HEADERS += \
  ../../../src/config.h \
  ../../../src/global.h \
  ../../../src/Utils.h \
  ../../../src/Debug.h \
  ../../../src/CTimer.h \
  ../../../src/CSharedDataPtr.h \
  ../../../src/Geometry.h \
  ../../../src/COpticalField.h \
  ../../../src/CPointCloud.h \
  ../../../src/CBasePointCloudRW.h \
  ../../../src/CXMLPointCloudRW.h \
  ../../../src/CImagePointCloudRW.h \
  ../../../src/CPCPointCloudRW.h \
  ../../../src/CPointCloudRWFactory.h \
  ../../../src/CBaseRenderer.h \
  ../../../src/CSimpleRenderer.h \
  ../../../src/ocl.h \
  ../../../src/COpenCLRenderer.h

# Source files
SOURCES += \
  ../../../src/Utils.cpp \
  ../../../src/Debug.cpp \
  ../../../src/CTimer.cpp \
  ../../../src/COpticalField.cpp \
  ../../../src/CPointCloud.cpp \
  ../../../src/CBasePointCloudRW.cpp \
  ../../../src/CXMLPointCloudRW.cpp \
  ../../../src/CImagePointCloudRW.cpp \
  ../../../src/CPCPointCloudRW.cpp \
  ../../../src/CPointCloudRWFactory.cpp \
  ../../../src/CSimpleRenderer.cpp \
  ../../../src/ocl.cpp \
  ../../../src/COpenCLRenderer.cpp \
  ../../../src/main.cpp
