#ifdef USE_PANGOLIN_VIEWER
#include "pangolin_viewer/viewer.h"
#elif USE_SOCKET_PUBLISHER
#include "socket_publisher/publisher.h"
#endif

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/transform_listener.h>
#include <cv_bridge/cv_bridge.h>

#include "openvslam/system.h"
#include "openvslam/config.h"
#include "openvslam/util/stereo_rectifier.h"

#include <iostream>
#include <csignal>
#include <chrono>
#include <numeric>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <spdlog/spdlog.h>
#include <popl.hpp>
#include "openvslam/data/landmark.h"
#include "openvslam/data/keyframe.h"
#include <pcl_conversions/pcl_conversions.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
// #include <Matrix3x3.h>
// #include<cmath>

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
    odom_msg_.header.frame_id = "odom";
    odom_msg_.child_frame_id = "camera_link";
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
    camera_pose_msg_.header.frame_id = "odom";
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
    transformStamped.header.frame_id = "odom";
    transformStamped.child_frame_id = "camera_link";
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
            if (c[2]>=0.05 && c[2]<=0.6 && lms[i]->num_observations()>2 && lms[i]->get_min_valid_distance()<1.5)
                cloud->points.push_back(pcl::PointXYZ(c[0], c[1], c[2]));
        }

        cloud->width = cloud->points.size();
        cloud->height = 1;
        cloud->is_dense = true;


        cloud_pub.publish(cloud);
        //  ros::Duration(0.1).sleep();
    }



}

std::vector<openvslam::data::landmark *> clear_landmarks(std::vector<openvslam::data::landmark *> lms){
    std::vector<openvslam::data::landmark *> clear_lms;
    // float x,y,z,x_c,y_c,z_c;
    // float euc_dist;

    // auto i = lms.begin();

    // while(i != lms.end()){

    //     (*i)->update_normal_and_depth();
    //     Eigen::Matrix<double, 3, 1> pos= (*i)->get_pos_in_world();

    //     // std::cout<<"x--> "<<pos[0]<<" y--> "<<pos[1]<<" z--> "<<pos[2]<<std::endl;
    //     Eigen::Matrix<double, 3, 1> normal = (*i)->get_obs_mean_normal();
    //     if(pos[2]>0.05){
    //     //    i = lms.erase(i);
    //         clear_lms.push_back((*i));
    //     }
    //     ++i;
  
    // }

    for(auto j=lms.begin();j!=lms.end();j++){
        (*j)->update_normal_and_depth();
        Eigen::Matrix<double, 3, 1> pos= (*j)->get_pos_in_world();
        Eigen::Matrix<double, 3, 1> normal = (*j)->get_obs_mean_normal();
         if(pos[2]<0.05){
            j = lms.erase(j);
            break;
            // clear_lms.push_back(lms[j]);
        }
    }
    return lms;


}

int flag =0;
void signalHandler( int signum) {
   std::cout << "Interrupt signal (" << signum << ") received.\n";
   // cleanup and close up stuff here  
   // terminate program  
    flag=1;
    // return(signum);  
}




