#include <iostream>
#include <opencv2/videoio.hpp>
#include <opencv/cv.hpp>

#include "caf/all.hpp"


using namespace cv;
using namespace std;


using std::endl;
using std::string;

using namespace caf;

const string window_name("3DScanner");
static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,
                           double, const Scalar& color)
{
    for(int y = 0; y < cflowmap.rows; y += step)
        for(int x = 0; x < cflowmap.cols; x += step)
        {
            const Point2f& fxy = flow.at<Point2f>(y, x);
            const Point stepPoint(x,y);
            const Point flowPoint(cvRound(x+fxy.x), cvRound(y+fxy.y));

            if(flowPoint != stepPoint)
            {
                line(cflowmap, stepPoint, flowPoint, color);
            }

        }
}
Mat flow, cflow, frame;
UMat gray, prevgray, uflow;
int main() {
    VideoCapture capture(0);// = VideoCapture("/Users/bemcho/Movies/wall-e.mkv");

    if(!capture.isOpened())
    {
    namedWindow(window_name, WINDOW_OPENGL);
        for (int i = 1; i < 25; i++)
        {
            capture = VideoCapture(i);
            if (!capture.isOpened())
            {
                capture.release();
                cout << "--(!)Error opening video capture\nYou do have camera plugged in, right?" << endl;

                continue;
            } else {
                cout << "--(!)Camera found on " << i << " device index.";
                break;
            }
            return  EXIT_FAILURE;
        }
    }
    capture.set(CAP_PROP_FRAME_WIDTH, 10000);
    capture.set(CAP_PROP_FRAME_HEIGHT, 10000);
    capture.set(CAP_PROP_FRAME_WIDTH,
                (capture.get(CAP_PROP_FRAME_WIDTH) / 2) <= 1280 ? 1280 : capture.get(CAP_PROP_FRAME_WIDTH) / 2);
    capture.set(CAP_PROP_FRAME_HEIGHT,
                (capture.get(CAP_PROP_FRAME_HEIGHT) / 2) <= 720 ? 720 : capture.get(CAP_PROP_FRAME_HEIGHT) / 2);


    while (true) {


        capture >> frame;
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        if( !prevgray.empty() )
        {
            calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
            cvtColor(prevgray, cflow, COLOR_GRAY2BGR);

            uflow.copyTo(flow);
            drawOptFlowMap(flow, frame, 2, 1.5, Scalar(255, 123, 0));
            imshow(window_name, frame);
        }
        if(waitKey(1)==27)
            break;
        std::swap(prevgray, gray);

    }
    capture.release();
    exit(0);
}
