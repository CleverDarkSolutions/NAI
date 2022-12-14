#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
// g++ `pkg-config --cflags opencv4` cv2.cpp `pkg-config --libs opencv4`
using namespace std;
using namespace cv;
int main(int argc, char **argv)
{
    std::vector<int> lower = {0,99,239};
    std::vector<int> upper = {255,255,255};
    VideoCapture cap1(0);
    if (!cap1.isOpened())
        return -1;
    //namedWindow("pierwsze", WINDOW_AUTOSIZE);
    namedWindow("detected", WINDOW_AUTOSIZE);
//    createTrackbar("lh", "detected", &lower[0], 255);
//    createTrackbar("ls", "detected", &lower[1], 255);
//    createTrackbar("lv", "detected", &lower[2], 255);
//    createTrackbar("hh", "detected", &upper[0], 255);
//    createTrackbar("hs", "detected", &upper[1], 255);
//    createTrackbar("hv", "detected", &upper[2], 255);
    while (true)
    {
        Mat f1, dst, detected,dilated;
        cap1.read(f1);
        cvtColor(f1, dst, COLOR_BGR2HSV);

        //imshow("pierwsze", f1);
        //imshow("wynik", dst);
        inRange(dst, lower,upper, detected);
        auto kernel = getStructuringElement(MORPH_ELLIPSE,Size{5,5});
        erode(detected, dilated, kernel);
        dilate(dilated, dilated, kernel);
        //imshow("dilated", dilated);
        vector<vector<Size>> contours;
        vector<Vec4i> hierarchy;
        findContours (dilated, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
        //cout << contours.size() << endl;
        auto selected = [](auto a, auto b){
            return contourArea(a) < contourArea(b);};
        std::sort(contours.begin(), contours.end(), selected);
        //cout << " " << selected[0] << endl;


        if(contours.size()>1 && contours.at(1).size() > 1 && contours.at(0).size() > 1){
            if (abs((contours.at(0).at(0).height) - (contours.at(1).at(0).height)) < 10){
                cv::line(dst, contours.at(0).at(0), contours.at(1).at(0), {255,0,0}, 1);
                cv::line(dst, contours.at(0).at(2), contours.at(1).at(2), {255,0,0}, 1);
            }
        }
        imshow("wynik", dst);   
        imshow("detected", dilated);
        if (waitKey(1) == 27)
            break;
    }
    return 0;
}