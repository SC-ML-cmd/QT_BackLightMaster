#include "parameter.h"
#include "ui_parameter.h"
#include"global_variable.h"
#include<QDebug>
//int Num_1_L_Exposure_white;
//int Num_1_R_Exposure_white;
//int Num_1_L_Exposure_black;
//int Num_1_R_Exposure_black;

//int Num_2_L_Exposure_white;
//int Num_2_R_Exposure_white;
//int Num_2_L_Exposure_black;
//int Num_2_R_Exposure_black;
parameter::parameter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::parameter)
{
    ui->setupUi(this);
    ui->lineEdit->setText(QString::number(Exposure_Ceguang_Main));    //主黑白相机侧光曝光值
    ui->lineEdit_2->setText(QString::number(Exposure_White_Black_Left));
    ui->lineEdit_3->setText(QString::number(Exposure_White_Black_Middle));
    ui->lineEdit_4->setText(QString::number(Exposure_White_Color_Right));

    ui->lineEdit_5->setText(QString::number(Exposure_Black_Middle));
    ui->lineEdit_6->setText(QString::number(Exposure_Black_Color));
    ui->lineEdit_7->setText(QString::number(Num_1_L_Exposure_white));
    ui->lineEdit_8->setText(QString::number(Exposure_White_Color_Middle));

    ui->lineEdit_12->setText(QString::number(BoxOpenTime));
    ui->lineEdit_15->setText(QString::number(Std));

    ui->lineEdit_16->setText(QString::number(yiwu_pre_th));
    ui->lineEdit_17->setText(QString::number(yiwu_pre_size));
    ui->lineEdit_18->setText(QString::number(yiwu_area_lower));
    ui->lineEdit_19->setText(QString::number(yiwu_area_upper));
    ui->lineEdit_21->setText(QString::number(yiwu_k_w_h));

    ui->lineEdit_20->setText(QString::number(yiwu_sec_th));
    ui->lineEdit_22->setText(QString::number(yiwu_sec_size));
    ui->lineEdit_23->setText(QString::number(yiwu_sec_area_lower));
    ui->lineEdit_24->setText(QString::number(yiwu_sec_area_upper));
    ui->lineEdit_25->setText(QString::number(white_gray_lower));
    ui->lineEdit_26->setText(QString::number(black_gray_upper));

    ui->lineEdit_27->setText(QString::number(black_light_gram));
    ui->lineEdit_28->setText(QString::number(black_light_th1));
    ui->lineEdit_29->setText(QString::number(black_light_th2));

    ui->lineEdit_30->setText(QString::number(black_lightPoint_step));
    ui->lineEdit_31->setText(QString::number(black_lightPoint_size));
    ui->lineEdit_32->setText(QString::number(black_lightPoint_gray_th));
    ui->lineEdit_33->setText(QString::number(black_lightPoint_area_lower));
    ui->lineEdit_34->setText(QString::number(black_lightPoint_area_upper));
    ui->lineEdit_35->setText(QString::number(black_lightPoint_diff));

    ui->lineEdit_36->setText(QString::number(black_mura_step));
    ui->lineEdit_40->setText(QString::number(black_mura_size));
    ui->lineEdit_37->setText(QString::number(black_mura_gray_th));
    ui->lineEdit_38->setText(QString::number(black_mura_area_lower));
    ui->lineEdit_39->setText(QString::number(black_mura_area_upper));
    ui->lineEdit_49->setText(QString::number(black_mura_boder));
    ui->lineEdit_50->setText(QString::number(black_mura_corner));
    ui->lineEdit_51->setText(QString::number(black_mura_area_1));
    ui->lineEdit_52->setText(QString::number(black_mura_th_1));
    ui->lineEdit_53->setText(QString::number(black_mura_area_2));
    ui->lineEdit_54->setText(QString::number(black_mura_th_2));

    ui->lineEdit_14->setText(QString::number(GraySwitchTime));

    ui->lineEdit_41->setText(QString::number(Num_1_ceLL_Exposure_white));
    ui->lineEdit_42->setText(QString::number(Num_1_ceLR_Exposure_white));
    ui->lineEdit_43->setText(QString::number(Num_1_ceRL_Exposure_white));
    ui->lineEdit_44->setText(QString::number(Num_1_ceRR_Exposure_white));

    ui->lineEdit_46->setText(QString::number(Num_2_ceLL_Exposure_white));
    ui->lineEdit_47->setText(QString::number(Num_2_ceLR_Exposure_white));
    ui->lineEdit_45->setText(QString::number(Num_2_ceRL_Exposure_white));
    ui->lineEdit_48->setText(QString::number(Num_2_ceRR_Exposure_white));
    ui->lineEdit_13->setText(QString::number(BlackSwitchTime));
    ui->lineEdit_9->setText(QString::number(modbus_time));

    ui->lineEdit_55->setText(QString::number(white_seban_step));
    ui->lineEdit_56->setText(QString::number(white_seban_size));
    ui->lineEdit_57->setText(QString::number(white_seban_th_h_low));
    ui->lineEdit_58->setText(QString::number(white_seban_th_s_low));
    ui->lineEdit_59->setText(QString::number(white_seban_th_v_low));

    ui->lineEdit_62->setText(QString::number(white_seban_th_h_up));
    ui->lineEdit_64->setText(QString::number(white_seban_th_s_up));
    ui->lineEdit_63->setText(QString::number(white_seban_th_v_up));

    ui->lineEdit_60->setText(QString::number(white_seban_area_lower));
    ui->lineEdit_61->setText(QString::number(white_seban_area_upper));

    ui->lineEdit_65->setText(QString::number(white_seban_size2));
    ui->lineEdit_66->setText(QString::number(black_light_red));

     ui->lineEdit_67->setText(QString::number(whitePoint_threshold));
     ui->lineEdit_68->setText(QString::number(whitePoint_step));
     ui->lineEdit_69->setText(QString::number(whitePoint_lowerArea));
     ui->lineEdit_70->setText(QString::number(whitePoint_higherArea));

      ui->lineEdit_71->setText(QString::number(G_R_diff));
      ui->lineEdit_72->setText(QString::number(gray_thre));
}

