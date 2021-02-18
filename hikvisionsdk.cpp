#include "hikvisionsdk.h"
#include<QDebug>
#include<windows.h>
//#include "MvCameraControl.h"
//#include"MyCamera.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>


MV_CC_DEVICE_INFO_LIST m_stDevList;         // ch:设备信息列表结构体变量，用来存储设备列表
MV_CC_DEVICE_INFO* m_Device=NULL;                 //设备对象

HikvisionSDK::HikvisionSDK()
{
    m_hDevHandle_1    = NULL;
}

HikvisionSDK::~HikvisionSDK()
{
    if (m_hDevHandle)
    {
        MV_CC_DestroyHandle(m_hDevHandle);
        m_hDevHandle    = NULL;
    }
    if (m_hDevHandle_1)
    {
        MV_CC_DestroyHandle(m_hDevHandle_1);
        m_hDevHandle_1    = NULL;
    }
}

int  HikvisionSDK::SetEnumValue(IN const char* strKey, IN unsigned int nValue)
{
    if (NULL == strKey)
    {
        return MV_E_PARAMETER;
    }

    return MV_CC_SetEnumValue(m_hDevHandle, strKey, nValue);
}

int  HikvisionSDK::SetFloatValue(IN const char* strKey, IN float fValue)
{
    if (NULL == strKey)
    {
        return MV_E_PARAMETER;
    }

    return MV_CC_SetFloatValue(m_hDevHandle, strKey, fValue);
}

int  HikvisionSDK::SetExposureTime(float m_dExposureEdit)
{
    // ch:调节这两个曝光模式，才能让曝光时间生效
    // en:Adjust these two exposure mode to allow exposure time effective
    int nRet_EnumValue = SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
    if (MV_OK != nRet_EnumValue)
    {
        return nRet_EnumValue;
    }

    int nRet_EnumValue1 = SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);

    int nRet_FloatValue = SetFloatValue("ExposureTime", m_dExposureEdit);
    if (MV_OK != nRet_FloatValue)
    {
        return nRet_FloatValue;
    }

    return MV_OK;
}

