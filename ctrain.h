#ifndef CTRAIN_H
#define CTRAIN_H
//#define CPU_ONLY
#include <string>
#include <caffe/caffe.hpp>
#include "boost/algorithm/string.hpp"
using namespace std;
using namespace caffe;

class CTrain
{
public:
    CTrain(string solverfile,string weightfile, string snapshotfile);
    CTrain(string solverfile);
    int train();
    void CopyLayers(caffe::Solver<float>* solver, const std::string& model_list);
private:
    string m_solverfile;
    string m_modelfile;
    string m_weightfile;
    string m_snapshotfile;

};

#endif // CTRAIN_H