parameter::~parameter()
{
    delete ui;
}

void parameter::on_pushButton_clicked()
{

    Num_1_L_Exposure_black=ui->lineEdit->text().toInt();
    Exposure_White_Black_Left=ui->lineEdit_2->text().toInt();
    Exposure_White_Black_Middle=ui->lineEdit_3->text().toInt();
    Exposure_White_Color_Right=ui->lineEdit_4->text().toInt();


    Exposure_Black_Middle=ui->lineEdit_5->text().toInt();
    Exposure_Black_Color=ui->lineEdit_6->text().toInt();
    Num_1_L_Exposure_white=ui->lineEdit_7->text().toInt();

    Exposure_White_Color_Middle=ui->lineEdit_8->text().toInt();
    BoxOpenTime= ui->lineEdit_12->text().toInt();
    Std=ui->lineEdit_15->text().toInt();

    yiwu_pre_th=ui->lineEdit_16->text().toInt();
    yiwu_pre_size=ui->lineEdit_17->text().toInt();
    yiwu_area_lower=ui->lineEdit_18->text().toInt();
    yiwu_area_upper=ui->lineEdit_19->text().toInt();

    yiwu_sec_th=ui->lineEdit_20->text().toInt();
    yiwu_k_w_h=ui->lineEdit_21->text().toInt();
    yiwu_sec_size=ui->lineEdit_22->text().toInt();
    yiwu_sec_area_lower=ui->lineEdit_23->text().toInt();
    yiwu_sec_area_upper=ui->lineEdit_24->text().toInt();
    white_gray_lower=ui->lineEdit_25->text().toInt();
    black_gray_upper=ui->lineEdit_26->text().toInt();

    black_light_gram=ui->lineEdit_27->text().toFloat();
    black_light_th1=ui->lineEdit_28->text().toInt();
    black_light_th2=ui->lineEdit_29->text().toInt();

    black_lightPoint_step=ui->lineEdit_30->text().toInt();
    black_lightPoint_size=ui->lineEdit_31->text().toInt();
    black_lightPoint_gray_th=ui->lineEdit_32->text().toInt();
    black_lightPoint_area_lower=ui->lineEdit_33->text().toDouble();
    black_lightPoint_area_upper=ui->lineEdit_34->text().toInt();
    black_lightPoint_diff=ui->lineEdit_35->text().toInt();

    black_mura_step=ui->lineEdit_36->text().toInt();
    black_mura_size=ui->lineEdit_40->text().toInt();
    black_mura_gray_th=ui->lineEdit_37->text().toDouble();
    black_mura_area_lower=ui->lineEdit_38->text().toInt();
    black_mura_area_upper=ui->lineEdit_39->text().toInt();
    GraySwitchTime=ui->lineEdit_14->text().toInt();

    Num_1_ceLL_Exposure_white=ui->lineEdit_41->text().toInt();
    Num_1_ceLR_Exposure_white=ui->lineEdit_42->text().toInt();
    Num_1_ceRL_Exposure_white=ui->lineEdit_43->text().toInt();
    Num_1_ceRR_Exposure_white=ui->lineEdit_44->text().toInt();

    Num_2_ceLL_Exposure_white=ui->lineEdit_46->text().toInt();
    Num_2_ceLR_Exposure_white=ui->lineEdit_47->text().toInt();
    Num_2_ceRL_Exposure_white=ui->lineEdit_45->text().toInt();
    Num_2_ceRR_Exposure_white=ui->lineEdit_48->text().toInt();
    BlackSwitchTime=ui->lineEdit_13->text().toInt();

    black_mura_boder=ui->lineEdit_49->text().toInt();
    black_mura_corner=ui->lineEdit_50->text().toInt();
    black_mura_area_1=ui->lineEdit_51->text().toInt();
    black_mura_th_1=ui->lineEdit_52->text().toInt();
    black_mura_area_2=ui->lineEdit_53->text().toInt();
    black_mura_th_2=ui->lineEdit_54->text().toInt();
    modbus_time=ui->lineEdit_9->text().toInt();

    white_seban_step=ui->lineEdit_55->text().toInt();
    white_seban_size=ui->lineEdit_56->text().toInt();
    white_seban_th_h_low=ui->lineEdit_57->text().toInt();
    white_seban_th_s_low=ui->lineEdit_58->text().toInt();
    white_seban_th_v_low=ui->lineEdit_59->text().toInt();
    white_seban_th_h_up=ui->lineEdit_62->text().toInt();
    white_seban_th_s_up=ui->lineEdit_64->text().toInt();
    white_seban_th_v_up=ui->lineEdit_63->text().toInt();


    white_seban_area_lower=ui->lineEdit_60->text().toInt();
    white_seban_area_upper=ui->lineEdit_61->text().toInt();
    white_seban_size2=ui->lineEdit_65->text().toInt();
    black_light_red =ui->lineEdit_66->text().toDouble();
    whitePoint_threshold=ui->lineEdit_67->text().toInt();
    whitePoint_step=ui->lineEdit_68->text().toInt();
    whitePoint_lowerArea=ui->lineEdit_69->text().toInt();
    whitePoint_higherArea=ui->lineEdit_70->text().toInt();

    G_R_diff = ui->lineEdit_71->text().toInt();
    gray_thre=ui->lineEdit_72->text().toInt();
}
