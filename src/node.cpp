#include "april_tchuss/april_tchuss.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>

int main(int argc, char *argv[])
{

    // std::ifstream input("/home/kolmogorov/april_test/Acquisizione02.raw", std::ios::binary );

    // std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    // cv::Mat ImMat;
    // ImMat = cv::imdecode(buffer, 1);

    april_manager mng = april_manager();
    // mng.add_april_tag(0);
    // mng.add_april_tag(2);
    mng.add_april_tag(3);

    // std::string image_path_root = "/home/kolmogorov/april_test/imgs1/img";
    std::string image_path_root = "/home/kolmogorov/april_test/bari2/img_";
   
    auto start  = high_resolution_clock::now();
    auto end    = high_resolution_clock::now();
    int timetot = 0;

    for (int cnt=0;cnt<2000;cnt++){
        std::string path = image_path_root + std::to_string(cnt) + ".jpg"; 
        cv::Mat img_tmp = cv::imread(path, cv::IMREAD_GRAYSCALE);
        
        mng.load_image(img_tmp);
        mng.detect();
        mng.debug();


        end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        timetot += duration.count();
        // std::cout << cnt << " " << duration.count() << "\n";

        start = end;

    }

    // int seen = node_0.seen + node_2.seen + node_3.seen;
    // std::cout  << " seen " << seen << "\n";
    std::cout  << " seen " << mng.count() << "\n";
    std::cout  << " avg " << (float)timetot/100.0 << "ms \n";
}
