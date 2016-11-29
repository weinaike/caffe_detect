TEMPLATE = app
#CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    detector.cpp \
    cvxtext.cpp \
    jsoncpp.cpp \
    ctrain.cpp \
    classifier.cpp \
    evaluate.cpp

HEADERS += \
    detector.h \
    cvxtext.h \
    json/json-forwards.h \
    json/json.h \
    ctrain.h \
    classifier.h \
    evaluate.h

INCLUDEPATH=/usr/include/\
/usr/include/opencv\
/usr/include/opencv2\
/home/joyoung/caffe/include\
/usr/local/cuda-7.5/include\
/usr/local/include/freetype2 \

LIBS=/usr/lib/x86_64-linux-gnu/libopencv_*.so\
/usr/local/caffe/libcaffe.so\
/usr/lib/x86_64-linux-gnu/libboost_*.so\
/usr/local/cuda-7.5/lib64/*.so\
/usr/lib/x86_64-linux-gnu/libglog.so\
/usr/local/lib/libfreetype.so\
