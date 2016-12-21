#ifndef CIMAGEPRO_H
#define CIMAGEPRO_H
#include <string>
#include <opencv.hpp>
using namespace std;
using namespace cv;
struct object
{
    int rect[4];
    string objName ;
};
// pretreat images
class cimagepro
{
public:
    cimagepro(string & srcimgpath, string & srcannopath,
              string & dstimgpath, string & dstannopath);
    cimagepro(string & srcimgpath);
    void imagecut();
    void process_jzlp();
    vector<object> ObjectFromFile(const string & path,
                                  const string & filename,
                                  const string & ext);
    string GetExt(string filename);
    string GetName(string filename);
    void show(const cv::Rect & rect);
private:
    string parent ;
    string annopath ;
    string outimage ;
    string outanno ;
    map <string, string> wordsmap;
};



#endif // CIMAGEPRO_H
