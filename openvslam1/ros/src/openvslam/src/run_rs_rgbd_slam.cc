#ifdef USE_PANGOLIN_VIEWER
#include "pangolin_viewer/viewer.h"
#elif USE_SOCKET_PUBLISHER
#include "socket_publisher/publisher.h"
#endif

// My libraries -------------------------
#include <ros/ros.h>
 #include <geometry_msgs/PoseStamped.h>
 #include <nav_msgs/Odometry.h>
 #include <tf2_geometry_msgs/tf2_geometry_msgs.h>
 #include <tf2_ros/transform_broadcaster.h>
 #include <tf2_ros/transform_listener.h>
 #include <cv_bridge/cv_bridge.h>


 #include "openvslam/data/landmark.h"
 #include "openvslam/data/keyframe.h"
 #include <pcl_conversions/pcl_conversions.h>
 #include <sensor_msgs/PointCloud2.h>
 #include <pcl_ros/point_cloud.h>
 #include <pcl/point_types.h>
// My libraries end --------------------




#include "openvslam/system.h"
#include "openvslam/config.h"
#include "openvslam/util/stereo_rectifier.h"

#include <iostream>
#include <chrono>
#include <numeric>

// #include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include "my_util/cv-helpers.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <spdlog/spdlog.h>
#include <popl.hpp>

#ifdef USE_STACK_TRACE_LOGGER
#include <glog/logging.h>
#endif

