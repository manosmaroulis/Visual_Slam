// #include "/home/nitlab/catkin_ws/camera/openvslam/example/util/tum_rgbd_util.h"
#ifdef USE_PANGOLIN_VIEWER
#include <pangolin_viewer/viewer.h>
#elif USE_SOCKET_PUBLISHER
#include <socket_publisher/publisher.h>
#endif

#include <openvslam/system.h>
#include <openvslam/config.h>

#include <iostream>
#include <chrono>
#include <numeric>

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <spdlog/spdlog.h>
#include <popl.hpp>
#include<deque>
#ifdef USE_STACK_TRACE_LOGGER
#include <glog/logging.h>
#endif

#ifdef USE_GOOGLE_PERFTOOLS
#include <gperftools/profiler.h>
#endif



class binsem {
public:
    explicit binsem(int init_count = count_max)
      : count_(init_count) {}

    // P-operation / acquire
    void wait()
    {
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [=]{ return 0 < count_; });
        --count_;
    }
    bool try_wait()
    {
        std::lock_guard<std::mutex> lk(m_);
        if (0 < count_) {
            --count_;
            return true;
        } else {
            return false;
        }
    }
    // V-operation / release
    void signal()
    {
        std::lock_guard<std::mutex> lk(m_);
        if (count_ < count_max) {
            ++count_;
            cv_.notify_one();
        }
    }

    // Lockable requirements
    void lock() { wait(); }
    bool try_lock() { return try_wait(); }
    void unlock() { signal(); }

private:
    static const int count_max = 1;
    int count_;
    std::mutex m_;
    std::condition_variable cv_;
};

binsem smphSignalMainToThread1(0);
binsem smphSignalThreadToMain1(0);

binsem smphSignalMainToThread2(0);
binsem smphSignalThreadToMain2(0);

std::atomic<int> counter(0);

std::atomic<int> depth_counter(0);



std::deque<cv::Mat>image;
std::deque<cv::Mat>depth_image;







void rgbd_tracking(const std::shared_ptr<openvslam::config>& cfg,
                   const std::string& vocab_file_path, const std::string& mask_img_path,
                   const bool eval_log,const std::string& map_db_path) {
    //tum_rgbd_sequence sequence(sequence_dir_path);
    //const auto frames = sequence.get_frames();
     const cv::Mat mask = mask_img_path.empty() ? cv::Mat{} : cv::imread(mask_img_path, cv::IMREAD_UNCHANGED);
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

    std::vector<double> track_times;
    // track_times.reserve(frames.size());
    const auto tp_0 = std::chrono::steady_clock::now();


    // cv::Mat* image = new cv::Mat();
    // cv::Mat* depth_image = new cv::Mat(); 
    
   
    // cv::Mat empty;
    const ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);


    // std::thread thread1([&]() {
        
    image_transport::Subscriber sub = it.subscribe("camera/color/image_raw", 10, [&](const sensor_msgs::ImageConstPtr& msg) {
        // const auto tp_1 = std::chrono::steady_clock::now();
        //const auto timestamp = std::chrono::duration_cast<std::chrono::duration<double>>(tp_1 - tp_0).count();
        // std::cout<<"WAITING COLOR"<<std::endl;
        // smphSignalThreadToThread.wait();
        // input the current frame and estimate the camera pose
        //SLAM.feed_monocular_frame(cv_bridge::toCvShare(msg, "bgr8")->image, timestamp, mask);
        // if (depth_counter >= counter){
            // if(counter<=depth_counter)

            image.push_back(cv_bridge::toCvShare(msg, "bgr8")->image);
        

            // else{
            //     // image.release();
            // }
        // }


        //const auto tp_2 = std::chrono::steady_clock::now();

        //const auto track_time = std::chrono::duration_cast<std::chrono::duration<double>>(tp_2 - tp_1).count();
        //track_times.push_back(track_time);
        // counter++;
        // std::cout<<"callback color COUNTER = "<<counter<<std::endl;
        // smphSignalMainToThread.signal();
        // smphSignalMainToThread1.signal();
        // smphSignalThreadToMain1.wait();

    
        });
    // ros::spin();
    
    // )};


    image_transport::Subscriber sub_depth = it.subscribe("/camera/aligned_depth_to_color/image_raw", 10, [&](const sensor_msgs::ImageConstPtr& msg) {
        //const auto tp_1 = std::chrono::steady_clock::now();
        //const auto timestamp = std::chrono::duration_cast<std::chrono::duration<double>>(tp_1 - tp_0).count();
        // std::cout<<"WAITING DEPTH"<<std::endl;
        
        // smphSignalMainToThread2.wait();
        depth_image.push_back(cv_bridge::toCvShare(msg, "bgr8")->image);
        // input the current frame and estimate the camera pose
        //SLAM.feed_monocular_frame(cv_bridge::toCvShare(msg, "bgr8")->image, timestamp, mask);
        // depth_counter++;
        // std::cout<<"callback depth COUNTER = "<<depth_counter<<std::endl;
        // smphSignalMainToThread1.signal();
        // smphSignalThreadToMain1.wait();

        // smphSignalMainToThread.signal();
        //const auto tp_2 = std::chrono::steady_clock::now();

        //const auto track_time = std::chrono::duration_cast<std::chrono::duration<double>>(tp_2 - tp_1).count();
        //track_times.push_back(track_time);
    });







    unsigned int num_frame = 0;

    bool is_not_end = true;


