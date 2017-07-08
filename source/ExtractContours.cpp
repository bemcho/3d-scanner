//
// Created by Emil Tomov on 7/7/17.
//

#include "ExtractContours.h"
namespace opencvbox {
    using namespace cv;

    Mat frame, dst, detected_edges;
    int edgeThresh = 1;
    int lowThreshold;
    int const max_lowThreshold = 100;
    int ratio = 3;
    int kernel_size = 3;

    static void CannyThreshold(int, void *) {

        //![reduce_noise]
        /// Reduce noise with a kernel 3x3
        cv::GaussianBlur(frame, frame, Size(3, 3),1,1);

        //![reduce_noise]

        //![canny]
        /// Canny detector
        cv::Canny(frame, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
        //![canny]

        dst.create(frame.size(), frame.type());
        dst = Scalar::all(0);
        frame.copyTo(dst, detected_edges);
    }

    void extractEdges(const Mat& src, Mat& edged)
    {
        src.copyTo(frame);
        CannyThreshold(0,0);
        dst.copyTo(edged);
    }

    void showFrame(std::string name, cv::Mat frame) {
        CannyThreshold(0, 0);
        cv::imshow(name, dst);
    }

    void initTrackBar(std::string name) {
        createTrackbar("Min Threshold:", name, &lowThreshold, max_lowThreshold, CannyThreshold);
    }

    int startVisualLoop(int camIndex, std::string window_name) {

        cv::namedWindow(window_name, cv::WINDOW_OPENGL);
        cv::VideoCapture capture(camIndex);
        if (!capture.isOpened())
            return EXIT_FAILURE;

        capture >> frame;


        //![create_trackbar]
        /// Create a Trackbar for user to enter threshold
        initTrackBar(window_name);
        //![create_trackbar]

        /// Show the image
        CannyThreshold(0, 0);
        showFrame(window_name, frame);

        for (;;) {
            capture >> frame;
            showFrame(window_name, frame);
            if (cvWaitKey(1) == 27) {
                capture.release();
                return EXIT_SUCCESS;
            }
        }
    }
}