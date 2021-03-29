# Install script for directory: /home/nitlab/catkin_ws/camera/openvslam/src/openvslam

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopenvslam.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopenvslam.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopenvslam.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/lib/libopenvslam.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopenvslam.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopenvslam.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopenvslam.so"
         OLD_RPATH "/usr/local/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopenvslam.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/openvslam/config.h;/openvslam/global_optimization_module.h;/openvslam/mapping_module.h;/openvslam/system.h;/openvslam/tracking_module.h;/openvslam/type.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/openvslam" TYPE FILE FILES
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/config.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/global_optimization_module.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/mapping_module.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/system.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/tracking_module.h"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/type.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/openvslam/3rd/json/include/nlohmann/json.hpp;/openvslam/3rd/json/include/nlohmann/json_fwd.hpp")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/openvslam/3rd/json/include/nlohmann" TYPE FILE FILES
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/3rd/json/include/nlohmann/json.hpp"
    "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/3rd/json/include/nlohmann/json_fwd.hpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/openvslam/3rd/spdlog/include")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/openvslam/3rd/spdlog" TYPE DIRECTORY FILES "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/3rd/spdlog/include")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/camera/cmake_install.cmake")
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/data/cmake_install.cmake")
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/feature/cmake_install.cmake")
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/initialize/cmake_install.cmake")
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/io/cmake_install.cmake")
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/match/cmake_install.cmake")
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/module/cmake_install.cmake")
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/optimize/cmake_install.cmake")
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/publish/cmake_install.cmake")
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/solve/cmake_install.cmake")
  include("/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/util/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/nitlab/catkin_ws/camera/openvslam/src/openvslam/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
