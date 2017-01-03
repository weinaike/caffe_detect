#include "evaluate.h"
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <iomanip>
evaluate::evaluate(const string & groudtruthpath,
                   const string & ext1,
                   const string & predictpath,
                   const string & ext2,
                   const string & labelfile)
{
    gtpath = groudtruthpath;
    ptpath = predictpath;
    gtext  = ext1;
    ptext  = ext2;
    wordsfile = labelfile;
    joinMapFlag = 0;
    // load labels & create map of result
    ifstream fs(wordsfile);
    if(fs.is_open()){
        std::cout<<"label is opened"<<endl;
        string buffer;
        // gt num (0) | pt num (1) | pt right (2) | gt right (3) | error num (4)
        while(getline(fs,buffer))
        {
            labels.push_back(buffer);
            vector<int> result;
            result.push_back(0);
            result.push_back(0);
            result.push_back(0);
            result.push_back(0);
            result.push_back(0);
            resultmap.insert(pair<string,vector<int>>(buffer,result));
        }
    }
    else
    {
        cout<<"labels file is not exist"<<endl;
    }
    fs.close();

}

evaluate::evaluate()
{


}

int evaluate::setLabelMap(const string & labelmap)
{
    joinMapFlag = 1;
    ifstream fs(labelmap);
    if(fs.is_open()){
        std::cout<<"labelMap is opened"<<endl;
        string buffer;
        while(getline(fs,buffer))
        {
            string::size_type pos;
            pos = buffer.find('\t');
            string name1 = buffer.substr(0,pos);
            string name2 = buffer.substr(pos+1);
            wordsmap.insert(pair<string,string>(name1,name2));
        }
    }
    else
    {
        cout<<"labels file is not exist"<<endl;
    }
    fs.close();
    return 1;
}

float evaluate::CalcAera(float box1[],float box2[])
{
    float s_i,s_j,xmin,ymin,xmax,ymax,w,h,inters,overlap;
    s_i=(box1[2]-box1[0])*(box1[3]-box1[1]);
    s_j=(box2[2]-box2[0])*(box2[3]-box2[1]);
    xmin = max(box1[0], box2[0]);
    ymin = max(box1[1], box2[1]);
    xmax = min(box1[2], box2[2]);
    ymax = min(box1[3], box2[3]);
    w = max(xmax - xmin, (float)0.0);
    h = max(ymax - ymin, (float)0.0);
    inters = w * h;
    overlap=inters/(s_i+s_j-inters);
    return overlap;

}

bool evaluate::IsOverlap(object obj1,object obj2)
{
    if((obj1.objName == obj2.objName)&&(CalcAera(obj1.rect,obj2.rect)>=0.5))
    {
        return true;
    }
    else
    {
        return false;
    }
}

vector<object> evaluate::ObjectFromFile(const string & path,const string & filename,const string & ext)
{
    vector<object> objset;
    string filepath = path + filename + "." + ext;
    string imagefile = path + filename + ".jpg";
    ifstream ifs(filepath);
    if(ifs.is_open())
    {
        if(ext == "txt")
        {
            cv::Mat img = cv::imread(imagefile);
            float height = img.size().height;
            float width =img.size().width;
            string buffer;
            getline(ifs,buffer);
            int num = stoi(buffer);
            while(getline(ifs,buffer))
            {
                object obj;
                string::size_type pos;
                for(int i = 0; i < 5; i++)
                {
                    if(i == 4)
                    {
                        pos = buffer.find('\r');
                        if(pos==string::npos)
                        {
                            pos = buffer.find(' ');
                        }
                        string name = buffer.substr(0,pos);

                        if (joinMapFlag)
                        {
                            map <string, string>::iterator itr;
                            itr = wordsmap.find(name);
                            obj.objName = itr->second;
                        }
                        else
                        {
                            obj.objName = name;
                        }
                    }
                    else
                    {
                        pos = buffer.find(' ');
                        int temp = stoi(buffer.substr(0,pos));
                        if(i == 0 || i ==2)
                            obj.rect[i] = temp/width;
                        if(i == 1 || i == 3)
                            obj.rect[i] = temp/height;
                        buffer = buffer.substr(pos+1);
                    }
                }
                objset.push_back(obj);
            }
        }
        else if(ext == "json")
        {
            Json::Reader reader;
            Json::Value root;
            if(reader.parse(ifs,root))
            {
                //读取根节点信息
                Json::Value subroot = root["objects"];
                int num = root["number"].asInt();
                //读取子节点信息
                for (int i = 0; i < num; i++)
                {
                    object obj;
                    string name = subroot[i]["name"].asString();
                    float bottom = subroot[i]["bottom"].asFloat();
                    float left = subroot[i]["left"].asFloat();
                    float right = subroot[i]["right"].asFloat();
                    float top = subroot[i]["top"].asFloat();
                    obj.rect[0] = left; //int (left * 1280) ;
                    obj.rect[1] = top; //int (top * 720) ;
                    obj.rect[2] = right; //int (right * 1280) ;
                    obj.rect[3] = bottom; //int (bottom * 720) ;
                    if (joinMapFlag)
                    {
                        map <string, string>::iterator itr;
                        itr = wordsmap.find(name);
                        obj.objName = itr->second;
                    }
                    else
                    {
                        obj.objName = name;
                    }
                    objset.push_back(obj);
                }
            }
            ifs.close();
        }
        else
        {
            cout<<"unknown file type"<<endl;

        }
    }
    else
    {
        cout<<filepath<<" is not exist"<<endl;

    }
    return objset;

}


