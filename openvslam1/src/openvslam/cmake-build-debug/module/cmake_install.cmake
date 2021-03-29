# Install script for directory: /home/nitlab/catkin_ws/camera/openvslam/src/openvslam/module

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
   "/openvslam/module/frame_tracker.h;/openvslam/module/initializer.h;/openvslam/module/keyframe_inserter.h;/openvslam/module/local_map_cleaner.h;/openvslam/module/local_map_updater.h;/openvslam/module/loop_bundle_adjuster.h;/openvslam/module/loop_detector.h;/openvslam/module/relocalizer.h;/openvslam/module/two_view_triangulator.h;/openvslam/module/type.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/openvslam/module" TYPE FILE FILES
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/module/frame_tracker.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/module/initializer.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/module/keyframe_inserter.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/module/local_map_cleaner.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/module/local_map_updater.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/module/loop_bundle_adjuster.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/module/loop_detector.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/module/relocalizer.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/module/two_view_triangulator.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/module/type.h"
    )
endif()