#ifdef USE_PANGOLIN_VIEWER
    // viewer.run();
     std::thread thread1([&]() {
        viewer.run();
        if (SLAM.terminate_is_requested()) {
            // wait until the loop BA is finished
            // std::cout<<"ti ginete reeeeeeeeeeeeee"<<std::endl;
            while (SLAM.loop_BA_is_running()) {
                std::this_thread::sleep_for(std::chrono::microseconds(5000));
            }
            ros::shutdown();
        }
    });
#elif USE_SOCKET_PUBLISHER
    publisher.run();
#endif

    ros::Rate rate(10);




    // run the SLAM in another thread
    // std::thread thread([&]() {
        
        // int flag=0;
        while(is_not_end){

            if (SLAM.terminate_is_requested()) {
                std::cout<<"ti ginete"<<std::endl;
                break;
            }
            // std::cout<<"THREAD TRY WAIT"<<std::endl;
            // smphSignalMainToThread1.wait();
            // smphSignalMainToThread2.wait();   
           // std::cout<<"IMAGE_SIZE "<<image.size()<<"DEPTH_IMAGE_SIZE "<<depth_image.size()<<std::endl;
            if (image.size()!=0 && depth_image.size()!=0){
                
                const auto tp_1 = std::chrono::steady_clock::now();
                const auto timestamp = std::chrono::duration_cast<std::chrono::duration<double>>(tp_1 - tp_0).count();
                // std::cout<<image.front()<<std::endl;
                SLAM.feed_RGBD_frame(image.front(), depth_image.front(),timestamp);
                // image.release();
                // image = nullptr;
                // std::cout<<depth_image.front()<<std::endl;
                image.pop_front();
                depth_image.pop_front();
                // ros::Duration(0.1).sleep();
                // std::this_thread::sleep_for(std::chrono::microseconds(5000));
                //depth_image.release();
            }
            // std::cout<<"SIGNAL"<<std::endl;
            // smphSignalThreadToMain1.signal();
            // smphSignalThreadToMain2.signal();
            rate.sleep();
            ros::spinOnce();
        }
    // return;
    // });

    // run the viewer in the current thread


    //  std::cout<<"eisai pio xazos"<<std::endl;
    

// #ifdef USE_PANGOLIN_VIEWER
//     viewer.request_terminate();
//     thread1.join();

// #endif

    //  smphSignalMainToThread1.signal();
    std::cout<<"eisai xazos"<<std::endl;
    thread1.join();

    // shutdown the SLAM process
    SLAM.shutdown();

    // if (eval_log) {
    //     // output the trajectories for evaluation
    //     SLAM.save_frame_trajectory("frame_trajectory.txt", "TUM");
    //     SLAM.save_keyframe_trajectory("keyframe_trajectory.txt", "TUM");
    //     // output the tracking times for evaluation
    //     std::ofstream ofs("track_times.txt", std::ios::out);
    //     if (ofs.is_open()) {
    //         for (const auto track_time : track_times) {
    //             ofs << track_time << std::endl;
    //         }
    //         ofs.close();
    //     }
    // }

    if (!map_db_path.empty()) {
        // output the map database
        SLAM.save_map_database(map_db_path);
    }

    // std::sort(track_times.begin(), track_times.end());
    // const auto total_track_time = std::accumulate(track_times.begin(), track_times.end(), 0.0);
    // std::cout << "median tracking time: " << track_times.at(track_times.size() / 2) << "[s]" << std::endl;
    // std::cout << "mean tracking time: " << total_track_time / track_times.size() << "[s]" << std::endl;
}











