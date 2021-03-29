# Install script for directory: /home/nitlab/catkin_ws/camera/openvslam/src/openvslam/data

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/openvslam/data/bow_database.h;/openvslam/data/bow_vocabulary.h;/openvslam/data/camera_database.h;/openvslam/data/common.h;/openvslam/data/frame.h;/openvslam/data/frame_statistics.h;/openvslam/data/graph_node.h;/openvslam/data/keyframe.h;/openvslam/data/landmark.h;/openvslam/data/map_database.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/openvslam/data" TYPE FILE FILES
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/data/bow_database.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/data/bow_vocabulary.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/data/camera_database.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/data/common.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/data/frame.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/data/frame_statistics.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/data/graph_node.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/data/keyframe.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/data/landmark.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/data/map_database.h"
    )
endif()