#ifdef USE_GOOGLE_PERFTOOLS
#include <gperftools/profiler.h>
#endif
typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;



 void pose_odometry_pub(auto cam_pose_, auto pose_pub_, auto odometry_pub_){
     Eigen::Matrix3d rotation_matrix = cam_pose_.block(0, 0, 3, 3);
     Eigen::Vector3d translation_vector = cam_pose_.block(0, 3, 3, 1);

     tf2::Matrix3x3 tf_rotation_matrix(rotation_matrix(0, 0), rotation_matrix(0, 1), rotation_matrix(0, 2),
                                       rotation_matrix(1, 0), rotation_matrix(1, 1), rotation_matrix(1, 2),
                                       rotation_matrix(2, 0), rotation_matrix(2, 1), rotation_matrix(2, 2));
    
     tf2::Vector3 tf_translation_vector(translation_vector(0), translation_vector(1), translation_vector(2));

     tf_rotation_matrix = tf_rotation_matrix.inverse();
     tf_translation_vector = -(tf_rotation_matrix * tf_translation_vector);

     tf2::Transform transform_tf(tf_rotation_matrix, tf_translation_vector);

     tf2::Matrix3x3 rot_open_to_ros (0, 0, 1,
                                   -1, 0, 0,
                                    0,-1, 0);

     tf2::Transform transformA(rot_open_to_ros, tf2::Vector3(0.0, 0.0, 0.0));
     tf2::Transform transformB(rot_open_to_ros.inverse(), tf2::Vector3(0.0, 0.0, 0.0));

     transform_tf = transformA * transform_tf * transformB;

     ros::Time now = ros::Time::now();

     nav_msgs::Odometry odom_msg_;
     odom_msg_.header.stamp = now;
     odom_msg_.header.frame_id = "map";
     odom_msg_.child_frame_id = "base_link";
     odom_msg_.pose.pose.orientation.x = transform_tf.getRotation().getX();
     odom_msg_.pose.pose.orientation.y = transform_tf.getRotation().getY();
     odom_msg_.pose.pose.orientation.z = transform_tf.getRotation().getZ();
     odom_msg_.pose.pose.orientation.w = transform_tf.getRotation().getW();
     odom_msg_.pose.pose.position.x = transform_tf.getOrigin().getX();
     odom_msg_.pose.pose.position.y = transform_tf.getOrigin().getY();
     odom_msg_.pose.pose.position.z = transform_tf.getOrigin().getZ();
     odometry_pub_.publish(odom_msg_);

     // Create pose message and update it with current camera pose
     geometry_msgs::PoseStamped camera_pose_msg_;
     camera_pose_msg_.header.stamp = now;
     camera_pose_msg_.header.frame_id = "map";
     camera_pose_msg_.pose.position.x = transform_tf.getOrigin().getX();
     camera_pose_msg_.pose.position.y = transform_tf.getOrigin().getY();
     camera_pose_msg_.pose.position.z = transform_tf.getOrigin().getZ();
     camera_pose_msg_.pose.orientation.x = transform_tf.getRotation().getX();
     camera_pose_msg_.pose.orientation.y = transform_tf.getRotation().getY();
     camera_pose_msg_.pose.orientation.z = transform_tf.getRotation().getZ();
     camera_pose_msg_.pose.orientation.w = transform_tf.getRotation().getW();
     pose_pub_.publish(camera_pose_msg_);

     // transform broadcast
     static tf2_ros::TransformBroadcaster tf_br;
   
     geometry_msgs::TransformStamped transformStamped;

     transformStamped.header.stamp = ros::Time::now();
     transformStamped.header.frame_id = "map";
     transformStamped.child_frame_id = "base_link";
     transformStamped.transform.translation.x = transform_tf.getOrigin().getX();
     transformStamped.transform.translation.y = transform_tf.getOrigin().getY();
     transformStamped.transform.translation.z = transform_tf.getOrigin().getZ();
     transformStamped.transform.rotation.x = transform_tf.getRotation().getX();
     transformStamped.transform.rotation.y = transform_tf.getRotation().getY();
     transformStamped.transform.rotation.z = transform_tf.getRotation().getZ();
     transformStamped.transform.rotation.w = transform_tf.getRotation().getW();

     tf_br.sendTransform(transformStamped);
 }



 void cloud_point_pub(auto lms,auto cloud_pub){

     if (!lms.empty()){
         PointCloud::Ptr cloud (new PointCloud);
         cloud->header.frame_id = "map";
         cloud->header.stamp = ros::Time::now().toNSec() / 1000;

         //  cloud->width = lms.size();
         // cloud->height = 1;
         // cloud->is_dense = true;
         Eigen::Matrix<double, 3, 1> c;
         // std::vector<int> positions;

         // int counter = 0;

         for( size_t i=0 ; i<lms.size();i++){

             c =lms[i]->get_pos_in_world();
             // std::cout<<"min-> "<<lms[i]->get_min_valid_distance()<< " max-> "<<lms[i]->get_max_valid_distance()<<std::endl;
//             if (c[2]>=0.05 && c[2]<=0.6 && lms[i]->num_observations()>2 && lms[i]->get_min_valid_distance()<1.5)
                 cloud->points.push_back(pcl::PointXYZ(c[0], c[1], c[2]));
         }

         cloud->width = cloud->points.size();
         cloud->height = 1;
         cloud->is_dense = true;


         cloud_pub.publish(cloud);
         //  ros::Duration(0.1).sleep();
     }

 }

