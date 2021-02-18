#ifndef GLOBAL_VARIABLE_H
#define GLOBAL_VARIABLE_H

#include <QMainWindow>
#include <QWidget>
#include "dbhelper.h"
#include <QSqlTableModel>
#include <QDialog>

extern bool isCeshi;
extern bool isFrontBack;
extern bool isColor;

extern int Exposure_Ceguang_Front;              //主黑白相机侧光曝光值
extern int Exposure_Ceguang_Back;               //主黑白相机侧光曝光值
extern int Exposure_Ceguang_Left;               //主黑白相机侧光曝光值
extern int Exposure_Ceguang_Right;              //主黑白相机侧光曝光值
extern int Exposure_Ceguang_Main;               //主黑白相机侧光曝光值

extern int Exposure_White_Black_Left;           //左黑白相机曝光值
extern int Exposure_White_Black_Middle;         //主黑白相机曝光值
extern int Exposure_White_Color_Right;          //主彩色相机曝光值
extern int Exposure_White_Black_Front;          //前黑白相机曝光值
extern int Exposure_White_Black_Back;           //前黑白相机曝光值
extern int Exposure_White_Color_Middle;         //前黑白相机曝光值

extern int Exposure_Black_Middle;               //主黑白相机黑底曝光值
extern int Exposure_Black_Color;                //主彩色相机黑底曝光值

extern int Exposure_Gray_Left;                  //左相机灰底曝光值
extern int Exposure_Gray_Right;                 //右相机灰底曝光值
extern int Exposure_Gray_Main;                  //主相机灰底曝光值
extern int Exposure_Gray_Color;                 //主彩色相机灰底曝光值

extern int Num_1_L_Exposure_white;
extern int Num_1_R_Exposure_white;
extern int Num_1_L_Exposure_black;
extern int Num_1_R_Exposure_black;

extern int Num_2_L_Exposure_white;
extern int Num_2_R_Exposure_white;
extern int Num_2_L_Exposure_black;
extern int Num_2_R_Exposure_black;


extern int Num_1_ceLL_Exposure_white;
extern int Num_1_ceLR_Exposure_white;
extern int Num_1_ceRL_Exposure_white;
extern int Num_1_ceRR_Exposure_white;

extern int Num_2_ceLL_Exposure_white;
extern int Num_2_ceLR_Exposure_white;
extern int Num_2_ceRL_Exposure_white;
extern int Num_2_ceRR_Exposure_white;


extern int BoxOpenTime;
extern int BoxSwitchTime;
extern int GraySwitchTime;
extern int SideLightOpenTime;
extern int BlackSwitchTime;

extern int Std;

extern int yiwu_pre_size;
extern double yiwu_pre_th;
extern int yiwu_area_lower;
extern int yiwu_area_upper;
extern int yiwu_k_w_h;
extern int yiwu_ceguang_th;
extern int yiwu_sec_th;
extern int yiwu_sec_size;
extern int yiwu_sec_area_lower;
extern int yiwu_sec_area_upper;
extern double  yiwu_lighth;

extern double white_gray_lower;
extern int black_gray_upper;

extern double black_light_gram;
extern double black_light_red;
extern double black_colorline;
extern int black_light_th1;
extern int black_light_th2;


extern int black_lightPoint_step;
extern int black_lightPoint_size;
extern int black_lightPoint_gray_th;
extern double black_lightPoint_area_lower;
extern int black_lightPoint_area_upper;
extern int black_lightPoint_diff;
extern double black_lightPoint_w_h;

extern int redBlue_area_lower;
extern int redBlue_area_upper;
extern int redBlue_numupper;

extern int Whiteprint_Detect_Flag;
extern int black_mura_step;
extern int black_mura_size;
extern double black_mura_gray_th;
extern int black_mura_area_lower;
extern int black_mura_area_upper;
extern int black_mura_boder;
extern int black_mura_corner;
extern int black_mura_area_1;
extern int black_mura_th_1;
extern int black_mura_area_2;
extern int black_mura_th_2;
extern double mura_bubbleth;
extern double mura_centerlighth;
extern double mura_lighth;
extern int modbus_time;

extern int white_seban_size;
extern int white_seban_size2;
extern int white_seban_step;
extern int white_seban_th_h_low;
extern int white_seban_th_s_low;
extern int white_seban_th_v_low;
extern int white_seban_th_h_up;
extern int white_seban_th_s_up;
extern int white_seban_th_v_up;
extern int white_seban_area_lower;
extern int white_seban_area_upper;

extern int lackline_bolckSize;  //少线分割邻域块大小
extern double lackline_delta;   //少线分割邻域块阈值
extern double longShortlowerth; //少线长宽比下限
extern int shortHigherth;
extern int longLowerth;
extern double lackscratchth;

extern double whitePoint_threshold;
extern int whitePoint_step;         //步长
extern int whitePoint_lowerArea;    //面积下限
extern int whitePoint_higherArea;   //面积上限
extern double scratchth;            //白点排除划痕阈值大于则认为贴膜划痕
extern double bubbleth;             //白点排除气泡阈值大于则认为贴膜气泡
extern int dotdomainnum;            //白点连通域个数大于则认为噪点
extern double defectouterth;	    //白点缺陷处灰度均值与缺陷外围灰度差
extern double corewholeth;          //白点缺陷中心点灰度值与整块疑似缺陷灰度均值差
extern double spotpeak;	            //白点缺陷最亮点与最暗点灰度差
extern double siecevariance;	    //侧光图中排除划痕参数
extern double whitePoint_w_h;

