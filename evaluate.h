#ifndef EVALUATE_H
#define EVALUATE_H
#include <json/json.h>
#include <string>
#include <vector>
using namespace std;
struct object
{
    int rect[4];
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
    int calcresult();
    int printresult();
private:
    float CalcAera(int box1[],int box2[]);
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

};

#endif // EVALUATE_H