void mono_tracking(rs2::pipeline pipe, const std::shared_ptr<openvslam::config>& cfg,
                   const std::string& vocab_file_path, const std::string& map_db_path) {

    // build a SLAM system
    openvslam::system SLAM(cfg, vocab_file_path);
    // startup the SLAM process
    SLAM.startup();

    // create a viewer object
    // and pass the frame_publisher and the map_publisher
#ifdef USE_PANGOLIN_VIEWER
    pangolin_viewer::viewer viewer(cfg, &SLAM, SLAM.get_frame_publisher(), SLAM.get_map_publisher());
#elif USE_SOCKET_PUBLISHER
    socket_publisher::publisher publisher(cfg, &SLAM, SLAM.get_frame_publisher(), SLAM.get_map_publisher());
#endif

    using namespace cv;
    using namespace rs2;

    align align_to(RS2_STREAM_COLOR);

    double timestamp = 0.0;
    std::vector<double> track_times;

    unsigned int num_frame = 0;

    bool is_not_end = true;

    // run the SLAM in another thread
    std::thread thread([&]() {
        while (is_not_end) {
            // check if the termination of SLAM system is requested or not
            if (SLAM.terminate_is_requested()) {
                break;
            }
            frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera

            frameset aligned_set = align_to.process(data);

            auto frame = frame_to_mat(aligned_set.get_color_frame());

            if (frame.empty()) {
                continue;
            }

            const auto tp_1 = std::chrono::steady_clock::now();

            // input the current frame and estimate the camera pose
            SLAM.feed_monocular_frame(frame, timestamp);

            const auto tp_2 = std::chrono::steady_clock::now();

            const auto track_time = std::chrono::duration_cast<std::chrono::duration<double>>(tp_2 - tp_1).count();
            track_times.push_back(track_time);

            timestamp += 1.0 / cfg->camera_->fps_;
            ++num_frame;
        }

        // wait until the loop BA is finished
        while (SLAM.loop_BA_is_running()) {
            std::this_thread::sleep_for(std::chrono::microseconds(5000));
        }
    });

    // run the viewer in the current thread
#ifdef USE_PANGOLIN_VIEWER
    viewer.run();
#elif USE_SOCKET_PUBLISHER
    publisher.run();
#endif

    thread.join();

    // shutdown the SLAM process
    SLAM.shutdown();

    if (!map_db_path.empty()) {
        // output the map database
        SLAM.save_map_database(map_db_path);
    }

    std::sort(track_times.begin(), track_times.end());
    const auto total_track_time = std::accumulate(track_times.begin(), track_times.end(), 0.0);
    std::cout << "median tracking time: " << track_times.at(track_times.size() / 2) << "[s]" << std::endl;
    std::cout << "mean tracking time: " << total_track_time / track_times.size() << "[s]" << std::endl;
}




void rgbd_tracking(rs2::pipeline pipe, const std::shared_ptr<openvslam::config>& cfg,
                     const std::string& vocab_file_path, const std::string& map_db_path) {
    // build a SLAM system
    openvslam::system SLAM(cfg, vocab_file_path);

     ros::NodeHandle nh;
     ros::Publisher camera_pose_publisher = nh.advertise<geometry_msgs::PoseStamped>("/openvslam/camera_pose", 1);
     ros::Publisher odometry_pub_publisher = nh.advertise<nav_msgs::Odometry>("/openvslam/odometry", 1);
     ros::Publisher cloud_pub_publisher = nh.advertise<PointCloud> ("cloud_in", 10);
//     startup the SLAM process
    SLAM.startup();

    // create a viewer object
    // and pass the frame_publisher and the map_publisher
#ifdef USE_PANGOLIN_VIEWER
    pangolin_viewer::viewer viewer(cfg, &SLAM, SLAM.get_frame_publisher(), SLAM.get_map_publisher());
#elif USE_SOCKET_PUBLISHER
    socket_publisher::publisher publisher(cfg, &SLAM, SLAM.get_frame_publisher(), SLAM.get_map_publisher());
#endif

    using namespace cv;
    using namespace rs2;

    align align_to(RS2_STREAM_COLOR);

    double timestamp = 0.0;
    std::vector<double> track_times;
    unsigned int num_frame = 0;

    std::vector<openvslam::data::landmark *> landmarks;
    bool is_not_end = true;
    // run the SLAM in another thread
    std::thread thread([&]() {
        while (is_not_end) {
            // check if the termination of SLAM system is requested or not
            if (SLAM.terminate_is_requested()) {
                break;
            }

            frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
            frameset aligned_set = align_to.process(data);
            
            auto rgb_img = frame_to_mat(aligned_set.get_color_frame());
            auto depth_img = frame_to_mat(aligned_set.get_depth_frame());

            if (rgb_img.empty() || depth_img.empty()) {
                continue;
            }

            const auto tp_1 = std::chrono::steady_clock::now();

            // input the current frame and estimate the camera pose
            auto cam_pose = SLAM.feed_RGBD_frame(rgb_img, depth_img, timestamp);

            const auto tp_2 = std::chrono::steady_clock::now();

            const auto track_time = std::chrono::duration_cast<std::chrono::duration<double>>(tp_2 - tp_1).count();
            track_times.push_back(track_time);

            timestamp += 1.0 / cfg->camera_->fps_;
            ++num_frame;

            pose_odometry_pub(cam_pose, camera_pose_publisher, odometry_pub_publisher);
            
            // landmarks = SLAM.print();
            
            // cloud_point_pub(landmarks,cloud_pub_publisher);

        



        }

        // wait until the loop BA is finished
        while (SLAM.loop_BA_is_running()) {
            std::this_thread::sleep_for(std::chrono::microseconds(5000));
        }
    });

    // run the viewer in the current thread
#ifdef USE_PANGOLIN_VIEWER
    viewer.run();
#elif USE_SOCKET_PUBLISHER
    publisher.run();
#endif

    thread.join();

    // shutdown the SLAM process
    SLAM.shutdown();

    if (!map_db_path.empty()) {
        // output the map database
        SLAM.save_map_database(map_db_path);
    }

    std::sort(track_times.begin(), track_times.end());
    const auto total_track_time = std::accumulate(track_times.begin(), track_times.end(), 0.0);
    std::cout << "median tracking time: " << track_times.at(track_times.size() / 2) << "[s]" << std::endl;
    std::cout << "mean tracking time: " << total_track_time / track_times.size() << "[s]" << std::endl;
}