string evaluate::GetExt(string filename)
{
    string::size_type pos;
    pos = filename.find('.');
    string ext = filename.substr(pos+1);
    return ext;
}

string evaluate::GetName(string filename)
{
    string::size_type pos;
    pos = filename.find('.');
    string name = filename.substr(0,pos);
    return name;
}

int evaluate::calcresult()
{
    const string dir = gtpath;
    DIR *gtdir = opendir(dir.c_str());
    struct dirent *fileinfo = NULL;
    int numFile = 0;
    while( NULL != (fileinfo = readdir(gtdir)))
    {
        vector<object> gtobjs,ptobjs;
        string imagename = fileinfo->d_name;
        if (GetExt(imagename) == gtext)
        {
            string filename = GetName(imagename);
            // load groundtruth objects from file
            gtobjs = ObjectFromFile(gtpath, filename, gtext);
            // load predict objects from file
            ptobjs = ObjectFromFile(ptpath, filename, ptext);

            // print  result
            cout<<"~~~"<<numFile<<"~~~"<<imagename<<"~~~"<<endl;
            cout<<"~~~ground truth is "<< gtobjs.size()<<"~~~"<<endl;
            for (unsigned i = 0; i < gtobjs.size(); i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    cout<<setprecision(3)<<gtobjs[i].rect[j]<<" ";
                }
                cout<<gtobjs[i].objName<<endl;
            }

            cout<<"~~~detected num is "<<ptobjs.size()<<"~~~"<<endl;
            for (unsigned i = 0; i < ptobjs.size(); i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    cout<<setprecision(3)<<ptobjs[i].rect[j]<<" ";
                }
                cout<<ptobjs[i].objName<<endl;
            }


            int numMatch = 0;
            for (unsigned i = 0; i < gtobjs.size(); i++)
            {
                int flag = 0;
                map <string,vector<int>>::iterator gtitr;
                gtitr = resultmap.find(gtobjs[i].objName);
                gtitr->second[0]++;
                for (unsigned j = 0; j < ptobjs.size(); j++)
                {
                    map <string,vector<int>>::iterator ptitr;
                    if(i == 0)
                    {
                        ptitr = resultmap.find(ptobjs[j].objName);
                        ptitr->second[1]++;

                        // count the error of predicting, and print it
                        bool errorObj = true;
                        for (unsigned k = 0; k < gtobjs.size(); k++)
                        {
                            if (ptobjs[j].objName == gtobjs[k].objName )
                                errorObj = false;
                        }
                        if (errorObj)
                        {
                            ptitr = resultmap.find(ptobjs[j].objName);
                            ptitr->second[4]++;
                            cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<filename<<" "<<ptobjs[j].objName<<endl;
                        }
                    }
                    if(IsOverlap(gtobjs[i],ptobjs[j]))
                    {
                        // count the same object and overlap > 0.5
                        ptitr = resultmap.find(gtobjs[i].objName);
                        ptitr->second[2]++;
                        if(flag == 0)
                        {
                            numMatch++;
                            ptitr->second[3]++;
                            flag = 1;
                        }

                    }
                }
            }
            cout<<"~~~right result is "<<numMatch<<"~~~"<<endl;
        }
    }
    closedir(gtdir);
    printresult();
    return 0;

}

int evaluate::printresult()
{
    cout<<"~~~~~~~~~~~~~~~~~all result~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    cout<<"class | "<<"gt num (A) | "<<"pt num (B) | "<<" pt right (C) | "
       <<"gt right (D) | "<<" error "<< "recall (D/A) | "<<"precise (C/B)"<<endl;
    map <string, vector<int>>::iterator itr;
    float recall_all = 0;
    float precise_all = 0;
    int num_clac = 0;
    for(itr = resultmap.begin(); itr!=resultmap.end(); itr++)
    {
        cout<<itr->first<<" ";
        for(int i = 0; i < 5; i++)
        {
            cout<<itr->second[i]<<" ";
        }

        if(itr->second[1] != 0)
        {
            recall_all += float(itr->second[3])/itr->second[0];
            precise_all += float(itr->second[2])/itr->second[1];
            cout<<float(itr->second[3])/itr->second[0]<<" ";
            cout<<float(itr->second[2])/itr->second[1]<<" ";
            cout<<endl;
            num_clac++;
        }
        else
        {
            cout<<endl;
        }
    }
    cout<<"calc num "<<num_clac<<endl;
    cout<<"average recall: "<<recall_all/num_clac<<endl;
    cout<<"average precise: "<<precise_all/num_clac<<endl;
    return 0;
}

