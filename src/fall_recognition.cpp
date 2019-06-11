#include "fall_recognition/fall_recognition.h"

#include "fall_recognition/timer.hpp"

using namespace caffe;
using namespace cv;

using namespace fall_recognition;

FallRecognition::FallRecognition(const std::string net_prototxt_file, const std::string module_file)
    : mean_value_(127.5),
      detector_num_(5)
{
    init(net_prototxt_file, module_file);
}

FallRecognition::FallRecognition(const std::string net_prototxt_file, const std::string module_file, int detector_num)
    : mean_value_(127.5)
{
    detector_num_ = detector_num;
    init(net_prototxt_file, module_file);
}

FallRecognition::~FallRecognition()
{

}

void FallRecognition::init(const std::string net_prototxt_file, const std::string module_file)
{
    net_ = new Net<float>(net_prototxt_file, TEST);

    net_->CopyTrainedLayersFrom(module_file);

    CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
    CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

    Blob<float>* input_layer = net_->input_blobs()[0];
    num_channels_ = input_layer->channels();
    CHECK(num_channels_ == 3 || num_channels_ == 1)
        << "Input layer should have 1 or 3 channels.";

    input_size_ = Size(input_layer->width(), input_layer->height());

    bind_id_ = new BindID(detector_num_);

    state_judge_.resize(detector_num_);
    states_.resize(detector_num_);

    for (int i = 0; i < state_judge_.size(); ++i)
    {
        state_judge_[i] = new StateJudge();
    }
}

void FallRecognition::setFPS(int fps)
{
    fps_ = fps;
}

void FallRecognition::setUseGPU(bool open)
{
    if (open)
        Caffe::set_mode(Caffe::GPU);
    else
        Caffe::set_mode(Caffe::CPU);
}

void FallRecognition::detector(Mat image)
{
    detections_.clear();

    /* Forward dimension change to all layers. */
    Blob<float>* input_layer = net_->input_blobs()[0];
    input_layer->Reshape(1, num_channels_,
                        input_size_.height, input_size_.width);
    net_->Reshape();

    vector<Mat> input_channels;

    wrapInputLayer(&input_channels);

    preProcess(image, &input_channels);

    timer t;

    net_->Forward();

    Blob<float>* result_blob = net_->output_blobs()[0];
    const float* result = result_blob->cpu_data();
    const int num_detector = result_blob->height();

    for (int i = 0; i < num_detector; ++i)
    {
        if (result[0] == -1)
        {
            result += 7;
            continue;
        }

        vector<float> detection(result, result + 7);
        detections_.push_back(detection);
        result += 7;

        // add id
        int up_x = static_cast<int>(detection[3] * image.cols);
        int up_y = static_cast<int>(detection[4] * image.rows);
        int down_x = static_cast<int>(detection[5] * image.cols);
        int down_y = static_cast<int>(detection[6] * image.rows);
        bind_id_->add(up_x, up_y, down_x, down_y, i);
    }

    t.reset();
    bind_id_->match();
    t.stop();
    printf("f:");
    t.show();

    t.reset();
    judge();
    t.stop();
    printf("j:");
    t.show();
}

void FallRecognition::judge()
{
    for (int i = 0; i < detector_num_; ++i)
    {
        int up_x, up_y, down_x, down_y;
        int result = bind_id_->getResult(i, up_x, up_y, down_x, down_y);

        if (result == NO_MATCH_ID || result == NONE_STATE || result == INIT_ID)
        {
            states_[i] = NONE_STATE; 
            continue;
        }

        states_[i] = state_judge_[i]->getResult(up_x, up_y, down_x, down_y);
    }
}

void FallRecognition::showResult(Mat img, int wait_key, bool show_all)
{
    if (show_all)
    {
        for (int i = 0; i < states_.size(); ++i)
        {
            if (states_[i] == NONE_STATE)
                continue;
            
            int up_x, up_y, down_x, down_y;
            bind_id_->getResult(i, up_x, up_y, down_x, down_y);
            char score[8];
            sprintf(score, "ID:%d", i);
            putText(img, score, Point(up_x, up_y), FONT_HERSHEY_COMPLEX, 0.6, Scalar(255, 255, 255));

            switch (states_[i])
            {
            case FALL_DOWN:
                rectangle(img,  Point(up_x, up_y),  Point(down_x, down_y), Scalar(0, 0, 255));
                break;
            
            case FALL_ALARM:
                rectangle(img,  Point(up_x, up_y),  Point(down_x, down_y), Scalar(0, 255, 255));
                break;
            
            default:
                rectangle(img,  Point(up_x, up_y),  Point(down_x, down_y), Scalar(0, 255, 0));
                break;
            }
        }
    }
    else
    {
        if (states_[0] != NONE_STATE)
        {
            int up_x, up_y, down_x, down_y;
            bind_id_->getResult(0, up_x, up_y, down_x, down_y);
            char score[8];
            sprintf(score, "ID:%d", 0);
            putText(img, score, Point(up_x, up_y), FONT_HERSHEY_COMPLEX, 0.6, Scalar(255, 255, 255));

            switch (states_[0])
            {
            case FALL_DOWN:
                rectangle(img,  Point(up_x, up_y),  Point(down_x, down_y), Scalar(0, 0, 255));
                break;
            
            case FALL_ALARM:
                rectangle(img,  Point(up_x, up_y),  Point(down_x, down_y), Scalar(0, 255, 255));
                break;
            
            default:
                rectangle(img,  Point(up_x, up_y),  Point(down_x, down_y), Scalar(0, 255, 0));
                break;
            }
        }
    }
    
    imshow("FallRecognition Result", img);
    cvWaitKey(wait_key);
}

void FallRecognition::wrapInputLayer(std::vector<Mat>* input_channels)
{
    Blob<float>* input_layer = net_->input_blobs()[0];

    int width = input_layer->width();
    int height = input_layer->height();
    float* input_data = input_layer->mutable_cpu_data();

    for (int i = 0; i < input_layer->channels(); ++i) 
    {
        Mat channel(height, width, CV_32FC1, input_data);
        input_channels->push_back(channel);
        input_data += width * height;
    }
}

void FallRecognition::preProcess(const Mat& img, std::vector<Mat>* input_channels)
{
    Mat sample;
    if (img.channels() == 3 && num_channels_ == 1)
        cvtColor(img, sample, COLOR_BGR2GRAY);
    else if (img.channels() == 4 && num_channels_ == 1)
        cvtColor(img, sample, COLOR_BGRA2GRAY);
    else if (img.channels() == 4 && num_channels_ == 3)
        cvtColor(img, sample, COLOR_BGRA2BGR);
    else if (img.channels() == 1 && num_channels_ == 3)
        cvtColor(img, sample, COLOR_GRAY2BGR);
    else
        sample = img.clone();

    Mat sample_resized;
    if (sample.size() != input_size_)
        resize(sample, sample_resized, input_size_);
    else
        sample_resized = sample;

    Mat sample_float;
    if (num_channels_ == 3)
        sample_resized.convertTo(sample_float, CV_32FC3, 1/mean_value_, -1);
    else
        sample_resized.convertTo(sample_float, CV_32FC1, 1/mean_value_, -1);
    
    split(sample_float, *input_channels);  
}