#ifndef HIKVISIONSDK_H
#define HIKVISIONSDK_H
#include"MvCameraControl.h"
#include"opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include"string"


using namespace std;
//using namespace cv;

class HikvisionSDK
{
public:

    int SetExposureTime(float m_dExposureEdit);
    int SetFloatValue(IN const char* strKey, IN float fValue);
    int SetEnumValue(IN const char* strKey, IN unsigned int nValue);
    int  GetIntValue(IN const char* strKey, OUT unsigned int *pnValue);
    // ch:连接设备
    int Find_Device();
    int connectCamera(int id,void* m_hDevHandle);
    int connectCamera(int id);
    //设置是否为触发模式
    int setTriggerMode(unsigned int TriggerModeNum);

    //开启相机采集
    int startCamera(void* m_hDevHandle);

    //关闭相机采集
    int stopCamera(void* m_hDevHandle);

    //关闭相机
    int closeCamera(void* m_hDevHandle);

    //软触发
    int softTrigger();

    //读取buffer
    int ReadBuffer(cv::Mat &image);

    //设置图像高度
    int setHeight(unsigned int height);

    //设置图像ROI宽度
    int setWidth(unsigned int width);

    //获取图像最大高度值
    int getHeight();

    //获取图像最大宽度值
    int getWidth();

    //获取相机曝光时间
    float getExposureTime();

    //设置图像水平偏移OffsetX
    int setOffsetX(unsigned int offsetX);

    //设置图像竖直偏移OffsetY
    int setOffsetY(unsigned int offsetY);

    //设置触发源
    int setTriggerSource(unsigned int TriggerSourceNum);

    //设置帧率控制使能
    int setFrameRateEnable(bool comm);

    //设置心跳时间
    int setHeartBeatTime(unsigned int time);

    //设置曝光时间
    int setExposureTime(float ExposureTimeNum);

    //保存图片
     cv::Mat saveImage(void* handel);

    //清理相机缓存
    void clearBuffer();
    cv::Mat saveImage7(int id);
    cv::Mat saveImage(void* handel, int id);

private:
      HikvisionSDK();
     ~HikvisionSDK();
      unsigned int nTLayerType = MV_GIGE_DEVICE | MV_USB_DEVICE;
      int nRet;
      static HikvisionSDK* m_pInstance;




      unsigned int nDataLen = 0;


public:
     void*               m_hDevHandle_1;
     void*               m_hDevHandle_2;
     void*               m_hDevHandle_3;
     void*               m_hDevHandle_4;
     void*               m_hDevHandle_5;
     void*               m_hDevHandle_6;
     void*               m_hDevHandle;
    //unsigned char*  m_pBufForSaveImage;         // 用于保存图像的缓存
    static  int id1;


    //unsigned char*  m_pBufForDriver;            // 用于从驱动获取图像的缓存
    unsigned int    m_nBufSizeForDriver;
     MV_CC_DEVICE_INFO_LIST m_stDevList = {0};
     static HikvisionSDK* getInstance()
            {
                   if( m_pInstance == NULL)
                         { m_pInstance= new HikvisionSDK();}

                    return m_pInstance;
            }
      cv::Mat getImage1;
      cv::Mat getImage2;
      cv::Mat getImage3;
      cv::Mat getImage4;
      cv::Mat getImage5;
      cv::Mat getImage6;


};

#endif // HIKVISIONSDK_H
