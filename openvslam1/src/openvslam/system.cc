#include "openvslam/system.h"
#include "openvslam/config.h"
#include "openvslam/tracking_module.h"
#include "openvslam/mapping_module.h"
#include "openvslam/global_optimization_module.h"
#include "openvslam/camera/base.h"
#include "openvslam/data/camera_database.h"
#include "openvslam/data/map_database.h"
#include "openvslam/data/bow_database.h"
#include "openvslam/io/trajectory_io.h"
#include "openvslam/io/map_database_io.h"
#include "openvslam/publish/map_publisher.h"
#include "openvslam/publish/frame_publisher.h"
#include "openvslam/data/landmark.h"
#include "openvslam/data/keyframe.h"
#include <thread>

#include <spdlog/spdlog.h>

namespace openvslam {

system::system(const std::shared_ptr<config>& cfg, const std::string& vocab_file_path)
    : cfg_(cfg), camera_(cfg->camera_) {
    spdlog::debug("CONSTRUCT: system");

    std::cout << R"(  ___               __   _____ _      _   __  __ )" << std::endl;
    std::cout << R"( / _ \ _ __  ___ _ _\ \ / / __| |    /_\ |  \/  |)" << std::endl;
    std::cout << R"(| (_) | '_ \/ -_) ' \\ V /\__ \ |__ / _ \| |\/| |)" << std::endl;
    std::cout << R"( \___/| .__/\___|_||_|\_/ |___/____/_/ \_\_|  |_|)" << std::endl;
    std::cout << R"(      |_|                                        )" << std::endl;
    std::cout << std::endl;
    std::cout << "Copyright (C) 2019," << std::endl;
    std::cout << "National Institute of Advanced Industrial Science and Technology (AIST)" << std::endl;
    std::cout << "All rights reserved." << std::endl;
    std::cout << std::endl;
    std::cout << "This is free software," << std::endl;
    std::cout << "and you are welcome to redistribute it under certain conditions." << std::endl;
    std::cout << "See the LICENSE file." << std::endl;
    std::cout << std::endl;

    // show configuration
    std::cout << *cfg_ << std::endl;

    // load ORB vocabulary
    spdlog::info("loading ORB vocabulary: {}", vocab_file_path);
#ifdef USE_DBOW2
    bow_vocab_ = new data::bow_vocabulary();
    try {
        bow_vocab_->loadFromBinaryFile(vocab_file_path);
    }
    catch (const std::exception& e) {
        spdlog::critical("wrong path to vocabulary");
        delete bow_vocab_;
        bow_vocab_ = nullptr;
        exit(EXIT_FAILURE);
    }
#else
    bow_vocab_ = new fbow::Vocabulary();
    bow_vocab_->readFromFile(vocab_file_path);
    if (!bow_vocab_->isValid()) {
        spdlog::critical("wrong path to vocabulary");
        delete bow_vocab_;
        bow_vocab_ = nullptr;
        exit(EXIT_FAILURE);
    }
#endif

    // database
    cam_db_ = new data::camera_database(camera_);
    map_db_ = new data::map_database();
    bow_db_ = new data::bow_database(bow_vocab_);

    // frame and map publisher
    frame_publisher_ = std::shared_ptr<publish::frame_publisher>(new publish::frame_publisher(cfg_, map_db_));
    map_publisher_ = std::shared_ptr<publish::map_publisher>(new publish::map_publisher(cfg_, map_db_));

    // tracking module
    tracker_ = new tracking_module(cfg_, this, map_db_, bow_vocab_, bow_db_);
    // mapping module
    mapper_ = new mapping_module(map_db_, camera_->setup_type_ == camera::setup_type_t::Monocular);
    // global optimization module
    global_optimizer_ = new global_optimization_module(map_db_, bow_db_, bow_vocab_, camera_->setup_type_ != camera::setup_type_t::Monocular);

    // connect modules each other
    tracker_->set_mapping_module(mapper_);
    tracker_->set_global_optimization_module(global_optimizer_);
    mapper_->set_tracking_module(tracker_);
    mapper_->set_global_optimization_module(global_optimizer_);
    global_optimizer_->set_tracking_module(tracker_);
    global_optimizer_->set_mapping_module(mapper_);
}

system::~system() {
    global_optimization_thread_.reset(nullptr);
    delete global_optimizer_;
    global_optimizer_ = nullptr;

    mapping_thread_.reset(nullptr);
    delete mapper_;
    mapper_ = nullptr;

    delete tracker_;
    tracker_ = nullptr;

    delete bow_db_;
    bow_db_ = nullptr;
    delete map_db_;
    map_db_ = nullptr;
    delete cam_db_;
    cam_db_ = nullptr;
    delete bow_vocab_;
    bow_vocab_ = nullptr;

    spdlog::debug("DESTRUCT: system");
}

void system::startup(const bool need_initialize) {
    spdlog::info("startup SLAM system");
    system_is_running_ = true;

    if (!need_initialize) {
        tracker_->tracking_state_ = tracker_state_t::Lost;
    }

    mapping_thread_ = std::unique_ptr<std::thread>(new std::thread(&openvslam::mapping_module::run, mapper_));
    global_optimization_thread_ = std::unique_ptr<std::thread>(new std::thread(&openvslam::global_optimization_module::run, global_optimizer_));
}

void system::shutdown() {
    // terminate the other threads
    mapper_->request_terminate();
    global_optimizer_->request_terminate();
    // wait until they stop
    while (!mapper_->is_terminated()
           || !global_optimizer_->is_terminated()
           || global_optimizer_->loop_BA_is_running()) {
        std::this_thread::sleep_for(std::chrono::microseconds(5000));
    }

    // wait until the threads stop
    mapping_thread_->join();
    global_optimization_thread_->join();

    spdlog::info("shutdown SLAM system");
    system_is_running_ = false;
}

void system::save_frame_trajectory(const std::string& path, const std::string& format) const {
    pause_other_threads();
    io::trajectory_io trajectory_io(map_db_);
    trajectory_io.save_frame_trajectory(path, format);
    resume_other_threads();
}

void system::save_keyframe_trajectory(const std::string& path, const std::string& format) const {
    pause_other_threads();
    io::trajectory_io trajectory_io(map_db_);
    trajectory_io.save_keyframe_trajectory(path, format);
    resume_other_threads();
}

void system::load_map_database(const std::string& path) const {
    pause_other_threads();
    io::map_database_io map_db_io(cam_db_, map_db_, bow_db_, bow_vocab_);
    map_db_io.load_message_pack(path);
    resume_other_threads();
}

void system::save_map_database(const std::string& path) const {
    pause_other_threads();
    io::map_database_io map_db_io(cam_db_, map_db_, bow_db_, bow_vocab_);
    map_db_io.save_message_pack(path);
    resume_other_threads();
}

const std::shared_ptr<publish::map_publisher> system::get_map_publisher() const {
    return map_publisher_;
}

const std::shared_ptr<publish::frame_publisher> system::get_frame_publisher() const {
    return frame_publisher_;
}

void system::enable_mapping_module() {
    std::lock_guard<std::mutex> lock(mtx_mapping_);
    if (!system_is_running_) {
        spdlog::critical("please call system::enable_mapping_module() after system::startup()");
    }
    // resume the mapping module
    mapper_->resume();
    // inform to the tracking module
    tracker_->set_mapping_module_status(true);
}

void system::disable_mapping_module() {
    std::lock_guard<std::mutex> lock(mtx_mapping_);
    if (!system_is_running_) {
        spdlog::critical("please call system::disable_mapping_module() after system::startup()");
    }
    // pause the mapping module
    mapper_->request_pause();
    // wait until it stops
    while (!mapper_->is_paused()) {
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
    // inform to the tracking module
    tracker_->set_mapping_module_status(false);
}

bool system::mapping_module_is_enabled() const {
    return !mapper_->is_paused();
}

void system::enable_loop_detector() {
    std::lock_guard<std::mutex> lock(mtx_loop_detector_);
    global_optimizer_->enable_loop_detector();
}

void system::disable_loop_detector() {
    std::lock_guard<std::mutex> lock(mtx_loop_detector_);
    global_optimizer_->disable_loop_detector();
}

bool system::loop_detector_is_enabled() const {
    return global_optimizer_->loop_detector_is_enabled();
}

bool system::loop_BA_is_running() const {
    return global_optimizer_->loop_BA_is_running();
}

void system::abort_loop_BA() {
    global_optimizer_->abort_loop_BA();
}

Mat44_t system::feed_monocular_frame(const cv::Mat& img, const double timestamp, const cv::Mat& mask) {
    assert(camera_->setup_type_ == camera::setup_type_t::Monocular);

    check_reset_request();

    const Mat44_t cam_pose_cw = tracker_->track_monocular_image(img, timestamp, mask);

    frame_publisher_->update(tracker_);
    if (tracker_->tracking_state_ == tracker_state_t::Tracking) {
        map_publisher_->set_current_cam_pose(cam_pose_cw);
    }

    return cam_pose_cw;
}

Mat44_t system::feed_stereo_frame(const cv::Mat& left_img, const cv::Mat& right_img, const double timestamp, const cv::Mat& mask) {
    assert(camera_->setup_type_ == camera::setup_type_t::Stereo);

    check_reset_request();

    const Mat44_t cam_pose_cw = tracker_->track_stereo_image(left_img, right_img, timestamp, mask);

    frame_publisher_->update(tracker_);
    if (tracker_->tracking_state_ == tracker_state_t::Tracking) {
        map_publisher_->set_current_cam_pose(cam_pose_cw);
    }

    return cam_pose_cw;
}

Mat44_t system::feed_RGBD_frame(const cv::Mat& rgb_img, const cv::Mat& depthmap, const double timestamp, const cv::Mat& mask) {
    assert(camera_->setup_type_ == camera::setup_type_t::RGBD);

    check_reset_request();
    // std::cout<<"OOOOOOO"<<std::endl;
    // std::cout<<rgb_img<<std::endl;
    

    const Mat44_t cam_pose_cw = tracker_->track_RGBD_image(rgb_img, depthmap, timestamp, mask);
       

    frame_publisher_->update(tracker_);
     if (tracker_->tracking_state_ == tracker_state_t::Tracking) {
         map_publisher_->set_current_cam_pose(cam_pose_cw);
        //std::cout<<"camera_pose_cw\n"<<cam_pose_cw<<std::endl<<"-------\n";
        //std::cout<<"get"<<map_publisher_->get_current_cam_pose()<<"\n------\n";
     }

    return cam_pose_cw;
}

void system::pause_tracker() {
    tracker_->request_pause();
}

bool system::tracker_is_paused() const {
    return tracker_->is_paused();
}

void system::resume_tracker() {
    tracker_->resume();
}

void system::request_reset() {
    std::lock_guard<std::mutex> lock(mtx_reset_);
    reset_is_requested_ = true;
}

bool system::reset_is_requested() const {
    std::lock_guard<std::mutex> lock(mtx_reset_);
    return reset_is_requested_;
}

void system::request_terminate() {
    std::lock_guard<std::mutex> lock(mtx_terminate_);
    terminate_is_requested_ = true;
}

bool system::terminate_is_requested() const {
    std::lock_guard<std::mutex> lock(mtx_terminate_);
    return terminate_is_requested_;
}

void system::check_reset_request() {
    std::lock_guard<std::mutex> lock(mtx_reset_);
    if (reset_is_requested_) {
        tracker_->reset();
        reset_is_requested_ = false;
    }
}

void system::pause_other_threads() const {
    // pause the mapping module
    if (mapper_ && !mapper_->is_terminated()) {
        mapper_->request_pause();
        while (!mapper_->is_paused() && !mapper_->is_terminated()) {
            std::this_thread::sleep_for(std::chrono::microseconds(5000));
        }
    }
    // pause the global optimization module
    if (global_optimizer_ && !global_optimizer_->is_terminated()) {
        global_optimizer_->request_pause();
        while (!global_optimizer_->is_paused() && !global_optimizer_->is_terminated()) {
            std::this_thread::sleep_for(std::chrono::microseconds(5000));
        }
    }
}

void system::resume_other_threads() const {
    // resume the global optimization module
    if (global_optimizer_) {
        global_optimizer_->resume();
    }
    // resume the mapping module
    if (mapper_) {
        mapper_->resume();
    }
}


std::vector<openvslam::data::landmark *> system::print(){
    
    // std::vector<openvslam::data::landmark*>  
    std::vector<openvslam::data::landmark *> a = map_db_->get_local_landmarks();
   // auto c = a[0];
    // std::cout << "lm SIZE"<<a.size() << std::endl;
    // if (&a != nullptr && !a.empty()){
    //     return a;
    //     //
    //    // std::cout << type(a[0])<<endl;
    // openvslam::Vec3_t 
    // Eigen::Matrix<double, 3, 1> c = a[0]->get_pos_in_world();
    //     //openvslam::data::keyframe* b = c->get_ref_keyframe();
    // std::cout <<"x:" << c[0] << "y:"<< c[1]<< "z:" << c[2]<<std::endl;
    // }
    // }
     return a;
}
std::vector<openvslam::data::keyframe *> system::get_keyframes(unsigned int id){
    
    // unsigned int size = map_db_->get_num_keyframes();
    std::vector<openvslam::data::keyframe*> keyframes;
    // keyframes.reserve(sizeof(data::keyframe)*size);
    
    keyframes = map_db_->get_all_keyframes();



    // keyframes= map_db_->get_specific_keyframe(id);
    //std::vector<openvslam::data::keyframe *> keyframe_to_ret;
    //keyframe_to_ret.reserve(sizeof(data::keyframe));
    // for (const auto id_keyframe : keyframe){
    //     if (id_keyframe.second->id_ == id)  
    //         keyframe_to_ret.push_back(id_keyframe.second);


    // if (keyframes!=nullptr){
    //  std::cout << keyframes.size() <<std::endl;
    
    // }

    // }
    //  for (size_t x =0 ;x != keyframes.size();x++){


    //      std::cout << keyframes[x]->id_ <<std::endl;
    //  }
        //  if (x->id_==id){
            
            //  keyframe_to_ret.push_back(x);        
    //         return keyframe_to_ret;
    //     }
    // }
//     auto all_keyframes = map_db_->get_all_keyframes();
//     //return keyframe_and_landmarks;
    // for (auto kfs:keyframes){
    //     if (kfs->id_ == id){
    //          std::cout << kfs->id_ <<std::endl;
    //     }

    

    // if (!all_keyframes.empty()){
    //     // Eigen::Matrix<double, 3, 1> c = all_keyframes[0];

   
    // }
    return keyframes;


}

unsigned int system::get_latest_kf_id(){

    return map_db_->get_max_keyframe_id();
}


} // namespace openvslam