extern int scratch__bolckSize;         //划伤自适应分割领域块大小
extern double scratch__delta;          //划伤自适应分割领阈值大小
extern int scratch_lowerArea;          //划伤面积下限
extern int scratch_higherArea;         //划伤面积上限
extern int scratch_lowerWidth;         //划伤宽度下限
extern int scratch_higherWidth;        //划伤宽度上限
extern double scratch_lowerLongShort;  //划伤长宽比下限
extern double scratch_higherLongShort; //划伤长宽比上限
extern int scratch_lowerLength;        //划伤长度下限
extern int scratch_higherLength;       //划伤长度上限
extern double filmscratch;             //贴膜划痕系数
extern double scratchbubbleth;         //贴膜排除气泡系数
extern double scratchbubbleth1;        //贴膜排除气泡系数2
extern double scratchbrighth;          //划伤亮度阈值
extern double minscratchbrighth;       //最小划伤亮度阈值

extern int G_R_diff;
extern int gray_thre;
//extern int test_num;

extern int leakage_bolckSize;     //黑底漏光适应分割领域块大小
extern double leakage_delta;         //黑底漏光适应分割阈值大小
extern int leakage_area_lower;
extern int leakage_area_upper;
extern double leakageth;
extern double leakage_w_h;

extern double test_num;        //测试总数目
extern double qualified_num;   //合格总数目
extern int yiwu_num;        //异物缺陷数目
extern int liangdian_num;   //亮点
extern int white_num;       //白点数目
extern int mura_num;        //白印数目
extern int lackline_num;    //少线数目
extern int leakage_num;     //漏光数目
extern int redBlue_num;     //红蓝斑数目
extern int dispalyError_num;//显示异常数目
extern int scratch_num;     //划伤数目
extern int dengyan_num;     //划伤数目
extern int liangbian_num;     //划伤数目
extern int louguang_num;     //划伤数目
extern int biankuang_num;     //划伤数目
extern int anjiao_num;     //划伤数目
extern int shaoliao_num;     //划伤数目
extern int baodeng_num;     //划伤数目
extern int shizi_num;     //划伤数目
extern int secha_num;     //划伤数目
extern int dazhe_num;     //划伤数目
extern double pass_per;     //合格率

//**************相机亮度参数***************//
extern double light_num;//可以点亮的屏幕的数量
extern double main_light;
extern double right_light;
extern double left_light;
extern double Average_mainlight;
extern double Average_rightlight;
extern double Average_leftlight;
extern double Max_mainlight;
extern double Min_mainlight;
extern double Max_rightlight;
extern double Min_rightlight;
extern double Max_leftlight;
extern double Min_leftlight;
//**************相机亮度参数***************//

extern double main_whitemean;
extern double left_whitemean;
extern double right_whitemean;
extern int screen_type;

extern QString Model_Mod_id;
extern QString Pro_Model_Mod;
extern QString Model_Mod_type;
extern QString Model_Mod_size;
extern QString Model_Mod_fenbian;
extern QString Model_Mod_shuaping;
extern QString Model_Mod_waixing;
extern QString Model_Mod_fangan;
extern QString Model_Mod_pingbi;

extern QString Model_Code;     //产品型号编码
extern QString Modcopy_bef;    //复制前厂家产品型号
extern QString Modcopy_aft;    //复制后厂家产品型号
extern QString Mode_Code_bef;  //复制前内部产品型号编码
extern QString Mode_Code_aft;  //复制后内部产品型号编码


extern QString product_ID;         //产品批次信息
extern QString product_Lot;
extern QString product_Customize_Lot;
extern QString product_Time;
extern QString product_Type;
extern QString paraConfigOk;        //参数配置完成标志位

extern bool Form_Camera_Show;//相机窗口是否存在标志位
extern bool production_lot_information_win_show;//批次窗口是否存在的标志位
extern bool promodel_show;//型号窗口是否存在的标志位
extern bool parameter_control_show;//参数窗口是否已经存在的标志位
extern bool save_images_show;//保存窗口是否已经存在的标志位
extern bool datastatistics_win_show;//统计窗口是否已经存在的标志位

extern QString imageSavePath;//图片保存路径
extern bool autoSave;//是否自动保存标志位
extern bool SaveSize;//保存的类型标志位，0-保存缺陷样本；1-保存全部样本
extern bool m_checked;             //是否选中

extern bool ForeignBody_Checked; //异物检测标志位
extern bool LessMaterial_Checked;//少料检测标志位
extern bool CrossLine_Checked; //十字检测标志位
extern bool boom_light_Checked;//爆灯检测标志位
extern bool Frame_Checked; //边框检测标志位
extern bool mura_Checked;  //白印、膜拱、白团检测标志位
extern bool LampEye_Checked; //灯眼检测标志位
extern bool Brightedge_Checked;  //亮边检测标志位
extern bool WhiteDot_Checked; //白点检测标志位
extern bool Scratch_Checked;  //划伤检测标志位
extern bool Abshow_White_Detect_Checked; //显异、暗区检测标志位
extern bool Shitfing_Checked;//移位标志位
extern bool DarkCorner_Checked;  //暗角检测标志位
extern bool SunSpots_Checked;   //色差检测标志位
extern bool Light_leakage_Checked;

extern double size_long;
extern double size_width;
extern double roi_proportion;
extern int pixel_num;

extern bool Ext_Result_BlackWhite;
extern bool Ext_Result_Left;
extern bool Ext_Result_Right;//屏幕提取异常标志

#endif // GLOBAL_VARIABLE_H