void mono_tracking(const std::shared_ptr<openvslam::config>& cfg, const std::string& vocab_file_path,
                   const std::string& mask_img_path, const bool eval_log, const std::string& map_db_path) {
    // load the mask image
    const cv::Mat mask = mask_img_path.empty() ? cv::Mat{} : cv::imread(mask_img_path, cv::IMREAD_GRAYSCALE);

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

    std::vector<double> track_times;
    const auto tp_0 = std::chrono::steady_clock::now();

    // initialize this node
    const ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);

    // run the SLAM as subscriber
    image_transport::Subscriber sub = it.subscribe("camera/color/image_raw", 1, [&](const sensor_msgs::ImageConstPtr& msg) {
        const auto tp_1 = std::chrono::steady_clock::now();
        const auto timestamp = std::chrono::duration_cast<std::chrono::duration<double>>(tp_1 - tp_0).count();

        // input the current frame and estimate the camera pose
        SLAM.feed_monocular_frame(cv_bridge::toCvShare(msg, "bgr8")->image, timestamp, mask);

        const auto tp_2 = std::chrono::steady_clock::now();

        const auto track_time = std::chrono::duration_cast<std::chrono::duration<double>>(tp_2 - tp_1).count();
        track_times.push_back(track_time);
    });

    // run the viewer in another thread
#ifdef USE_PANGOLIN_VIEWER
    std::thread thread1([&]() {
        viewer.run();
        if (SLAM.terminate_is_requested()) {
            // wait until the loop BA is finished
            while (SLAM.loop_BA_is_running()) {
                std::this_thread::sleep_for(std::chrono::microseconds(5000));
            }
            ros::shutdown();
        }
    });
#elif USE_SOCKET_PUBLISHER
    std::thread thread([&]() {
        publisher.run();
        if (SLAM.terminate_is_requested()) {
            // wait until the loop BA is finished
            while (SLAM.loop_BA_is_running()) {
                std::this_thread::sleep_for(std::chrono::microseconds(5000));
            }
            ros::shutdown();
        }
    });
#endif

    ros::spin();

    // automatically close the viewer
#ifdef USE_PANGOLIN_VIEWER
    viewer.request_terminate();
    thread1.join();
#elif USE_SOCKET_PUBLISHER
    publisher.request_terminate();
    thread.join();
#endif

    // shutdown the SLAM process
    SLAM.shutdown();

    if (eval_log) {
        // output the trajectories for evaluation
        SLAM.save_frame_trajectory("frame_trajectory.txt", "TUM");
        SLAM.save_keyframe_trajectory("keyframe_trajectory.txt", "TUM");
        // output the tracking times for evaluation
        std::ofstream ofs("track_times.txt", std::ios::out);
        if (ofs.is_open()) {
            for (const auto track_time : track_times) {
                ofs << track_time << std::endl;
            }
            ofs.close();
        }
    }

    if (!map_db_path.empty()) {
        // output the map database
        SLAM.save_map_database(map_db_path);
    }

    if (track_times.size()) {
        std::sort(track_times.begin(), track_times.end());
        const auto total_track_time = std::accumulate(track_times.begin(), track_times.end(), 0.0);
        std::cout << "median tracking time: " << track_times.at(track_times.size() / 2) << "[s]" << std::endl;
        std::cout << "mean tracking time: " << total_track_time / track_times.size() << "[s]" << std::endl;
    }
}

int main(int argc, char* argv[]) {
#ifdef USE_STACK_TRACE_LOGGER
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
#endif
    ros::init(argc, argv, "run_slam");

    // create options
    popl::OptionParser op("Allowed options");
    auto help = op.add<popl::Switch>("h", "help", "produce help message");
    auto vocab_file_path = op.add<popl::Value<std::string>>("v", "vocab", "vocabulary file path");
    auto setting_file_path = op.add<popl::Value<std::string>>("c", "config", "setting file path");
    auto mask_img_path = op.add<popl::Value<std::string>>("", "mask", "mask image path", "");
    auto debug_mode = op.add<popl::Switch>("", "debug", "debug mode");
    auto eval_log = op.add<popl::Switch>("", "eval-log", "store trajectory and tracking times for evaluation");
    auto map_db_path = op.add<popl::Value<std::string>>("", "map-db", "store a map database at this path after SLAM", "");
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
    if (!vocab_file_path->is_set() || !setting_file_path->is_set()) {
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
        cfg = std::make_shared<openvslam::config>(setting_file_path->value());
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
        mono_tracking(cfg, vocab_file_path->value(), mask_img_path->value(), eval_log->is_set(), map_db_path->value());
        std::cout<<"run MONO\n";
    }
    else if (cfg->camera_->setup_type_ == openvslam::camera::setup_type_t::RGBD){
        // throw std::runtime_error("Invalid setup type: " + cfg->camera_->get_setup_type_string());
        rgbd_tracking(cfg,vocab_file_path->value(),mask_img_path->value(),eval_log->is_set(), map_db_path->value());
    }

#ifdef USE_GOOGLE_PERFTOOLS
    ProfilerStop();
#endif

    return EXIT_SUCCESS;
}
