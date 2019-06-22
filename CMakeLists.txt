cmake_minimum_required(VERSION 3.2)
PROJECT(fall_recognition)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

find_package(Caffe)

include_directories(${Caffe_INCLUDE_DIRS})
add_definitions(${Caffe_DEFINITIONS})

include_directories(
    include
)

aux_source_directory(
    src DIR_LIB_SRCS
)

add_library(${PROJECT_NAME}
    ${DIR_LIB_SRCS}
)

target_link_libraries(${PROJECT_NAME} ${Caffe_LIBRARIES})

add_executable(demo demo/demo.cpp)
target_link_libraries(demo
    ${PROJECT_NAME}
)

add_executable(usb_cam_demo demo/usb_cam_demo.cpp)
target_link_libraries(usb_cam_demo
    ${PROJECT_NAME}
)

add_executable(skip_frame_test demo/skip_frame_test.cpp)
target_link_libraries(skip_frame_test
    ${PROJECT_NAME}
)

add_executable(no_bn_demo demo/no_bn_demo.cpp)
target_link_libraries(no_bn_demo
    ${PROJECT_NAME}
)