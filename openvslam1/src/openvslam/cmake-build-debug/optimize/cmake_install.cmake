# Install script for directory: /home/nitlab/catkin_ws/camera/openvslam/src/openvslam/optimize

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
   "/openvslam/optimize/global_bundle_adjuster.h;/openvslam/optimize/graph_optimizer.h;/openvslam/optimize/local_bundle_adjuster.h;/openvslam/optimize/pose_optimizer.h;/openvslam/optimize/transform_optimizer.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/openvslam/optimize" TYPE FILE FILES
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/optimize/global_bundle_adjuster.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/optimize/graph_optimizer.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/optimize/local_bundle_adjuster.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/optimize/pose_optimizer.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/optimize/transform_optimizer.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/optimize/g2o/cmake_install.cmake")

endif()

