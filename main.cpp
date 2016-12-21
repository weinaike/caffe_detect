#include <caffe/caffe.hpp>
#include <iostream>
#include <fstream>
#include <opencv.hpp>
#include <vector>
#include <string>
#include "detector.h"
#include "ctrain.h"
#include "classifier.h"
#include "cvxtext.h"
#include "cimagepro.h"

using namespace cv;
using namespace std;
void prep_treat(Mat & src,Mat & dst);
void prep_treat2(Mat & src, Mat & dst);
void train_test()
{
    string solverfile="cifar10_full_solver.prototxt";
    CTrain cifar_train(solverfile);
    cifar_train.train();

}
// process images from jzlp (1280*960)
Mat process_jzlp(Mat src)
{
    Rect ROI = Rect(560,0,720,960);
    return src(ROI);
}


// pretreat image form doujiangji (djj) 1280*960
void pretreat_djj(Mat & src, Mat & dst)
{
//    Size m_size=src.size();
//    GaussianBlur(src,src,Size(5,5),1.5);
    int len= 640;
    int left,top;
    left=320;
    top=320;
    Rect m_rect;
    m_rect=Rect(left,top,len,len);
    Mat temp=src(m_rect);
    cv::resize(temp,temp,Size(256,256));
    Rect ROI=Rect(6,6,224,224);
    dst=temp(ROI);
}
void pretreat_dzc(Mat &src, Mat &dst)
{
    Rect rect = Rect(300,0,720,720);
    Mat temp = src(rect);
    Mat temp2;
    cv::resize(temp,temp2,Size(256,256));
    Rect ROI=Rect(4,4,227,227);
    dst=temp2(ROI);
}



void class_one(VideoCapture & cap)
{
    // resnet for food 120
//    string path="/home/joyoung/qtwork/caffe_detect/food120/";
//    string model_file=path+"resnet.prototxt";
//    string mean_file=path+"resnet_mean.binaryproto";
//    string trained_file=path+"resnet.caffemodel";
//    string label_file=path+"resnet_labels.txt";
    // googlenet for food 107
//    string path="/home/joyoung/qtwork/caffe_detect/food120/";
//    string model_file=path+"googlenet.prototxt";
//    string mean_file=path+"googlenet_mean.binaryproto";
//    string trained_file=path+"googlenet100000.caffemodel";
//    string label_file=path+"googlenet_labels.txt";
    // googlenet for show2016 of cookbook
    string path="/home/joyoung/qtwork/caffe_detect/show2016/";
    string model_file=path+"caffenet.prototxt";
    string mean_file=path+"mean.binaryproto";
    string trained_file=path+"caffenet.caffemodel";
    string label_file=path+"labels.txt";
    Classifier classifier(model_file, trained_file, mean_file, label_file);
    while(1)
    {
        Mat src,dst;
        double t=(double)getTickCount();
        cap>>src;
        pretreat_dzc(src,dst);
        std::vector<Prediction> predictions = classifier.Classify(dst,1);
        t=((double)getTickCount()-t)/getTickFrequency();
        std::cout<<"comsume: "<<t<<"s"<<endl;
        classifier.draw_show();
    }

}


void det_one(VideoCapture & cap) {
    const string& model_file = "/home/joyoung/caffe/models/ResNet/Joyjzlp-small/SSD_300x300_50/resnet.prototxt";
    //const string& model_file = "/home/joyoung/resnet.prototxt";
    const string& weights_file = "/home/joyoung/caffe/models/ResNet/Joyjzlp-small/SSD_300x300_50/resnet.caffemodel";
    const string& labels_file= "/home/joyoung/caffe/data/Joyjzlp-small/labels.txt";
    const string& meanfile = "";
    const string& meanvalue = "104,117,123";    
    // Initialize the network.
    Detector detector(model_file, weights_file, meanfile, meanvalue, labels_file);
    while(1){
        Mat src, img;
        //std::string file="/home/joyoung/py-faster-rcnn/data/JoyData/Joyjzlp/JPEGImages/camera1_2016_11_24_20_19_13.jpg";
        //src = cv::imread(file);
        double t=(double)getTickCount();
        cap>>src;
        img = process_jzlp(src);
        std::vector<vector<float> > detections = detector.Detect(img);
        t=((double)getTickCount()-t)/getTickFrequency();
        detector.draw_show();
    }
}


int main()
{
    VideoCapture cap(0);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);
    cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    if(!cap.isOpened())
    {
        cout<<"camera is not open\n";
        return(1);
    }
    //det_one(cap);
    class_one(cap);
//    string parent = "/home/joyoung/py-faster-rcnn/data/JoyData/Joyjzlp/JZLP/";
//    string annopath = "/home/joyoung/py-faster-rcnn/data/JoyData/Joyjzlp/JZLP/";
//    string outimage = "/home/joyoung/py-faster-rcnn/data/JoyData/Joyjzlp-small/img/";
//    string outanno = "/home/joyoung/py-faster-rcnn/data/JoyData/Joyjzlp-small/anno/";
//    cimagepro deal(parent,annopath,outimage,outanno);
//    deal.process_jzlp();


//    string parent = "/home/joyoung/digits/data/show2016_dealed/new/";
//    cimagepro test(parent);
//    test.show(cv::Rect(300,0,720,720));
//    Mat src = imread("/home/joyoung/Desktop/a.png");
//    Mat dst;
//    resize(src,dst,Size(300,300));
//    cv::cvtColor(dst,dst,CV_RGB2BGR);
//    imshow("dst",dst);
//    waitKey(0);
    return 0;
}
