# generated from catkin/cmake/template/pkg.context.pc.in
CATKIN_PACKAGE_PREFIX = ""
PROJECT_PKG_CONFIG_INCLUDE_DIRS = "${prefix}/include;/usr/local/include".split(';') if "${prefix}/include;/usr/local/include" != "" else []
PROJECT_CATKIN_DEPENDS = "roscpp;visualization_msgs;sensor_msgs;pcl_ros;pcl_conversions;nav_msgs;std_msgs;std_srvs;octomap_ros;octomap_msgs;dynamic_reconfigure;nodelet".replace(';', ' ')
PKG_CONFIG_LIBRARIES_WITH_PREFIX = "-loctomap_server;/usr/local/lib/liboctomap.so;/usr/local/lib/liboctomath.so".split(';') if "-loctomap_server;/usr/local/lib/liboctomap.so;/usr/local/lib/liboctomath.so" != "" else []
PROJECT_NAME = "octomap_server"
PROJECT_SPACE_DIR = "/home/nitlab/catkin_ws/camera/openvslam/ros/install"
PROJECT_VERSION = "0.6.6"
