#ifndef EVALUATE_H
#define EVALUATE_H
#include <json/json.h>
#include <string>
#include <vector>
#include <opencv.hpp>


using namespace std;
struct object
{
    float rect[4];
    string objName ;
};

class evaluate
{

public:
    evaluate(const string & groudtruthpath,
             const string & ext1,
             const string & predictpath ,
             const string & ext2,
             const string & labelfile);
    evaluate();
    int calcresult();
    int printresult();
    /*
     * input : label table for similar object
     * set joinMapFlag = 1
     * if used the joinLabel function, call the fun after constructed function;
     */
    int setLabelMap(const string & labelmap);

private:
    float CalcAera(float box1[],float box2[]);
    bool IsOverlap(object obj1,object obj2);
    vector<object> ObjectFromFile(const string & path,
                                  const string & filename,
                                  const string & ext);
    string GetExt(string filename);
    string GetName(string filename);

private:

    map <string, string> wordsmap;
    map <string, vector<int>> resultmap;
    string gtpath;  // groudtruth file path
    string ptpath;  // predict file path
    string gtext;   // gt file extent name
    string ptext;   // pt file extent name
    string wordsfile;
    vector<string> labels;    
    int joinMapFlag; // whether to join label for similar objects

};

#endif // EVALUATE_H
