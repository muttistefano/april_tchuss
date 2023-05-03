#include "april_tchuss/april_tchuss.hpp"

using namespace std::chrono;


april_follow::april_follow(int tag_n):
tag_n_(tag_n)
{
    tf_ = tag36h11_create();
    apriltag_detector_add_family(td_, tf_);
    curr_det_ = new apriltag_detection_t();
    
    td_->quad_decimate = 1.0;
    td_->quad_sigma    = 0.8;
    td_->nthreads      = 2;
    td_->debug         = 0;
    td_->refine_edges  = 1;

    _c_time      = high_resolution_clock::now();
    _c_time_old  = high_resolution_clock::now();

}

void april_follow::load_image(const cv::Mat &img_in)
{
    //TODO std::move
    img_ = std::make_shared<cv::Mat>(img_in);
    // img_ = cv::imread(path, cv::IMREAD_GRAYSCALE);
    _c_time  = high_resolution_clock::now();
}

void april_follow::_get_tag_vel(apriltag_detection_t * tag)
{
    // std::cout << "1 " << _vx << " " << _vy << "\n";
    // std::cout << "2 " << duration<double> (_c_time_old - _c_time).count() << " " << _vy << "\n";
    // std::cout << "3 " << _cx_old << " " << _cy_old << "\n";
    // std::cout << "4 " <<shiftx << " " << shifty << "\n";
    _vx = (_cx_old - (double)(shiftx + tag->c[0]) );// / duration<double>(_c_time_old - _c_time).count();
    _vy = (_cy_old - (double)(shifty + tag->c[1]) );// / duration<double>(_c_time_old - _c_time).count();
    _c_time_old = _c_time;
    _cx_old = (double)(shiftx + tag->c[0]);
    _cy_old = (double)(shifty + tag->c[1]);
    // std::cout << "1 " << _vx << " " << _vy << "\n";
}

void april_follow::_get_tag_borders_center(apriltag_detection_t * tag)
{
    bounds_[0][0] = ((tag->p[0][0] - tag->c[0]) * (_roi_tag_scale_x + _vx)) + tag->c[0];
    bounds_[0][1] = ((tag->p[0][1] - tag->c[1]) * (_roi_tag_scale_y + _vy)) + tag->c[1];
    bounds_[1][0] = ((tag->p[1][0] - tag->c[0]) * (_roi_tag_scale_x + _vx)) + tag->c[0];
    bounds_[1][1] = ((tag->p[1][1] - tag->c[1]) * (_roi_tag_scale_y + _vy)) + tag->c[1];
    bounds_[2][0] = ((tag->p[2][0] - tag->c[0]) * (_roi_tag_scale_x + _vx)) + tag->c[0];
    bounds_[2][1] = ((tag->p[2][1] - tag->c[1]) * (_roi_tag_scale_y + _vy)) + tag->c[1];
    bounds_[3][0] = ((tag->p[3][0] - tag->c[0]) * (_roi_tag_scale_x + _vx)) + tag->c[0];
    bounds_[3][1] = ((tag->p[3][1] - tag->c[1]) * (_roi_tag_scale_y + _vy)) + tag->c[1];
}

void april_follow::pre_process()
{
    // std::cout << "preprocess"  << "\n" ;
    int pTopLeft_x     = std::min({bounds_[0][0],bounds_[1][0],bounds_[2][0],bounds_[3][0]}) ;
    pTopLeft_x         = std::clamp(pTopLeft_x + shiftx,0,res_width);
    int pTopLeft_y     = std::min({bounds_[0][1],bounds_[1][1],bounds_[2][1],bounds_[3][1]}) ;
    pTopLeft_y         = std::clamp(pTopLeft_y + shifty,0,res_height);
    pTopLeft_          = cv::Point(pTopLeft_x,pTopLeft_y);

    int pBottomRight_x = std::max({bounds_[0][0],bounds_[1][0],bounds_[2][0],bounds_[3][0]}) ;
    pBottomRight_x     = std::clamp(pBottomRight_x + shiftx,0,res_width);
    int pBottomRight_y = std::max({bounds_[0][1],bounds_[1][1],bounds_[2][1],bounds_[3][1]}) ;
    pBottomRight_y     = std::clamp(pBottomRight_y + shifty,0,res_height);
    pBottomRight_      = cv::Point(pBottomRight_x,pBottomRight_y);

    // std::cout << " ptop pbot " << pTopLeft_ << " " << pBottomRight_ << "\n";
    // std::cout << " ptop pbot " << img_->rows << " " << img_->cols << "\n";

    shiftx = pTopLeft_x;
    shifty = pTopLeft_y;
    // std::cout << "shifts "<< shiftx << " " << shifty << "\n";

    rRect_        = cv::Rect(pTopLeft_, pBottomRight_);
    //TOTO withoud clone it doesen't work
    imgRoi_       = (*img_)(rRect_).clone();
    im_ = new image_u8_t  
        {   .width  = imgRoi_.cols,
            .height = imgRoi_.rows,
            .stride = imgRoi_.cols,
            .buf    = imgRoi_.data
        };
}