int   HikvisionSDK::GetIntValue(IN const char* strKey, OUT unsigned int *pnValue)
{
    if (NULL == strKey || NULL == pnValue)
    {
        return MV_E_PARAMETER;
    }

    MVCC_INTVALUE stParam;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    int nRet = MV_CC_GetIntValue(m_hDevHandle, strKey, &stParam);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    *pnValue = stParam.nCurValue;

    return MV_OK;
}
//查找设备列表
int HikvisionSDK::Find_Device()
{

     nRet = MV_CC_EnumDevices(nTLayerType, &m_stDevList);
     return  0;

}
//连接相机
int HikvisionSDK::connectCamera(int id,void* m_hDevHandle)
{

   // unsigned int nTLayerType = MV_GIGE_DEVICE | MV_USB_DEVICE;
   // MV_CC_DEVICE_INFO_LIST m_stDevList = {0};
    //int nRet = MV_CC_EnumDevices(nTLayerType, &m_stDevList);
   // int nDeviceIndex = 0;
    MV_CC_DEVICE_INFO m_stDevInfo = {0};
    memcpy(&m_stDevInfo, m_stDevList.pDeviceInfo[id], sizeof(MV_CC_DEVICE_INFO));

    nRet = MV_CC_CreateHandle(&m_hDevHandle, &m_stDevInfo);
    unsigned int nAccessMode = MV_ACCESS_Exclusive;
    unsigned short nSwitchoverKey = 0;
    nRet = MV_CC_OpenDevice(m_hDevHandle, nAccessMode, nSwitchoverKey);
 //   unsigned int nNum = 15;
 //   nRet = MV_CC_SetImageNodeNum(m_handle, nNum);
 //   nRet = MV_CC_RegisterImageCallBack(m_handle, ImageCallBack, NULL);
    if (MV_OK == nRet)
        {

//            qDebug()<<"打开设备成功"<<endl;
//            switch(id)
//            {
//               case 0:  m_hDevHandle_1=m_hDevHandle;break;
//               case 1:  m_hDevHandle_2=m_hDevHandle;break;
//               case 2:  m_hDevHandle_3=m_hDevHandle;break;
//               case 3:  m_hDevHandle_4=m_hDevHandle;break;
//               case 4:  m_hDevHandle_5=m_hDevHandle;break;
//               case 5:  m_hDevHandle_6=m_hDevHandle;break;
//            }
            return 0;

        }
    else {
          return -1;
    }
}
/*====================================================================
* 类型：connectCamera
* 功能：根据序列号对相机进行连接
* 输入：相机ID号
* 输出：
* 其他：
======================================================================*/
int HikvisionSDK::connectCamera(int id)
{
    void* m_hDevHandle;
    // unsigned int nTLayerType = MV_GIGE_DEVICE | MV_USB_DEVICE;
    // MV_CC_DEVICE_INFO_LIST m_stDevList = {0};
    //int nRet = MV_CC_EnumDevices(nTLayerType, &m_stDevList);
    // int nDeviceIndex = 0;
    MV_CC_DEVICE_INFO m_stDevInfo = {0};
    memcpy(&m_stDevInfo, m_stDevList.pDeviceInfo[id], sizeof(MV_CC_DEVICE_INFO));
    char strUserName7[256] = {0};
    sprintf_s(strUserName7, "%s", m_stDevList.pDeviceInfo[id]->SpecialInfo.stGigEInfo.chSerialNumber);
    QString qStr(strUserName7);

    nRet = MV_CC_CreateHandle(&m_hDevHandle, &m_stDevInfo);
    unsigned int nAccessMode = MV_ACCESS_Exclusive;
    unsigned short nSwitchoverKey = 0;
    nRet = MV_CC_OpenDevice(m_hDevHandle, nAccessMode, nSwitchoverKey);//打开设备（连接设备）
    //   unsigned int nNum = 15;
    //   nRet = MV_CC_SetImageNodeNum(m_handle, nNum);
    //   nRet = MV_CC_RegisterImageCallBack(m_handle, ImageCallBack, NULL);
    if (MV_OK == nRet)
    {
        qDebug()<<"打开设备成功"<<endl;
        if(qStr=="00C62638017")//左彩色相
        {
            m_hDevHandle_1=m_hDevHandle;
            return 0;
        }
        if(qStr=="00D87499483")// 主彩色相机
        {
            m_hDevHandle_2=m_hDevHandle;
            return 0;
        }
        if(qStr=="00E08082919") //右彩色相机
        {
            m_hDevHandle_3=m_hDevHandle;
            qDebug()<<"connect_rightcamera"<<endl;
            return 0;
        }
        if(qStr=="00C89933429")//后面黑白相机
        {
            m_hDevHandle_5=m_hDevHandle;
            return 0;
        }
        if(qStr=="00E36174043")//前面黑白相机
        {
            m_hDevHandle_4=m_hDevHandle;
            return 0;
        }
        if(qStr=="00C72338603")//彩色相机
        {
            m_hDevHandle_6=m_hDevHandle;
            return 0;
        }
    }
    else
    {
        return -1;
    }
}

//启动相机采集
int HikvisionSDK::startCamera(void* m_hDevHandle)
{
    int nRet =MV_CC_StartGrabbing(m_hDevHandle);//取流操作
    //成功，返回MV_OK（0）；失败，返回错误码。
    if(nRet ==MV_OK)
    {
        qDebug()<<"采集成功"<<endl;
        qDebug()<<m_hDevHandle<<endl;
        return 0;
    }
    else
    {

        printf("error: StartGrabbing fail [%x]\n", nRet);
        qDebug()<<nRet<<endl;
        qDebug()<<"采集失败"<<endl;
        return -1;
    }
}

//停止相机采集
int HikvisionSDK::stopCamera(void* m_hDevHandle)
{
    int tempValue=MV_CC_StopGrabbing(m_hDevHandle);
    if(tempValue!=0)
    {
        return -1;
    }else
    {
        return 0;
    }
}

//关闭相机
int HikvisionSDK::closeCamera(void* m_hDevHandle)
{
    if (NULL == m_hDevHandle)
    {
        return -1;
    }
    MV_CC_CloseDevice(m_hDevHandle);
    int tempValue = MV_CC_DestroyHandle(m_hDevHandle);
    m_hDevHandle = NULL;
    if(tempValue!=0)
    {
        return -1;
    }else
    {
        qDebug()<<"关闭设备成功"<<endl;
        return 0;
    }
}

