#!/bin/sh

if [ -n "$DESTDIR" ] ; then
    case $DESTDIR in
        /*) # ok
            ;;
        *)
            /bin/echo "DESTDIR argument must be absolute... "
            /bin/echo "otherwise python's distutils will bork things."
            exit 1
    esac
fi

echo_and_run() { echo "+ $@" ; "$@" ; }

echo_and_run cd "/home/nitlab/catkin_ws/camera/openvslam/ros/src/cv_bridge"

# ensure that Python install destination exists
echo_and_run mkdir -p "$DESTDIR/home/nitlab/catkin_ws/camera/openvslam/ros/install/lib/python2.7/dist-packages"

# Note that PYTHONPATH is pulled from the environment to support installing
# into one location when some dependencies were installed in another
# location, #123.
echo_and_run /usr/bin/env \
    PYTHONPATH="/home/nitlab/catkin_ws/camera/openvslam/ros/install/lib/python2.7/dist-packages:/home/nitlab/catkin_ws/camera/openvslam/ros/build/lib/python2.7/dist-packages:$PYTHONPATH" \
    CATKIN_BINARY_DIR="/home/nitlab/catkin_ws/camera/openvslam/ros/build" \
    "/usr/bin/python2" \
    "/home/nitlab/catkin_ws/camera/openvslam/ros/src/cv_bridge/setup.py" \
     \
    build --build-base "/home/nitlab/catkin_ws/camera/openvslam/ros/build/cv_bridge" \
    install \
    --root="${DESTDIR-/}" \
    --install-layout=deb --prefix="/home/nitlab/catkin_ws/camera/openvslam/ros/install" --install-scripts="/home/nitlab/catkin_ws/camera/openvslam/ros/install/bin"
