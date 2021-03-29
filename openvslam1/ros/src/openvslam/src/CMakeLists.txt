if(USE_PANGOLIN_VIEWER)
    message(STATUS "Viewer for examples: pangolin viewer")
elseif(USE_SOCKET_PUBLISHER)
    message(STATUS "Viewer for examples: socket publisher")
else()
    message(STATUS "Viewer for examples: none")
endif()
#find_package(realsense2 REQUIRED)
#find_package(realsense2 2.42.0)
#include_directories(${realsense2_INCLUDE_DIR})
set(EXECUTABLE_TARGETS "")

add_executable(run_rs_rgbd_slam run_rs_rgbd_slam.cc)
list(APPEND EXECUTABLE_TARGETS run_rs_rgbd_slam)



add_executable(run_slam run_slam.cc)
list(APPEND EXECUTABLE_TARGETS run_slam)



add_executable(run_localization run_localization.cc)
list(APPEND EXECUTABLE_TARGETS run_localization)


add_executable(manos_slam manos_slam.cc)
#target_link_libraries(manos_slam ${OpenCV_LIBS})

list(APPEND EXECUTABLE_TARGETS manos_slam)

foreach(EXECUTABLE_TARGET IN LISTS EXECUTABLE_TARGETS)
    add_dependencies(${EXECUTABLE_TARGET} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

    # pangolin_viewer is used on a priority basis
    if(USE_PANGOLIN_VIEWER)
        # set macro flag
        target_compile_definitions(${EXECUTABLE_TARGET} 
                PRIVATE -DUSE_PANGOLIN_VIEWER)
        # link viewer
        target_link_libraries(${EXECUTABLE_TARGET}  PRIVATE
                ${Pangolin_Viewer_LIB} ${Pangolin_LIBRARIES}  opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs -lrealsense2)
	# ${realsense2_LIBRARY}
	#target_include_directories(${PROJECT_NAME}
 	 #PRIVATE ${realsense2_INCLUDE_DIR})

    elseif(USE_SOCKET_PUBLISHER)
        # set macro flag
        target_compile_definitions(${EXECUTABLE_TARGET}
                PRIVATE -DUSE_SOCKET_PUBLISHER)
        # link viewer
        target_link_libraries(${EXECUTABLE_TARGET} PRIVATE
                ${Socket_Publisher_LIB} ${SIOCLIENT_LIBRARY} ${PROTOBUF_LIBRARIES})
    endif()

    # setup stack trace logger
    if(USE_STACK_TRACE_LOGGER)
        target_compile_definitions(${EXECUTABLE_TARGET}
                PRIVATE -DUSE_STACK_TRACE_LOGGER)
        target_link_libraries(${EXECUTABLE_TARGET} PRIVATE
                ${GLOG_LIBRARIES})
    endif()

    # setup google-perftools
    if(USE_GOOGLE_PERFTOOLS)
        target_compile_definitions(${EXECUTABLE_TARGET}
                PRIVATE -DUSE_GOOGLE_PERFTOOLS)
        target_link_libraries(${EXECUTABLE_TARGET} PRIVATE
                ${GPERFTOOLS_LIBRARIES})
    endif()

endforeach()
