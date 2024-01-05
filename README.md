# squeezenet-ncnn-pnnx-opencv-cpp

## 简介

使用 pnnx 工具， 转换 PyTorch 模型到 ncnn 格式。然后用 opencv 读取图像和imshow， 用 ncnn 做推理， 执行图像分类，输出类别和置信度。

你可以基于这个工程， 直接(或者很少修改)放到 Android JNI / 车载开发板上部署。

![](output.png)

## 安装 - Python 相关

安装 Python, 例如通过 miniconda 安装。

安装 torch, pnnx 等 python 包:

```bash
pip install numpy matplotlib
pip install -U torch torchvision
pip install -U pnnx
```

## 安装 - C++ 相关

![](cpp_packages.png)

下载 ncnn: https://github.com/Tencent/ncnn/releases

我用的动态库, 解压到 `D:\artifacts\ncnn\ncnn-20240102-windows-vs2022-shared` 目录

下载 opencv: https://github.com/opencv/opencv/releases/

解压到 `D:\artifacts\opencv\4.9.0\windows` 目录

## 转模型

```powershell
python convertmodel.py
```

得到很多文件，我们只用到 `squeezenet_v1.1.ncnn.param` 和 `squeezenet_v1.1.ncnn.bin`.

## C++ 推理

- `detect_squeezenet_pnnx()`: PyTorch 官方仓库中 squeezenet1_1 的推理
- `detect_squeezenet_caffe()`: ncnn 仓库自带的的模型的推理，也就是 caffemodel

两个模型的输入、输出处理差异，见 `detect_squeezenet_pnnx()` 函数:
```cpp
// There are 5 differences between squeezenet_v1.1.param and squeezenet_v1.1.ncnn.param
static int detect_squeezenet_pnnx(const cv::Mat& bgr, std::vector<float>& cls_scores)
{
    ncnn::Net net;
    net.opt.use_vulkan_compute = false;

    // difference 1: model file name contains "ncnn"
    // torchvision.models.squeezenet1_1
    if (net.load_param("squeezenet_v1.1.ncnn.param"))
        exit(-1);
    if (net.load_model("squeezenet_v1.1.ncnn.bin"))
        exit(-1);

    // difference 2: network requires RGB, not BGR. i.e. PIXEL_BGR2RGB instead of PIXEL_BGR
    cv::Mat img;
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR2RGB, bgr.cols, bgr.rows, 227, 227);
    const float mean_vals[3] = { 104.f, 117.f, 123.f };
    in.substract_mean_normalize(mean_vals, 0);

    ncnn::Extractor ex = net.create_extractor();

    // difference 3: input blob name is in0, not data
    ex.input("in0", in);
    //ex.input("data", in);

    ncnn::Mat out;
    // difference 4: output blob name is out0, not prob
    ex.extract("out0", out);
    //ex.extract("prob", out);

    // difference 5: manually call softmax on the fc output.
    // manually call softmax on the fc output
    // convert result into probability
    // skip if your model already has softmax operation
    if(1)
    {
        ncnn::Layer* softmax = ncnn::create_layer("Softmax");

        ncnn::ParamDict pd;
        softmax->load_param(pd);

        softmax->forward_inplace(out, net.opt);

        delete softmax;
    }

    cls_scores.resize(out.w);
    for (int j = 0; j < out.w; j++)
    {
        cls_scores[j] = out[j];
    }

    return 0;
}
```

## Debug

VS 调试 C++ 代码，需要另行配置 Debug 版本的 ncnn, 需要自行编译。

## Referenecs

- https://github.com/Tencent/ncnn
- https://github.com/opencv/opencv
