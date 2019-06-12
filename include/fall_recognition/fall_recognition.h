/*
 * fall_recognition.h
 *
 *  Created on: 2019. 6. 11
 *      Author: wxx
 *      E-mail: wxx0520@foxmail.com
 */

#ifndef _FALL_RECOGNITION_H_
#define _FALL_RECOGNITION_H_

#include <vector>

#include <caffe/caffe.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "fall_recognition/state_judge.h"
#include "fall_recognition/bind_id.h"

namespace fall_recognition
{

class FallRecognition
{
public:
    FallRecognition(const std::string net_prototxt_file, const std::string module_file);
    FallRecognition(const std::string net_prototxt_file, const std::string module_file, int detector_num);
    ~FallRecognition();

    void setFPS(int fps);
    void setUseGPU(bool open);
    void detector(cv::Mat image);

    void showResult(cv::Mat img, int wait_key, bool show_all);
    void setSlopeRadio(float radio);

private:
    caffe::Net<float>* net_;
    cv::Size input_size_;
    std::vector<std::vector<float> > detections_;
    std::vector<StateJudge*> state_judge_;
    std::vector<int> states_;

    BindID *bind_id_;

    int fps_;
    int num_channels_;
    int detector_num_;

    const float mean_value_;

    void init(const std::string net_prototxt_file, const std::string module_file);
    void judge();

    void wrapInputLayer(std::vector<cv::Mat>* input_channels);
    void preProcess(const cv::Mat& img, std::vector<cv::Mat>* input_channels);
};

}

#endif /* _FALL_RECOGNITION_H_ */