//发送软触发
int HikvisionSDK::softTrigger()
{
    int tempValue= MV_CC_SetCommandValue(m_hDevHandle, "TriggerSoftware");
    if(tempValue!=MV_OK )
    {
        return -1;
    }else
    {
        return 0;
    }
}

//读取相机中的图像
int HikvisionSDK::ReadBuffer(cv::Mat &image)
{
    cv::Mat getImage;
    unsigned int nRecvBufSize = 0;
    MVCC_INTVALUE stParam;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    int tempValue = MV_CC_GetIntValue(m_hDevHandle, "PayloadSize", &stParam);
    if (tempValue != 0)
    {
        return -1;
    }
    nRecvBufSize = stParam.nCurValue;
    unsigned char* pDate;
    pDate=(unsigned char *)malloc(nRecvBufSize);

    MV_FRAME_OUT_INFO_EX stImageInfo = {0};
    tempValue= MV_CC_GetOneFrameTimeout(m_hDevHandle, pDate, nRecvBufSize, &stImageInfo, 700);
    if(tempValue!=0)
    {
        return -1;
    }
    int m_nBufSizeForSaveImage = stImageInfo.nWidth * stImageInfo.nHeight * 3 + 2048;
    unsigned char* m_pBufForSaveImage;
    m_pBufForSaveImage = (unsigned char*)malloc(m_nBufSizeForSaveImage);


    bool isMono;
    switch (stImageInfo.enPixelType)
    {
    case PixelType_Gvsp_Mono8:
    case PixelType_Gvsp_Mono10:
    case PixelType_Gvsp_Mono10_Packed:
    case PixelType_Gvsp_Mono12:
    case PixelType_Gvsp_Mono12_Packed:
        isMono=true;
        break;
    default:
        isMono=false;
        break;
    }
    if(isMono)
    {
        getImage=cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC1,pDate);
        //imwrite("d:\\测试opencv_Mono.tif", image);
    }
    else
    {
        //转换图像格式为BGR8
        MV_CC_PIXEL_CONVERT_PARAM stConvertParam = {0};
        memset(&stConvertParam, 0, sizeof(MV_CC_PIXEL_CONVERT_PARAM));
        stConvertParam.nWidth = stImageInfo.nWidth;                 //ch:图像宽 | en:image width
        stConvertParam.nHeight = stImageInfo.nHeight;               //ch:图像高 | en:image height
        //stConvertParam.pSrcData = m_pBufForDriver;                  //ch:输入数据缓存 | en:input data buffer
        stConvertParam.pSrcData = pDate;                  //ch:输入数据缓存 | en:input data buffer
        stConvertParam.nSrcDataLen = stImageInfo.nFrameLen;         //ch:输入数据大小 | en:input data size
        stConvertParam.enSrcPixelType = stImageInfo.enPixelType;    //ch:输入像素格式 | en:input pixel format
        stConvertParam.enDstPixelType = PixelType_Gvsp_BGR8_Packed; //ch:输出像素格式 | en:output pixel format  适用于OPENCV的图像格式
        //stConvertParam.enDstPixelType = PixelType_Gvsp_RGB8_Packed; //ch:输出像素格式 | en:output pixel format
        stConvertParam.pDstBuffer = m_pBufForSaveImage;                    //ch:输出数据缓存 | en:output data buffer
        stConvertParam.nDstBufferSize = m_nBufSizeForSaveImage;            //ch:输出缓存大小 | en:output buffer size
        MV_CC_ConvertPixelType(m_hDevHandle, &stConvertParam);

        getImage=cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,m_pBufForSaveImage);
        //imwrite("d:\\测试opencv_Color.tif", image);
    }
    getImage.copyTo(image);
    getImage.release();
    free(pDate);
    free(m_pBufForSaveImage);
    return 0;
}

//获取图像最大高度值
int HikvisionSDK::getHeight()
{
    MVCC_INTVALUE stParam;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    int tempValue=MV_CC_GetIntValue(m_hDevHandle, "Height", &stParam);
    int value= stParam.nCurValue;
    if(tempValue!=0)
    {
        return -1;
    }else
    {
        return value;
    }
}

