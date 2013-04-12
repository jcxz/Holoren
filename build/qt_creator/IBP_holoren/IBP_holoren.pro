################################################################################
#                                                                              #
# Copyright (C) 2012-2013 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>           #
#                                                                              #
# This file is part of Pexeso3D.                                               #
#                                                                              #
# Pexeso3D is free software: you can redistribute it and/or modify             #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation, either version 3 of the License, or            #
# (at your option) any later version.                                          #
#                                                                              #
# Pexeso3D is distributed in the hope that it will be useful,                  #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with Pexeso3D. If not, see <http://www.gnu.org/licenses/>.             #
#                                                                              #
################################################################################

TEMPLATE = app    # project type
TARGET = holoren  # the application name
DESTDIR = bin     # the destination directory
MOC_DIR = obj     # meta object compoliler directory
RCC_DIR = obj     # a directory for compiled resource files
OBJECTS_DIR = obj # a directory for temporary compile output
QT -= core gui    # exclude qt libraries
#CONFIG += qt debug

# tweak compilation flags for gcc.
# When you are using a different compiler on unix platform
# just comment this setting out
unix:QMAKE_CXXFLAGS += \
  -std=c++11 \
  -pedantic \
  -Wextra

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
  
#unix: LIBS += -L$$OUT_PWD/libs/glew/lib -lGLEW -lGLU


# Header files
HEADERS += \
  ../../../src/CBasePointCloudRW.h \
  ../../../src/CImagePointCloudRW.h \
  ../../../src/config.h \
  ../../../src/COpenCLRenderer.h \
  ../../../src/COpticalField.h \
  ../../../src/CPCPointCloudRW.h \
  ../../../src/CPointCloud.h \
  ../../../src/CPointCloudRWFactory.h \
  ../../../src/CSharedDataPtr.h \
  ../../../src/CSimpleRenderer.h \
  ../../../src/CXMLPointCloudRW.h \
  ../../../src/Geometry.h \
  ../../../src/global.h \
  ../../../src/IRenderer.h \
  ../../../src/Utils.h \
  ../../../src/ocl.h

# Source files
SOURCES += \
  ../../../src/CBasePointCloudRW.cpp \
  ../../../src/CImagePointCloudRW.cpp \
  ../../../src/COpenCLRenderer.cpp \
  ../../../src/COpticalField.cpp \
  ../../../src/CPCPointCloudRW.cpp \
  ../../../src/CPointCloud.cpp \
  ../../../src/CPointCloudRWFactory.cpp \
  ../../../src/CSimpleRenderer.cpp \
  ../../../src/CXMLPointCloudRW.cpp \
  ../../../src/main.cpp \
  ../../../src/Utils.cpp \
  ../../../src/ocl.cpp
