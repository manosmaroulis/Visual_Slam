# generated from catkin/cmake/template/pkg.context.pc.in
CATKIN_PACKAGE_PREFIX = ""
PROJECT_PKG_CONFIG_INCLUDE_DIRS = "${prefix}/include;/usr/local/include".split(';') if "${prefix}/include;/usr/local/include" != "" else []
PROJECT_CATKIN_DEPENDS = "octomap_msgs;sensor_msgs;tf".replace(';', ' ')
PKG_CONFIG_LIBRARIES_WITH_PREFIX = "-loctomap_ros;/usr/local/lib/liboctomap.so;/usr/local/lib/liboctomath.so".split(';') if "-loctomap_ros;/usr/local/lib/liboctomap.so;/usr/local/lib/liboctomath.so" != "" else []
PROJECT_NAME = "octomap_ros"
PROJECT_SPACE_DIR = "/home/nitlab/catkin_ws/camera/openvslam/ros/install"
PROJECT_VERSION = "0.4.1"