void april_follow::detect()
{
    // std::cout << "inputting : " << im_->height << " " << im_->width << " " << im_->stride << "\n" ;
    // std::cout << "detect"  << "\n" ;
    detections_ = apriltag_detector_detect(td_, im_);

    for (int i = 0; i < zarray_size(detections_); i++)
    {
        //TODO not important allocation
        apriltag_detection_t *det;
        zarray_get(detections_, i, &det);
        if(det->id == tag_n_)
        {
            // std::cout << det->id << " found \n";
            *curr_det_ = *det;
            this->_get_tag_borders_center(curr_det_);
            this->_get_tag_vel(curr_det_);
            seen++;
        }
    }

    if (zarray_size(detections_)==0){
        std::cout << "no tag\n";
        reset_bounds();
        }
    apriltag_detections_destroy(detections_);
}

void april_follow::show_tag()
{

    cv::line(*img_, cv::Point(_cx_old, _cy_old),cv::Point(_cx_old + _vx, _cy_old + _vy),cv::Scalar(0xff, 0, 0), 2);
    // cv::line(*img_, cv::Point(bounds_[0][0], bounds_[0][1]),cv::Point(bounds_[1][0], bounds_[1][1]),cv::Scalar(0xff, 0, 0), 2);
    // cv::line(*img_, cv::Point(bounds_[0][0], bounds_[0][1]),cv::Point(bounds_[3][0], bounds_[3][1]),cv::Scalar(0xff, 0, 0), 2);
    // cv::line(*img_, cv::Point(bounds_[1][0], bounds_[1][1]),cv::Point(bounds_[2][0], bounds_[2][1]),cv::Scalar(0xff, 0, 0), 2);
    // cv::line(*img_, cv::Point(bounds_[2][0], bounds_[2][1]),cv::Point(bounds_[3][0], bounds_[3][1]),cv::Scalar(0xff, 0, 0), 2);

    // cv::imshow("Detections_" + std::to_string(this->tag_n_), *img_);
    cv::imshow("imgRoi_" + std::to_string(this->tag_n_), imgRoi_);
    // if (cv::waitKey(0) >= 0)
    // {
    //     cv::destroyWindow("Detections");
    //     cv::destroyWindow("imgRoi");
    // }
}

april_manager::april_manager(){}

void april_manager::add_april_tag(int index)
{
    _april_followers.push_back(std::make_shared<april_follow>(index));
}

void april_manager::load_image(const cv::Mat &img_in)
{
    std::for_each(std::execution::par,
    _april_followers.begin(),_april_followers.end(),
    [&img_in]
    (auto&& item)
    {
        item->load_image(img_in);
    });
}


void april_manager::detect()
{
    std::for_each(std::execution::par,
    _april_followers.begin(),_april_followers.end(),
    []
    (auto&& item)
    {
        item->pre_process();
        item->detect();
    });
}

int april_manager::count()
{
    int count = 0;
    std::for_each(
    _april_followers.begin(),_april_followers.end(),
    [&count]
    (auto&& item)
    {
        count += item->seen;
    });

    return count;
}

void april_manager::debug()
{

    int count = 0;
    std::for_each(
    _april_followers.begin(),_april_followers.end(),
    [&count]
    (auto&& item)
    {
        item->show_tag();
    });

    cv::imshow("Base image", *_april_followers[0]->img_);

    if (cv::waitKey(1) == 27 )
    {
        // cv::destroyAllWindows();
        
    }
}