//
// Created by Emil Tomov on 7/7/17.
//

#ifndef INC_3DSCANNER_EXTRACTCONTOURS_H
#define INC_3DSCANNER_EXTRACTCONTOURS_H

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
namespace opencvbox
{
    using namespace cv;

    void extractEdges(const Mat& src, Mat& edged);
    void showFrame(std::string name, cv::Mat frame);
    void initTrackBar(std::string name);
    int startVisualLoop(int camIndex, std::string window_name);
}

#endif //INC_3DSCANNER_EXTRACTCONTOURS_H