void mono_tracking(const std::shared_ptr<openvslam::config>& cfg,
                   const std::string& vocab_file_path, const unsigned int cam_num, const std::string& mask_img_path,
                   const float scale, const std::string& map_db_path) {
    // load the mask image
    const cv::Mat mask = mask_img_path.empty() ? cv::Mat{} : cv::imread(mask_img_path, cv::IMREAD_GRAYSCALE);

    // build a SLAM system
    openvslam::system SLAM(cfg, vocab_file_path);
    // startup the SLAM process
    SLAM.startup();

    // create a viewer object
    // and pass the frame_publisher and the map_publisher
#ifdef USE_PANGOLIN_VIEWER
 //   pangolin_viewer::viewer viewer(cfg, &SLAM, SLAM.get_frame_publisher(), SLAM.get_map_publisher());
#elif USE_SOCKET_PUBLISHER
    socket_publisher::publisher publisher(cfg, &SLAM, SLAM.get_frame_publisher(), SLAM.get_map_publisher());
#endif
    // cv::VideoCapture::set(cv::CAP_PROP_CONVERT_RGB,0);
    auto video = cv::VideoCapture(cam_num);
    if (!video.isOpened()) {
        spdlog::critical("cannot open a camera {}", cam_num);
        SLAM.shutdown();
        return;
    }

    cv::Mat frame;
    double timestamp = 0.0;
    std::vector<double> track_times;
    
    ros::NodeHandle nh;
    ros::Publisher camera_pose_publisher = nh.advertise<geometry_msgs::PoseStamped>("/openvslam/camera_pose", 1);
    ros::Publisher odometry_pub_publisher = nh.advertise<nav_msgs::Odometry>("/odom", 1);
    // ros::Publisher cloud_pub_publisher = nh.advertise<PointCloud> ("cloud_in", 10);
    // ros::Rate loop_rate(30);

    unsigned int num_frame = 0;

    bool is_not_end = true;
    std::vector<openvslam::data::landmark *> landmarks;
    // std::vector<openvslam::data::keyframe *> kf;
    // kf.reserve(sizeof(openvslam::data::keyframe));
    // run the SLAM in another thread
    std::thread thread([&]() {
        
        signal(SIGINT,signalHandler);
        while (is_not_end) {
            // check if the termination of SLAM system is requested or not
            if (SLAM.terminate_is_requested()|| flag==1) {
                SLAM.request_terminate();
                break;
            }

            is_not_end = video.read(frame);
            if (frame.empty()) {
                continue;
            }
            if (scale != 1.0) {
                cv::resize(frame, frame, cv::Size(), scale, scale, cv::INTER_LINEAR);
            }

            const auto tp_1 = std::chrono::steady_clock::now();

            // input the current frame and estimate the camera pose
           auto cam_pose = SLAM.feed_monocular_frame(frame, timestamp, mask);

            const auto tp_2 = std::chrono::steady_clock::now();

            const auto track_time = std::chrono::duration_cast<std::chrono::duration<double>>(tp_2 - tp_1).count();
            track_times.push_back(track_time);

            timestamp += 1.0 / cfg->camera_->fps_;
            ++num_frame;

            pose_odometry_pub(cam_pose, camera_pose_publisher, odometry_pub_publisher);
            
            // landmarks = SLAM.print();

            // cloud_point_pub(landmarks,cloud_pub_publisher);
            // SLAM.get_keyframes();
            // unsigned int ls_kf_id = SLAM.get_latest_kf_id(); USED
            // SLAM.get_keyframes(ls_kf_id); USED
            // for (size_t i=0; i<kf.size();i++){
            //     std::cout<< kf[i]->id_ <<std::endl;
            //     if (ls_kf_id== kf[i]->id_){
            //     std::cout<<"BIKA EDW "<<std::endl;

            //  landmarks= kf[ls_kf_id]->get_landmarks();
            //     }
            // 
            // std::cout<<kf.size()<<std::endl;
            // landmarks=kf->get_landmarks();
            // clear_landmarks(landmarks);
            // cloud_point_pub(landmarks,cloud_pub_publisher);

            // cloud_point_pub(landmarks,cloud_pub_publisher);
            // std::cout << &c << std::endl;
            // loop_rate.sleep();


        }

        // wait until the loop BA is finished
        while (SLAM.loop_BA_is_running()) {
            std::this_thread::sleep_for(std::chrono::microseconds(5000));
        }
    });

    // run the viewer in the current thread
#ifdef USE_PANGOLIN_VIEWER
    //viewer.run();
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


int main(int argc, char* argv[]) {
#ifdef USE_STACK_TRACE_LOGGER
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
#endif
    ros::init(argc,argv,"run_slam");
    // create options
    popl::OptionParser op("Allowed options");
    auto help = op.add<popl::Switch>("h", "help", "produce help message");
    auto vocab_file_path = op.add<popl::Value<std::string>>("v", "vocab", "vocabulary file path");
    auto cam_num = op.add<popl::Value<unsigned int>>("n", "number", "camera number");
    auto config_file_path = op.add<popl::Value<std::string>>("c", "config", "config file path");
    auto mask_img_path = op.add<popl::Value<std::string>>("", "mask", "mask image path", "");
    auto scale = op.add<popl::Value<float>>("s", "scale", "scaling ratio of images", 1.0);
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
    if (!vocab_file_path->is_set() || !cam_num->is_set()
        || !config_file_path->is_set()) {
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

    // run tracking
    if (cfg->camera_->setup_type_ == openvslam::camera::setup_type_t::Monocular) {
        mono_tracking(cfg, vocab_file_path->value(), cam_num->value(), mask_img_path->value(),
                      scale->value(), map_db_path->value());
    }
    else {
        throw std::runtime_error("Invalid setup type: " + cfg->camera_->get_setup_type_string());
    }

#ifdef USE_GOOGLE_PERFTOOLS
    ProfilerStop();
#endif

    return EXIT_SUCCESS;
}
