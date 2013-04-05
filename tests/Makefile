CXXFLAGS=-std=c++11 -Wall -Wextra -pedantic -g -DDEBUG
CXX=g++
SRC=test_pcrw_factory.cpp \
    ../src/config.h \
    ../src/global.h \
    ../src/Utils.h \
    ../src/Utils.cpp \
    ../src/CPointCloud.h \
    ../src/CPointCloud.cpp \
    ../src/CBasePointCloudRW.h \
    ../src/CBasePointCloudRW.cpp \
    ../src/CXMLPointCloudRW.h \
    ../src/CXMLPointCloudRW.cpp \
    ../src/CImagePointCloudRW.h \
    ../src/CImagePointCloudRW.cpp \
    ../src/CPCPointCloudRW.h \
    ../src/CPCPointCloudRW.cpp \
    ../src/CPointCloudRWFactory.h \
    ../src/CPointCloudRWFactory.cpp
	

test_pcrw_factory: $(SRC)
	$(CXX) $(CXXFLAGS) -I/usr/include/libxml2 -I../src $(SRC) -o $@ -lxml2 -lSDL -lSDL_image