int main(int argc, char* argv[]) try
{
#ifdef USE_STACK_TRACE_LOGGER
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
#endif
    ros::init(argc,argv,"run_slam");
    // create options
    popl::OptionParser op("Allowed options");
    auto help = op.add<popl::Switch>("h", "help", "produce help message");
    auto vocab_file_path = op.add<popl::Value<std::string>>("v", "vocab", "vocabulary file path");
    auto config_file_path = op.add<popl::Value<std::string>>("c", "config", "config file path");
    auto map_db_path = op.add<popl::Value<std::string>>("p", "map-db", "store a map database at this path after SLAM", "");
    auto debug_mode = op.add<popl::Switch>("", "debug", "debug mode");
    try {
        op.parse(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << std::endl;
        std::cerr << op << std::endl;
        return EXIT_FAILURE;
    }

    // check validness of options
    if (help->is_set()) {
        std::cerr << op << std::endl;
        return EXIT_FAILURE;
    }
    if (!vocab_file_path->is_set() || !config_file_path->is_set()) {
        std::cerr << "invalid arguments" << std::endl;
        std::cerr << std::endl;
        std::cerr << op << std::endl;
        return EXIT_FAILURE;
    }

    // setup logger
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^[%L] %v%$");
    if (debug_mode->is_set()) {
        spdlog::set_level(spdlog::level::debug);
    }
    else {
        spdlog::set_level(spdlog::level::info);
    }

    // load configuration
    std::shared_ptr<openvslam::config> cfg;
    try {
        cfg = std::make_shared<openvslam::config>(config_file_path->value());
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

#ifdef USE_GOOGLE_PERFTOOLS
    ProfilerStart("slam.prof");
#endif

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    pipe.start();

    // run tracking
    if (cfg->camera_->setup_type_ == openvslam::camera::setup_type_t::Monocular) {
        mono_tracking(pipe, cfg, vocab_file_path->value(), map_db_path->value());
    }
    else if (cfg->camera_->setup_type_ == openvslam::camera::setup_type_t::RGBD) {
        rgbd_tracking(pipe, cfg, vocab_file_path->value(), map_db_path->value());
    }
    else {
        throw std::runtime_error("Invalid setup type: " + cfg->camera_->get_setup_type_string());
    }

#ifdef USE_GOOGLE_PERFTOOLS
    ProfilerStop();
#endif

    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}