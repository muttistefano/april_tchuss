#include <iostream>
#include <iomanip>

#include "opencv2/opencv.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>
#include <execution>

#include "apriltag.h"
#include "tag36h11.h"


#define DBG_SHOW false
#define DBG_TAGS true

using namespace std::chrono;


class april_follow
{
    public:
        april_follow(int tag_n);
        void pre_process();
        void load_image(const cv::Mat &img_in);
        void detect();
        void show_tag();
        ~april_follow()
        {
            apriltag_detector_destroy(td_);
            tag36h11_destroy(tf_);
        }

        void reset_bounds()
        {
            bounds_[0][0] = 0;
            bounds_[0][1] = 0;
            bounds_[1][0] = 0;
            bounds_[1][1] = 0;
            bounds_[2][0] = res_width;
            bounds_[2][1] = res_height;
            bounds_[3][0] = res_width;
            bounds_[3][1] = res_height;
            shiftx = 0;
            shifty = 0;
        }

        int seen    = 0;

    private:

        void _get_tag_borders(apriltag_detection_t * tag);

        apriltag_family_t   *tf_         = NULL;
        apriltag_detector_t *td_         = apriltag_detector_create();
        zarray_t *detections_            = NULL;
        apriltag_detection_t * curr_det_ = NULL;
        
        
        // int timetot = 0;
        const int tag_n_   = 0;

        // int res_width  = 1920;
        // int res_height = 1080;

        int res_width  = 1280;
        int res_height = 1024;

        int bounds_[4][2] = {{0,0},{0,0},{res_width,res_height},{res_width,res_height}};
        int shiftx = 0;
        int shifty = 0;

        cv::Point pTopLeft_;
        cv::Point pBottomRight_;
        cv::Rect  rRect_;

        image_u8_t* im_;
        cv::Mat     imgRoi_;
        std::shared_ptr<cv::Mat> img_;
};


class april_manager
{
    public:
        april_manager();
        void add_april_tag(int index);
        void load_image(const cv::Mat &img_in);
        void detect();
        int  count();
        void debug();

    private:

        std::vector<std::shared_ptr<april_follow>> _april_followers;
};