#include <iostream>
#include <queue>
#include <numeric>
#include <future>
#include <opencv2/videoio.hpp>
#include <opencv/cv.hpp>

using namespace cv;
using namespace std;


using std::endl;
using std::string;


const string window_name("Optical Flow Event Tracker");
Scalar getHeatMapColor(double value)
{
    const int NUM_COLORS = 9;
    static float color[NUM_COLORS][3] = { {255,0,0}, {255,192,0},{0,255,64},{0,255,128}, {0,255,0},{64,0,64},{128,0,128}, {128,0,255}, {0,0,255} };
    // A static array of 4 colors:  (blue,   green,  yellow,  red) using {r,g,b} for each.

    int idx1;        // |-- Our desired color will be between these two indexes in "color".
    int idx2;        // |
    float fractBetween = 0;  // Fraction between "idx1" and "idx2" where our value is.

    if(value <= 0)      {  idx1 = idx2 = 0;            }    // accounts for an input <=0
    else if(value >= 1)  {  idx1 = idx2 = NUM_COLORS-1; }    // accounts for an input >=0
    else
    {
        value = value * (NUM_COLORS-1);        // Will multiply value by 3.
        idx1  = floor(value);                  // Our desired color will be after this index.
        idx2  = idx1+1;                        // ... and before this index (inclusive).
        fractBetween = value - float(idx1);    // Distance between the two indexes (0-1).
    }

    float red = ((color[idx2][0] - color[idx1][0])*fractBetween + color[idx1][0]);
    float green = ((color[idx2][1] - color[idx1][1])*fractBetween + color[idx1][1]);
    float blue  = ((color[idx2][2] - color[idx1][2])*fractBetween + color[idx1][2]);
    return Scalar((uint)blue,(uint)green,(uint)red);
}

static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,
                           double, const Scalar& color)
{
    Point prevFlow;
    Point origin(0,0);
    Scalar currentColor = color;

    for(int y = 0; y < cflowmap.rows; y += step)
        for(int x = 0; x < cflowmap.cols; x += step)
        {
            const Point2f& fxy = flow.at<Point2f>(y, x);
            const Point stepPoint(x,y);
            const Point flowPoint(cvRound(x+fxy.x), cvRound(y+fxy.y));

            if(flowPoint != stepPoint)
            {
                if(cv::norm(prevFlow-flowPoint) > norm(step*1.5))
                {
                    currentColor = getHeatMapColor(cv::norm(origin-flowPoint)/255) ;
                }

                line(cflowmap, stepPoint, flowPoint, currentColor);
            }
            prevFlow = flowPoint;
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



    Mat flow, cflow, frame;
    UMat gray, prevgray, uflow;
    namedWindow(window_name, WINDOW_AUTOSIZE);

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
            drawOptFlowMap(flow, frame, 4, 1.5, Scalar(255, 255, 0));
            imshow(window_name, frame);
            frames_queue.push(frame.clone());

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

            break;
        }
    }


     exit(EXIT_SUCCESS);
}
