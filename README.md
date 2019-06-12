# fall_recognition  
采用Caffe实现人识别，并实现多目标摔倒识别。

# 使用
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