//获取图像最大宽度值
int HikvisionSDK::getWidth()
{
    MVCC_INTVALUE stParam;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    int tempValue=MV_CC_GetIntValue(m_hDevHandle, "Width", &stParam);
    int value= stParam.nCurValue;
    if(tempValue!=0)
    {
        return -1;
    }else
    {
        return value;
    }
}

//获取相机曝光时间
float HikvisionSDK::getExposureTime()
{
    MVCC_FLOATVALUE stParam;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    int tempValue=MV_CC_GetFloatValue(m_hDevHandle, "ExposureTime", &stParam);
    float value= stParam.fCurValue;
    if(tempValue!=0)
    {
        return -1;
    }else
    {
        return value;
    }
}

//设置图像ROI高度
int HikvisionSDK::setHeight(unsigned int height)
{
    int tempValue=MV_CC_SetIntValue(m_hDevHandle, "Height", height);
    if(tempValue!=0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

//设置图像ROI宽度
int HikvisionSDK::setWidth(unsigned int width)
{
    int tempValue=MV_CC_SetIntValue(m_hDevHandle, "Width", width);
    if(tempValue!=0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

//设置图像水平偏移OffsetX
int HikvisionSDK::setOffsetX(unsigned int offsetX)
{
    int tempValue=MV_CC_SetIntValue(m_hDevHandle, "OffsetX", offsetX);
    if(tempValue!=0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

//设置图像竖直偏移OffsetY
int HikvisionSDK::setOffsetY(unsigned int offsetY)
{
    int tempValue=MV_CC_SetIntValue(m_hDevHandle, "OffsetY", offsetY);
    if(tempValue!=0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

//设置是否为触发模式
int HikvisionSDK::setTriggerMode(unsigned int TriggerModeNum)
{
     int tempValue= MV_CC_SetEnumValue(m_hDevHandle,"TriggerMode", TriggerModeNum);
     if(tempValue!=0)
     {
         return -1;
     }else
     {
         return 0;
     }
}

//设置触发源
int HikvisionSDK::setTriggerSource(unsigned int TriggerSourceNum)
{
    //0：Line0  1：Line1  7：Software
     int tempValue= MV_CC_SetEnumValue(m_hDevHandle,"TriggerSource", TriggerSourceNum);
     if(tempValue!=0)
     {
         return -1;
     }else
     {
         return 0;
     }
}

//设置帧率控制使能
int HikvisionSDK::setFrameRateEnable(bool comm)
{
    int tempValue =MV_CC_SetBoolValue(m_hDevHandle, "AcquisitionFrameRateEnable", comm);
    if (tempValue != 0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

//设置心跳时间
int HikvisionSDK::setHeartBeatTime(unsigned int time)
{
    //心跳时间最小为500ms
    if(time<500)
        time=500;
    int tempValue=MV_CC_SetIntValue(m_hDevHandle, "GevHeartbeatTimeout", time);
    if(tempValue!=0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

/*====================================================================
* 类型：setExposureTime
* 功能：设置曝光时间
* 输入：曝光时间以毫秒为单位
* 输出：
* 其他：
======================================================================*/
int HikvisionSDK::setExposureTime(float ExposureTimeNum)
{
    int tempValue= MV_CC_SetFloatValue(m_hDevHandle, "ExposureTime",ExposureTimeNum );
    if(tempValue!=0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

//清理相机缓存
void HikvisionSDK::clearBuffer()
{
    //stopCamera();
    //startCamera();
}
cv::Mat HikvisionSDK::saveImage(void* handel)
 {

//     if (NULL ==  pFrameBuf)
//     {

//         MVCC_INTVALUE stIntvalue = {0};
//       //  MVCC_INTVALUE stParam;
//        // memset(&stParam, 0, sizeof(MVCC_INTVALUE));
//         int nRet = MV_CC_GetIntValue(handel, "PayloadSize", &stIntvalue);
//         nRecvBufSize = stIntvalue.nCurValue; //一帧数据大小
//         if (nRet == MV_OK)
//         {
//             qDebug()<<stIntvalue.nCurValue<<endl;

//         }

//         pFrameBuf = (unsigned char*)malloc(nRecvBufSize);
//     }
//     MV_FRAME_OUT_INFO_EX stImageInfo = {0};
//     memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
//     unsigned int    nTestFrameSize = 0;
//     while(1)
//         {
//             if (nTestFrameSize > 20)
//             {
//                 break;
//             }
//             nRet = MV_CC_GetOneFrameTimeout(handel,pFrameBuf,nRecvBufSize, &stImageInfo, 1000);

//             if (MV_OK != nRet)
//             {   qDebug()<<"第几次"<<nTestFrameSize<<endl;
//                 Sleep(100);
//             }
//             else
//             {
//                 //图片数据输入输出参数
////                 if(pImage==NULL)
////                {
//                     m_nBufSizeForSaveImage = stImageInfo.nWidth * stImageInfo.nHeight * 3 + 2048;
//                     pImage = (unsigned char*)malloc(m_nBufSizeForSaveImage);
//               // }
//                  MV_SAVE_IMAGE_PARAM stParam ={0};
//                 //源数据
//                 stParam.pData         = pFrameBuf;                //原始图像数据
//                 stParam.nDataLen      = stImageInfo.nFrameLen;    //原始图像数据长度
//                 stParam.enPixelType   = stImageInfo.enPixelType;  //原始图像数据的像素格式
//                 stParam.nWidth        = stImageInfo.nWidth;       //图像宽
//                 stParam.nHeight       = stImageInfo.nHeight;      //图像高
//                 //目标数据
//                 stParam.enImageType   =  MV_Image_Bmp;    // MV_Image_Jpeg  // MV_Image_Bmp     //需要保存的图像类型，转换成JPEG格式
//                 stParam.nBufferSize   = m_nBufSizeForSaveImage;                 //存储节点的大小
//                 stParam.pImageBuffer  = pImage;                   //输出数据缓冲区，存放转换之后的图片数据
//                 nRet = MV_CC_SaveImage(&stParam);
//                 if(MV_OK != nRet)
//                 {
//                     break;
//                 }
//                 cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage);
//                 free(pImage);
//                 return getImage;


//             }
//              nTestFrameSize++;
//         }
 }

/*====================================================================
* 类型：saveImage7
* 功能：对相机图像进行取帧包括彩色图像和黑白图像
* 输入：相机ID号总共6个相机
* 输出：取帧图像
* 其他：4月撰写
======================================================================*/
cv::Mat HikvisionSDK::saveImage7(int id)
{
    void* handel = NULL;
    switch (id)
    {
    case 1:handel = m_hDevHandle_1; break;
    case 2:handel = m_hDevHandle_2; break;
    case 3:handel = m_hDevHandle_3; break;
    case 4:handel = m_hDevHandle_4; break;
    case 5:handel = m_hDevHandle_5; break;
    case 6:handel = m_hDevHandle_6; break;
    }

    unsigned char*  pFrameBuf = NULL; //为一帧数据大小分配内存空间此处用来存储黑白相机图像
    MVCC_INTVALUE stIntvalue = { 0 }; //相机节点名称
    int nRet = MV_CC_GetIntValue(handel, "PayloadSize", &stIntvalue);

    unsigned int nRecvBufSize = 0;
    nRecvBufSize = stIntvalue.nCurValue; //海康的注释,一帧数据大小 8147712
    pFrameBuf = (unsigned char*)malloc(nRecvBufSize);//pFrameBuf为一帧数据大小分配内存空间用来保存黑白相机

    // MV_FRAME_OUT_INFO_EX:输出的帧信息结构体
    MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));

    unsigned int nTestFrameSize = 0;
    while (1)
    {
        if (nTestFrameSize > 20)
        {
            break;
        }
        //获取一帧图片，支持获取chunk信息和设置超时时间
        //设备句柄  pFrameBuf用于保存图像数据的缓存地址 nRecvBufSize缓存大小  stImageInfo:获取到的帧信息，包括chunk信息  等待超时时间，100毫秒
        nRet = MV_CC_GetOneFrameTimeout(handel, pFrameBuf, nRecvBufSize, &stImageInfo, 100);//pFrameBuf为一帧数据大小分配内存空间用来保存黑白相机
        if (MV_OK != nRet)
        {
            Sleep(100);
        }
        else //采集到图像开始处理
        {
            unsigned char* pImage = NULL;//保存图像的开辟缓存大小用来保存彩色相机指针
            unsigned int m_nBufSizeForSaveImage;//保存图像的开辟缓存大小用来保存彩色相机
            m_nBufSizeForSaveImage = stImageInfo.nWidth * stImageInfo.nHeight * 3 + 2048; //m_pBufForSaveImage:分配缓存大小 24445184
            pImage = (unsigned char*)malloc(m_nBufSizeForSaveImage);//pImage保存图像的开辟缓存大小用来保存彩色相机

            bool isMono;//判断是否为黑白图像
            switch (stImageInfo.enPixelType)//图像格式判断
            {
            case PixelType_Gvsp_Mono8:
            case PixelType_Gvsp_Mono10:
            case PixelType_Gvsp_Mono10_Packed:
            case PixelType_Gvsp_Mono12:
            case PixelType_Gvsp_Mono12_Packed:
                isMono = true;
                break;
            default:
                isMono = false;
                break;
            }
            if (isMono)//若为黑白格式图像格式
            {
                switch (id)
                {
                case 1:cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, pFrameBuf).copyTo(getImage1);
                    free(pImage);
                    free(pFrameBuf);
                    //imwrite("D:\\getImage10323return.bmp",getImage1);
                    return getImage1;
                case 2: cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, pFrameBuf).copyTo(getImage2);
                    free(pImage);
                    free(pFrameBuf);
                    //imwrite("D:\\getImage20323return.bmp",getImage2);
                    return getImage2;
                case 3:cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, pFrameBuf).copyTo(getImage3);
                    free(pImage);
                    free(pFrameBuf);
                    return getImage3;
                case 4: cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, pFrameBuf).copyTo(getImage4);
                    free(pImage);
                    free(pFrameBuf);
                    return getImage4;
                case 5:cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, pFrameBuf).copyTo(getImage5);
                    free(pImage);
                    free(pFrameBuf);
                    return getImage5;
                case 6: cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, pFrameBuf).copyTo(getImage6);
                    free(pImage);
                    free(pFrameBuf);
                    return getImage6;
                }
            }
            else//彩色相机
            {
                //图片格式转换参数结构体 pImageBuffer 输出数据缓冲区，存放转换之后的图片数据
                MV_SAVE_IMAGE_PARAM stParam = { 0 }; //像素格式转换输入输出参数
                memset(&stParam, 0, sizeof(MV_SAVE_IMAGE_PARAM));

                stParam.pData=pFrameBuf;                            //用于保存图像数据的缓存地址
                stParam.nDataLen=stImageInfo.nFrameLen;             //原始图像数据长度
                stParam.enPixelType=stImageInfo.enPixelType;        //enPixelType 相机对应的像素格式
                stParam.nWidth=stImageInfo.nWidth;                  //图像宽
                stParam.nHeight=stImageInfo.nHeight;                //图像高

                stParam.enImageType=MV_Image_Bmp;                   // MV_Image_Jpeg  MV_Image_Bmp
                stParam.nBufferSize=m_nBufSizeForSaveImage;         //输出数据缓冲区大小
                stParam.pImageBuffer=pImage;                        //输出数据缓冲区，存放转换之后的图片数据彩色

                //将原始图像数据转换成图片格式并保存在指定内存里
                //输入参数：pSaveParam:图片数据输入输出参数
                nRet = MV_CC_SaveImage(&stParam);
                if (MV_OK != nRet)
                {
                    break;
                }
                switch (id)
                {
                case 1:cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3, pImage).copyTo(getImage1);
                    flip(getImage1, getImage1, 0); //上下镜像翻转
                    free(pImage);
                    free(pFrameBuf);
                    return getImage1;
                case 2: cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3, pImage).copyTo(getImage2);
                    flip(getImage2, getImage2, 0); //上下镜像翻转
                    free(pImage);
                    free(pFrameBuf);
                    return getImage2;
                case 3:cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3, pImage).copyTo(getImage3);
                    flip(getImage3, getImage3, 0); //上下镜像翻转
                    free(pImage);
                    free(pFrameBuf);
                    return getImage3;
                case 4: cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3, pImage).copyTo(getImage4);
                    free(pImage);
                    free(pFrameBuf);
                    return getImage4;
                case 5:cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3, pImage).copyTo(getImage5);
                    free(pImage);
                    free(pFrameBuf);
                    return getImage5;
                case 6: cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3, pImage).copyTo(getImage6);
                    flip(getImage6, getImage6, 0); //上下镜像翻转
                    free(pImage);
                    free(pFrameBuf);
                    //imwrite("D:\\getImage6return.bmp",getImage6);
                    return getImage6;
                }
            }
        }
        nTestFrameSize++;
    }
}

