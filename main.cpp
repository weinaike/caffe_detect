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
Mat CutImage(Mat src)
{
    Rect ROI = Rect(560,0,720,960);
    return src(ROI);
}

void class_one(VideoCapture & cap)
{
    string path="/home/joyoung/qtwork/caffe_detect/soymilk_last_google/";
    string model_file=path+"deploy.prototxt";
    string mean_file=path+"mean.binaryproto";
    string trained_file=path+"snapshot_iter_3000.caffemodel";
    string label_file=path+"labels.txt";
    Classifier classifier(model_file, trained_file, mean_file, label_file);
    while(1)
    {
        Mat src,dst;
        double t=(double)getTickCount();
        for(int i = 0; i < 10; i++){
            Mat temp;
            cap>>src;
            prep_treat2(src,temp);
            if (i ==0 )
                dst = temp*0.1;
            else
                dst += temp*0.1;
        }
        std::vector<Prediction> predictions = classifier.Classify(dst,1);
        t=((double)getTickCount()-t)/getTickFrequency();
        classifier.draw_show();
    }

}


void det_one(VideoCapture & cap) {
    const string& model_file = "/home/joyoung/caffe/models/ResNet/Joyjzlp-small/SSD_300x300_50/deploy.prototxt";
    const string& weights_file = "/home/joyoung/caffe/models/ResNet/Joyjzlp-small/SSD_300x300_50/ResNet_Joyjzlp-small_50_iter_30000.caffemodel";
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
        img = CutImage(src);
        std::vector<vector<float> > detections = detector.Detect(img);
        t=((double)getTickCount()-t)/getTickFrequency();
        detector.draw_show();
    }
}

//void test_xy()
//{
//    char parent[] = "/home/joyoung/py-faster-rcnn/data/JoyData/Joyjzlp/JPEGImages/";
//    char annopath[] = "/home/joyoung/py-faster-rcnn/data/JoyData/Joyjzlp/Annotations/";
//    string outimage = "/home/joyoung/py-faster-rcnn/data/JoyData/Joyjzlp-small/JPEGImages/";
//    string outanno = "/home/joyoung/py-faster-rcnn/data/JoyData/Joyjzlp-small/Annotations/";
//    DIR *pdir = opendir(parent);
//    struct dirent *fileinfo = NULL;
//    int num;
//    int offset[4] ={560,0,560,0};
//    while( NULL != (fileinfo = readdir(pdir)))
//    {
//        vector<object> gtobj;
//        string imagename = fileinfo->d_name;
//        if (GetExt(imagename) == "jpg")
//        {
//            string fullname = parent + imagename ;
//            string resultimage = outimage + imagename;
//            Mat src = imread(fullname);
//            Mat dst = CutImage(src);
//            imwrite(resultimage,dst);
//            gtobj = ObjectFromFile(annopath, GetName(imagename), "txt");
//            ofstream of;
//            string resultanno = outanno + GetName(imagename) + ".txt" ;
//            of.open(resultanno);
//            of<<gtobj.size()<<endl;
//            for (unsigned i = 0; i < gtobj.size(); i++)
//            {
//                for(int j = 0; j < 4; j++)
//                {
//                    of<<gtobj[i].rect[j] - offset[j]<<" ";
//                }
//                of<<gtobj[i].objName<<endl;
//            }
//            of.close();
//        }
//        cout<<++num<<endl;
//    }
//}



int main()
{
    VideoCapture cap(0);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,960);
    cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    if(!cap.isOpened())
    {
        cout<<"camera is not open\n";
        return(1);
    }
    // det_one(cap);
    class_one(cap);
    // test_xy();

    return 0;
}


void prep_treat(Mat & src, Mat & dst)
{
    Size m_size=src.size();
    //GaussianBlur(src,src,Size(5,5),1.5);
    int len=0;
    if(m_size.height>m_size.width)
        len=m_size.width;
    else
        len=m_size.height;
    len = 720 ;
    int left,top;
    left=(m_size.width-len)/2;
    top=(m_size.height-len)/2;
    Rect m_rect;
    m_rect=Rect(left,top,len,len);
    Mat temp=src(m_rect);
    cv::resize(temp,temp,Size(256,256));
    Rect ROI=Rect(6,6,224,224);
    dst=temp(ROI);
}

void prep_treat2(Mat & src, Mat & dst)
{
    Size m_size=src.size();
    //GaussianBlur(src,src,Size(5,5),1.5);
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
