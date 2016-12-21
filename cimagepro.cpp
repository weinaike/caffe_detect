#include "cimagepro.h"
#include "dirent.h"
#include <fstream>
#include <json/json.h>

cimagepro::cimagepro(string & srcimgpath, string & srcannopath,
          string & dstimgpath, string & dstannopath)
{
    parent = srcimgpath;
    annopath = srcannopath;
    outimage = dstimgpath;
    outanno = dstannopath;
}

cimagepro::cimagepro(string & srcimgpath)
{
    parent = srcimgpath;
}

void cimagepro::show(const cv::Rect & rect)
{
    DIR *pdir = opendir(parent.c_str());
    struct dirent *fileinfo = NULL;
    while( NULL != (fileinfo = readdir(pdir)))
    {
        string filedir = fileinfo->d_name;
        if ((filedir != ".")&&(filedir != "..") )
        {
            string subfile = parent + filedir+"/";
            DIR *imagedir = opendir(subfile.c_str());
            struct dirent *imagefile = NULL;

            while(NULL != (imagefile = readdir(imagedir)))
            {
                string imagename = imagefile->d_name;
                if (GetExt(imagename) == "jpg")
                {
                    Mat img = imread(subfile+imagename);
                    cout<<subfile+imagename<<endl;                    
                    Mat dst = img(rect);
                    imshow("test",dst);
                    imwrite(subfile+"720_"+imagename,dst);
                    waitKey(1);
                }
            }

        }
    }

}

string cimagepro::GetExt(string filename)
{
    string::size_type pos;
    pos = filename.find('.');
    string ext = filename.substr(pos+1);
    return ext;
}

string cimagepro::GetName(string filename)
{
    string::size_type pos;
    pos = filename.find('.');
    string name = filename.substr(0,pos);
    return name;
}

void cimagepro::imagecut()
{

    DIR *pdir = opendir(parent.c_str());
    struct dirent *fileinfo = NULL;
    int num = 0;
    int offset[4] ={560,0,560,0};
    while( NULL != (fileinfo = readdir(pdir)))
    {
        vector<object> gtobj;
        string imagename = fileinfo->d_name;
        cout<<imagename<<endl;
        if (GetExt(imagename) == "jpg")
        {
            string fullname = parent + imagename ;
            string resultimage = outimage + imagename;
            cv::Mat src = cv::imread(fullname);
            cv::Mat dst = src(cv::Rect(560,0,720,960));
            imwrite(resultimage,dst);
            gtobj = ObjectFromFile(annopath, GetName(imagename), "txt");
            ofstream of;
            string resultanno = outanno + GetName(imagename) + ".txt" ;
            of.open(resultanno);
            of<<gtobj.size()<<endl;
            for (unsigned i = 0; i < gtobj.size(); i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    of<<gtobj[i].rect[j] - offset[j]<<" ";
                }
                of<<gtobj[i].objName<<endl;
            }
            of.close();
        }
        cout<<++num<<endl;
    }
}
// process JZLP sencond images
void cimagepro::process_jzlp()
{

    DIR *pdir = opendir(parent.c_str());
    struct dirent *fileinfo = NULL;
    int num = 0;
    int offset[4] ={560,0,560,0};
    while( NULL != (fileinfo = readdir(pdir)))
    {
        vector<object> gtobj;
        string imagename = fileinfo->d_name;
//        cout<<imagename<<endl;
        if (GetExt(imagename) == "jpg")
        {
            string fullname = parent + imagename ;
            string resultimage = outimage + imagename;
            cv::Mat src = cv::imread(fullname);
            Mat temp,temp1;
            transpose(src,temp);
            flip(temp,temp1,1);
            cout<<temp.size().width<<endl;
            cv::Mat dst = temp1(cv::Rect(560,0,720,960));
            imwrite(resultimage,dst);
//            imshow("test",dst);
//            waitKey(0);
            imwrite(resultimage,dst);
            gtobj = ObjectFromFile(annopath, GetName(imagename), "txt");
            ofstream of;
            string resultanno = outanno + GetName(imagename) + ".txt" ;
            of.open(resultanno);
            of<<gtobj.size()<<endl;
            for (unsigned i = 0; i < gtobj.size(); i++)
            {
//                for(int j = 0; j < 4; j++)
//                {
//                    of<<gtobj[i].rect[j] - offset[j]<<" ";
//                }
                of<<1280 - gtobj[i].rect[3] - offset[0]<<" ";
                of<<gtobj[i].rect[0] - offset[1]<<" ";
                of<<1280 - gtobj[i].rect[1] - offset[2]<<" ";
                of<<gtobj[i].rect[2] - offset[3]<<" ";
                of<<gtobj[i].objName<<endl;
            }
            of.close();
        }
        cout<<++num<<endl;
    }
}



vector<object> cimagepro::ObjectFromFile(const string & path,const string & filename,const string & ext)
{
    vector<object> objset;
    string filepath = path + filename + '.' + ext;
    ifstream ifs(filepath);
    if(ifs.is_open())
    {
        if(ext == "txt")
        {
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
                        obj.objName = buffer.substr(0,pos);
                    }
                    else
                    {
                        pos = buffer.find(' ');
                        obj.rect[i] = stoi(buffer.substr(0,pos));
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
                    obj.rect[0] = int (left * 1280) ;
                    obj.rect[1] = int (top * 720) ;
                    obj.rect[2] = int (right * 1280) ;
                    obj.rect[3] = int (bottom * 720) ;

                    map <string, string>::iterator itr;
                    itr = wordsmap.find(name);
                    obj.objName = itr->second;
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