/*====================================================================
* 类型：saveImage
* 功能：取帧图像不用了
* 输入：相机ID号总共6个                                                                                                                                                                  zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz相 机
* 输出：取帧图像
* 其他：
======================================================================*/
cv::Mat HikvisionSDK::saveImage(void* handel,int id)
 {     //cv::Mat getImage;

       unsigned char*  pFrameBuf = NULL;
//     if (NULL ==  pFrameBuf)
//     {

        MVCC_INTVALUE stIntvalue = {0};
       //  MVCC_INTVALUE stParam;
        // memset(&stParam, 0, sizeof(MVCC_INTVALUE));
         int nRet = MV_CC_GetIntValue(handel, "PayloadSize", &stIntvalue);
         unsigned int nRecvBufSize = 0;
         nRecvBufSize = stIntvalue.nCurValue; //一帧数据大小
//         if (nRet == MV_OK)
//         {
//            // qDebug()<<stIntvalue.nCurValue<<endl;

//         }

         pFrameBuf = (unsigned char*)malloc(nRecvBufSize);
   //  }
     MV_FRAME_OUT_INFO_EX stImageInfo = {0};
     memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
     unsigned int    nTestFrameSize = 0;
     while(1)
         {
             if (nTestFrameSize > 20)
             {
                 break;
             }
             nRet = MV_CC_GetOneFrameTimeout(handel,pFrameBuf,nRecvBufSize, &stImageInfo, 100);

             if (MV_OK != nRet)
             {  // qDebug()<<"第几次"<<nTestFrameSize<<endl;
                 Sleep(100);
             }
             else
             {
                 //图片数据输入输出参数
//                 if(pImage==NULL)
//                {
                     unsigned int    m_nBufSizeForSaveImage;
                     m_nBufSizeForSaveImage = stImageInfo.nWidth * stImageInfo.nHeight * 3 + 2048;
                     unsigned char* pImage=NULL;
                     pImage = (unsigned char*)malloc(m_nBufSizeForSaveImage);
               // }
                  MV_SAVE_IMAGE_PARAM stParam ={0};
                 //源数据
                 stParam.pData         = pFrameBuf;                //原始图像数据
                 stParam.nDataLen      = stImageInfo.nFrameLen;    //原始图像数据长度
                 stParam.enPixelType   = stImageInfo.enPixelType;  //原始图像数据的像素格式
                 stParam.nWidth        = stImageInfo.nWidth;       //图像宽
                 stParam.nHeight       = stImageInfo.nHeight;      //图像高
                 //目标数据
                 stParam.enImageType   = MV_Image_Bmp;    // MV_Image_Jpeg  // MV_Image_Bmp     //需要保存的图像类型，转换成JPEG格式
                 stParam.nBufferSize   = m_nBufSizeForSaveImage;                 //存储节点的大小
                 stParam.pImageBuffer  = pImage;                   //输出数据缓冲区，存放转换之后的图片数据
                 nRet = MV_CC_SaveImage(&stParam);
                 if(MV_OK != nRet)
                 {
                     break;
                 }
                 if(id==1)
                 {
                     cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage1);
                    // getImage.release();
                     free(pImage);
                     free(pFrameBuf);
                     return getImage1;
                 }
                 if(id==2)
                 {
                      cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage2);
                      free(pImage);
                      free(pFrameBuf);
                      return getImage2;
                 }

                 if(id==3)
                 {
                      cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage3);
                      free(pImage);
                      free(pFrameBuf);
                      return getImage3;
                 }

                 if(id==4)
                 {
                      cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage4);
                      free(pImage);
                      free(pFrameBuf);
                      return getImage4;
                 }

                 if(id==5)
                 {
                      cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage5);
                      free(pImage);
                      free(pFrameBuf);
                      return getImage5;
                 }

                 if(id==6)
                 {
                      cv::Mat(stImageInfo.nHeight,stImageInfo.nWidth,CV_8UC3,pImage).copyTo(getImage6);
                      free(pImage);
                      free(pFrameBuf);
                      return getImage6;
                 }

             }
              nTestFrameSize++;
         }
 }
