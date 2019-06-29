#include <stdio.h>

#include <opencv2/opencv.hpp>

#include "fall_recognition/fall_recognition.h"
#include "fall_recognition/timer.hpp"

// #define SAVE_VIDEO

using namespace caffe;
using namespace cv;

using namespace fall_recognition;

int main()
{
    VideoCapture cap("../video/test/test33.mp4");

#ifdef SAVE_VIDEO
    const std::string output_video_path = "/home/wxx/Git/fall_recognition/video/result/output33.mp4";
    VideoWriter output_video; //获取当前摄像头的视频信息
    cv::Size S = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH), (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT)); //打开视频路劲，设置基本信息
    output_video.open(output_video_path, CV_FOURCC('X', 'V', 'I', 'D'), 30.0, S, true);
#endif /* SAVE_VIDEO */

    std::string net_prototxt_file = "../snapshot/no_bn.prototxt";
    std::string module_file = "../snapshot/no_bn.caffemodel";

    FallRecognition *recog = new FallRecognition(net_prototxt_file, module_file);
    recog->setUseGPU(true);
    recog->setSlopeRadio(1.5);
    recog->setFPS(30);

    Mat image;

	timer t;
    while (cap.read(image))
    {
        // resize(image, image, Size(720, 720 * image.rows / image.cols));
		t.reset();
		recog->detector(image);
		t.stop();
		t.show();

        recog->showResult(image);
#ifdef SAVE_VIDEO
		output_video.write(image);
#endif /* SAVE_VIDEO */
    }

#ifdef SAVE_VIDEO
    output_video.release();
#endif /* SAVE_VIDEO */
    return 0;
}