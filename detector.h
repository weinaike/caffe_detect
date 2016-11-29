#ifndef DETECTOR_H
#define DETECTOR_H
//#define CPU_ONLY
#include <caffe/caffe.hpp>
#include <opencv.hpp>
#include <string>
#include <utility>
#include <vector>
#include <caffe/proto/caffe.pb.h>
using namespace std;
using namespace caffe;  // NOLINT(build/namespaces)


class Detector {
public:
    Detector(const string& model_file,
             const string& weights_file,
             const string& mean_file,
             const string& mean_value,
             const string& labels_file);
    vector<vector<float> > Detect(const cv::Mat& img);
    void draw_show();
private:
    void WrapInputLayer(std::vector<cv::Mat>* input_channels);
    void Preprocess(const cv::Mat& img, std::vector<cv::Mat>* input_channels);
    void SetMean(const string& mean_file, const string& mean_value);
private:
    caffe::shared_ptr<Net<float> > net_;
    cv::Size input_geometry_;
    int num_channels_;
    cv::Mat mean_;
    cv::Mat image_input, image_output;
    std::vector<vector<float> > detections;
    vector<string> labels;
    const float confidence_threshold = 0.3;
};
#endif
