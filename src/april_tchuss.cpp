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

    start = high_resolution_clock::now();
    end   = high_resolution_clock::now();

}

void april_follow::get_tag_borders_(apriltag_detection_t * tag)
{
    bounds_[0][0] = ((tag->p[0][0] - tag->c[0]) * 3) + tag->c[0];
    bounds_[0][1] = ((tag->p[0][1] - tag->c[1]) * 3) + tag->c[1];
    bounds_[1][0] = ((tag->p[1][0] - tag->c[0]) * 3) + tag->c[0];
    bounds_[1][1] = ((tag->p[1][1] - tag->c[1]) * 3) + tag->c[1];
    bounds_[2][0] = ((tag->p[2][0] - tag->c[0]) * 3) + tag->c[0];
    bounds_[2][1] = ((tag->p[2][1] - tag->c[1]) * 3) + tag->c[1];
    bounds_[3][0] = ((tag->p[3][0] - tag->c[0]) * 3) + tag->c[0];
    bounds_[3][1] = ((tag->p[3][1] - tag->c[1]) * 3) + tag->c[1];
}

void april_follow::pre_process()
{
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

    shiftx = pTopLeft_x;
    shifty = pTopLeft_y;
    // std::cout << "shifts "<< shiftx << " " << shifty << "\n";

    rRect_        = cv::Rect(pTopLeft_, pBottomRight_);
    // imgRoi_      = cv::Mat(img(rRect).clone());
    imgRoi_       = img_(rRect_).clone();
    im_ = new image_u8_t  
        {   .width  = imgRoi_.cols,
            .height = imgRoi_.rows,
            .stride = imgRoi_.cols,
            .buf    = imgRoi_.data
        };
}

void april_follow::load_image(std::string path)
{
    img_ = cv::imread(path, cv::IMREAD_GRAYSCALE);
}

void april_follow::detect()
{
    // std::cout << "inputting : " << im_->height << " " << im_->width << " " << im_->stride << "\n" ;
    detections_ = apriltag_detector_detect(td_, im_);

    for (int i = 0; i < zarray_size(detections_); i++)
    {

        apriltag_detection_t *det;
        zarray_get(detections_, i, &det);
        if(det->id == tag_n_)
        {
            // std::cout << det->id << " found \n";
            *curr_det_ = *det;
            this->get_tag_borders_(curr_det_);
        }
        

    }
    if (zarray_size(detections_)==0){
        std::cout << "no tag\n";
        reset_bounds();
        }
    // this->get_tag_borders_(curr_det_);
    apriltag_detections_destroy(detections_);
}

void april_follow::show_tag()
{

    // cv::line(img_, cv::Point(bounds_[0][0], bounds_[0][1]),cv::Point(bounds_[1][0], bounds_[1][1]),cv::Scalar(0xff, 0, 0), 2);
    // cv::line(img_, cv::Point(bounds_[0][0], bounds_[0][1]),cv::Point(bounds_[3][0], bounds_[3][1]),cv::Scalar(0xff, 0, 0), 2);
    // cv::line(img_, cv::Point(bounds_[1][0], bounds_[1][1]),cv::Point(bounds_[2][0], bounds_[2][1]),cv::Scalar(0xff, 0, 0), 2);
    // cv::line(img_, cv::Point(bounds_[2][0], bounds_[2][1]),cv::Point(bounds_[3][0], bounds_[3][1]),cv::Scalar(0xff, 0, 0), 2);

    cv::imshow("Detections", img_);
    cv::imshow("imgRoi", imgRoi_);
    if (cv::waitKey(0) >= 0)
    {
        cv::destroyWindow("Detections");
        cv::destroyWindow("imgRoi");
    }
}


int main(int argc, char *argv[])
{

    april_follow node_0 = april_follow(0);
    april_follow node_2 = april_follow(2);
    april_follow node_3 = april_follow(3);

    std::string image_path_root = "/home/kolmogorov/april_test/imgs2/img";
   
    auto start  = high_resolution_clock::now();
    auto end    = high_resolution_clock::now();
    int timetot = 0;

    for (int cnt=0;cnt<100;cnt++){
        std::string path = image_path_root + std::to_string(cnt) + ".jpg"; 
        node_0.load_image(path);
        node_0.pre_process();
        node_0.detect();
        // node.show_tag();
        // node_2.load_image(path);
        // node_2.pre_process();
        // node_2.detect();
        // node_2.show_tag();
        // node_3.load_image(path);
        // node_3.pre_process();
        // node_3.detect();
        end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        timetot += duration.count();
        // std::cout << cnt << " " << duration.count() << "\n";

        start = end;

    }


    std::cout  << " avg " << (float)timetot/100.0 << "ms \n";
}
