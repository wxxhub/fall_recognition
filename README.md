# fall_recognition  
采用Caffe实现人识别，并实现多目标摔倒识别。

## 使用
```shell
#先配置OpenCV和Caffe

# 下载项目
git clone git@github.com:wxxhub/fall_recognition.git
cd fall_recognition
mkdir build
cd build
cmake ..
make -j4

## 使用已存在视频识别，由于视频没有上传，请修改视频路径
./demo

## 打开摄像头识别
./usb_cam_demo
```

## API说明
```cpp
# 是否使用GPU
void setUseGPU(bool open);

# 摔倒判断程序将根据帧率调节拟合数据量， 默认10帧
void setFPS(int fps);

# 对图像进行识别并判断
void detector(cv::Mat image);

/*  拟合斜率乘的一个比例。
 *  该比例越小，对摔倒越敏感。
 *  当帧率比较大时， 摔倒出现的斜率变小， 应该适当减小该系数。
 */
void setSlopeRadio(float radio);

/*  设置一个预警时间， 当超出该时间， 人还未站起则状态变为摔倒。
 *  单位是秒
 *  设置预警时间可以避免出现弯腰和下蹲的误判。
 */
void setWarnTime(int second);

/*  将识别结果可视化出来。
 *  wait_key cvWaitKey的时间
 *  show_all是否显示所有目标， 如果为False则只显示目标0
 */
void showResult(cv::Mat img, int wait_key = 10, bool show_all = true);

/*  将没有绑定ID的识别结果可视化出来。
 */
void showNoIdBindResult(cv::Mat img, int wait_key = 10);
```

## 识别加速
> 合并bn层，提高模型运行速度，使用MobileNet-SSD作者的脚本将bn层合并到了卷积层中，相当于bn的计算时间就被节省了，识别时间从110ms下降到64ms左右。使用深度卷积后时间下降至21ms。

## 测试环境
> i5-6300HQ + GTX-950M + Ubantu16 