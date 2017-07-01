#include <iostream>
#include <queue>
#include <string>
#include <numeric>
#include <future>
#include <opencv2/videoio.hpp>
#include <opencv/cv.hpp>

#include "caf/all.hpp"


using namespace cv;
using namespace std;


using std::endl;
using std::string;

using namespace caf;

const string window_name("Optical Flow Event Tracker");
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

int main() {
    mutex queue_mutex;
    queue<cv::Mat> frames_queue;

    VideoCapture capture(0);// = VideoCapture("/Users/bemcho/Movies/wall-e.mkv");

    if(!capture.isOpened())
    {

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

    int codec = CV_FOURCC('M', 'J', 'P', 'G');
    double fps = capture.get(CAP_PROP_FPS) > 0 ? capture.get(CAP_PROP_FPS)  : 25.0;
    string filename = "./";
    filename.append(to_string(chrono::system_clock::now().time_since_epoch().count()));
    filename.append("processed.avi");
    VideoWriter writer(filename,
                       codec,
                       fps,
                       Size(capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT)));

    // check if we succeeded
    if (!writer.isOpened()) {
        cerr << "Could not open the output video file for write\n";
        return EXIT_FAILURE;
    }

    namedWindow(window_name, WINDOW_OPENGL);

    Mat flow, cflow, frame;
    UMat gray, prevgray, uflow;


    while (true)
    {
        capture >> frame;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        equalizeHist(gray, gray);

        if( !prevgray.empty() )
        {
            calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
            cvtColor(prevgray, cflow, COLOR_GRAY2BGR);

            uflow.copyTo(flow);
            drawOptFlowMap(flow, frame, 8, 1.5, Scalar(255, 123, 0));
            imshow(window_name, frame);
            frames_queue.push(frame.clone());

        }
        if(frames_queue.size() > 1000)
        {
            std::async(std::launch::async, [&]
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                while(frames_queue.size() > 0)
                {
                    writer.write(frames_queue.front());
                    frames_queue.pop();
                }
            }).get();
        }


        std::swap(prevgray, gray);
        if(waitKey(1)==27) {

            string fileText = "Saving as movie:";
            fileText.append(filename);
            fileText.append(" ...");
            putText(frame,fileText,Point(100,100),3,1,Scalar(255,213,123));

            string framesText = "Total frames: ";
            framesText.append(to_string(frames_queue.size()));
            putText(frame,framesText,Point(100,300),3,1,Scalar(255,213,123));
            imshow(window_name, frame);

            while(frames_queue.size() > 0)
            {
                writer.write(frames_queue.front());
                frames_queue.pop();

            }
            putText(frame,"Done",Point(100,800),3,1,Scalar(255,213,123));
            imshow(window_name, frame);

            break;
        }
    }


     exit(EXIT_SUCCESS);
}
