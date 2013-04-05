#
# This is a cmake project file for my bachelor thesis,
# which concers generating holograms with computers
#
# Author: Matus Fedorko (xfedor01@email.fit.vutbr.cz)
#

cmake_minimum_required(VERSION 2.8)
project(HOLOREN)

# release and debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -Wall -Wextra -pedantic -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -Wall -Wextra -pedantic -O3")

# include a non-standard module from a local directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")

# find pcap libraries nad header files
find_package(LibXml2 REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL REQUIRED)
find_package(OpenCL REQUIRED)

# include directories
include_directories(src ${LIBXML2_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} ${SDL_INCLUDE_DIR} ${OPENCL_INCLUDE_DIR})

# libraries
set(LIBS ${LIBS} ${LIBXML2_LIBRARIES} ${SDL_IMAGE_LIBRARIES} ${SDL_LIBRARY} ${OPENCL_LIBRARY})

# project sources
set(HOLOREN_SOURCES
  src/config.h
  src/global.h
  src/Utils.h
  src/Utils.cpp
  src/CSharedDataPtr.h
  src/Geometry.h
  src/COpticalField.h
  src/COpticalField.cpp
  src/CPointCloud.h
  src/CPointCloud.cpp
  src/CBasePointCloudRW.h
  src/CBasePointCloudRW.cpp
  src/CXMLPointCloudRW.h
  src/CXMLPointCloudRW.cpp
  src/CImagePointCloudRW.h
  src/CImagePointCloudRW.cpp
  src/CPCPointCloudRW.h
  src/CPCPointCloudRW.cpp
  src/CPointCloudRWFactory.h
  src/CPointCloudRWFactory.cpp
  src/IRenderer.h
  src/CSimpleRenderer.h
  src/CSimpleRenderer.cpp
  src/COpenCLRenderer.h
  src/COpenCLRenderer.cpp
  src/main.cpp
)

add_executable(holoren ${HOLOREN_SOURCES})
target_link_Libraries(holoren ${LIBS})


#########################################################
### Tests
#########################################################

# optical field test
set(TEST_OPTICAL_FIELD_SOURCES
  src/config.h
  src/global.h
  src/CSharedDataPtr.h
  src/COpticalField.h
  src/COpticalField.cpp
  tests/test_optical_field.cpp
)

add_executable(test_optical_field ${TEST_OPTICAL_FIELD_SOURCES})

# point cloud sampling test
set(TEST_POINT_CLOUD_SAMPLER_SOURCES
  src/config.h
  src/global.h
  src/Utils.h
  src/Utils.cpp
  src/CPointCloud.h
  src/CPointCloud.cpp
  src/CBasePointCloudRW.h
  src/CBasePointCloudRW.cpp
  src/CXMLPointCloudRW.h
  src/CXMLPointCloudRW.cpp
  src/CImagePointCloudRW.h
  src/CImagePointCloudRW.cpp
  src/CPCPointCloudRW.h
  src/CPCPointCloudRW.cpp
  src/CPointCloudRWFactory.h
  src/CPointCloudRWFactory.cpp
  tests/test_point_cloud_sampler.cpp
)

add_executable(test_point_cloud_sampler ${TEST_POINT_CLOUD_SAMPLER_SOURCES})
target_link_Libraries(test_point_cloud_sampler ${LIBS})

# point cloud sampling test
set(TEST_PCRW_FACTORY_SOURCES
  src/config.h
  src/global.h
  src/Utils.h
  src/Utils.cpp
  src/CPointCloud.h
  src/CPointCloud.cpp
  src/CBasePointCloudRW.h
  src/CBasePointCloudRW.cpp
  src/CXMLPointCloudRW.h
  src/CXMLPointCloudRW.cpp
  src/CImagePointCloudRW.h
  src/CImagePointCloudRW.cpp
  src/CPCPointCloudRW.h
  src/CPCPointCloudRW.cpp
  src/CPointCloudRWFactory.h
  src/CPointCloudRWFactory.cpp
  tests/test_pcrw_factory.cpp
)

add_executable(test_pcrw_factory ${TEST_PCRW_FACTORY_SOURCES})
target_link_Libraries(test_pcrw_factory ${LIBS})
