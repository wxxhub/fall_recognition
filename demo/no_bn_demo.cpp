#include <stdio.h>

#include <opencv2/opencv.hpp>

#include "fall_recognition/fall_recognition.h"
#include "fall_recognition/timer.hpp"

using namespace caffe;
using namespace cv;

using namespace fall_recognition;

const std::string output_video_path = "/home/wxx/Git/fall_recognition/video/result/output34.mp4";

int main()
{
    std::string net_prototxt_file = "../no_bn/no_bn.prototxt";
    std::string module_file = "../no_bn/no_bn.caffemodel";
    FallRecognition *recog = new FallRecognition(net_prototxt_file, module_file);
    recog->setUseGPU(true);
    recog->setSlopeRadio(1.5);
    recog->setFPS(30);

	VideoCapture cap("../video/test/test34.mp4");

    VideoWriter output_video; //获取当前摄像头的视频信息
    cv::Size S = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH), (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT)); //打开视频路劲，设置基本信息
    output_video.open(output_video_path, CV_FOURCC('X', 'V', 'I', 'D'), 30.0, S, true);

    Mat image;

	timer t;
    while (cap.read(image))
    {
		t.reset();
		recog->detector(image);
		t.stop();
		t.show();

		recog->showResult(image, 10, true);

        output_video.write(image);
    }
    output_video.release();
    return 0;
}