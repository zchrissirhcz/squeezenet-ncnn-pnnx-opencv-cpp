// using ncnn C++ API to load and run squeezenet model for image classification

#include "net.h"

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utils/logger.hpp>

static void disable_opencv_videoio_logging()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);
}

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

static int detect_squeezenet_caffe(const cv::Mat& bgr, std::vector<float>& cls_scores)
{
    ncnn::Net net;
    net.opt.use_vulkan_compute = false;

    // the ncnn model https://github.com/nihui/ncnn-assets/tree/master/models
    if (net.load_param("squeezenet_v1.1.param"))
        exit(-1);
    if (net.load_model("squeezenet_v1.1.bin"))
        exit(-1);

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR, bgr.cols, bgr.rows, 227, 227);
    const float mean_vals[3] = { 104.f, 117.f, 123.f };
    in.substract_mean_normalize(mean_vals, 0);

    ncnn::Extractor ex = net.create_extractor();

    ex.input("data", in);

    ncnn::Mat out;
    ex.extract("prob", out);

    cls_scores.resize(out.w);
    for (int j = 0; j < out.w; j++)
    {
        cls_scores[j] = out[j];
    }

    return 0;
}


class ClassMeaning
{
public:
    std::string id;
    std::string meaning;
};

static int print_topk(const std::vector<float>& cls_scores, int topk, const std::vector<ClassMeaning>& class_meaning)
{
    // partial sort topk with index
    int size = cls_scores.size();
    std::vector<std::pair<float, int> > vec;
    vec.resize(size);
    for (int i = 0; i < size; i++)
    {
        vec[i] = std::make_pair(cls_scores[i], i);
    }

    std::partial_sort(vec.begin(), vec.begin() + topk, vec.end(),
                      std::greater<std::pair<float, int> >());

    // print topk and score
    for (int i = 0; i < topk; i++)
    {
        float score = vec[i].first;
        int index = vec[i].second;
        ClassMeaning cm = class_meaning[index];
        fprintf(stdout, "top%d: index=%d, conf=%.3f, id=%s, meaning=%s\n", 
            i + 1,
            index,
            score,
            cm.id.c_str(),
            cm.meaning.c_str()
        );
    }

    return 0;
}

static std::vector<ClassMeaning> read_class_meaing(const char* text_file_path)
{
    std::vector<ClassMeaning> res;
    FILE* fin = fopen(text_file_path, "r");
    if (fin==NULL)
    {
        fprintf(stderr, "failed to open %s\n", text_file_path);
        exit(-1);
    }
    char line[1024];
    void* ret;
    while (true)
    {
        memset(line, 0, sizeof(line));
        ret = fgets(line, sizeof(line), fin);
        if (ret == NULL)
            break;
        if (line[strlen(line)-1]=='\n' || line[strlen(line)-1]=='\r')
        {
            line[strlen(line) - 1] = '\0';
        }
        if (strlen(line)>0)
        {
            if (line[strlen(line)-1]=='\r' || line[strlen(line)-1]=='\n')
            {
                line[strlen(line) - 1] = '\0';
            }
        }
        ClassMeaning cm;
        std::string line_str = line;
        cm.id = line_str.substr(0, 9);
        cm.meaning = line_str.substr(10);
        res.push_back(cm);
    }
    fclose(fin);
    return res;
}

int main(int argc, char** argv)
{
    disable_opencv_videoio_logging();
    // if (argc != 2)
    // {
    //     fprintf(stderr, "Usage: %s [imagepath]\n", argv[0]);
    //     return -1;
    // }

    //const char* imagepath = argv[1];
    //const char* imagepath = "000017.jpg";
    const char* imagepath = "cat.jpg";

    cv::Mat m = cv::imread(imagepath);
    if (m.empty())
    {
        fprintf(stderr, "cv::imread %s failed\n", imagepath);
        return -1;
    }
    const char* class_meaing_txt = "synset_words.txt";
    std::vector<ClassMeaning> class_meaning = read_class_meaing(class_meaing_txt);

    using detect_squeezenet_func = std::function<int(const cv::Mat&, std::vector<float>&)>;
    for (detect_squeezenet_func detect_squeezenet : {detect_squeezenet_caffe, detect_squeezenet_pnnx})
    {
        std::vector<float> cls_scores;
        detect_squeezenet(m, cls_scores);
        print_topk(cls_scores, 3, class_meaning);
        fprintf(stdout, "----------\n");
    }
    cv::imshow("image", m);
    cv::waitKey(0);

    return 0;
}