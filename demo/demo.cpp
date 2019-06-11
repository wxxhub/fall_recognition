#include <stdio.h>

#include <opencv2/opencv.hpp>

#include "fall_recognition/fall_recognition.h"
#include "fall_recognition/timer.hpp"

using namespace caffe;
using namespace cv;

using namespace fall_recognition;

int main()
{
    std::string net_prototxt_file = "../snapshot/MobileNetSSD_deploy.prototxt";
    std::string module_file = "../snapshot/mobilenet_iter_10000.caffemodel";
    FallRecognition *recog = new FallRecognition(net_prototxt_file, module_file);
    recog->setUseGPU(true);
    recog->setSlopeRadio(1.5);

	VideoCapture cap("../video/test/test33.mp4");

    Mat image;

	timer t;
    while (cap.read(image))
    {
        // resize(image, image, Size(720, 720 * image.rows / image.cols));
		t.reset();
		recog->detector(image);
		t.stop();
		t.show();

		recog->showResult(image, 10, true);
    }
    
    return 0;
}