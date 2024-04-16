// include here
#include "mbed.h"
#include "mros2.h"
#include "mros2-platform.h"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "strswitch.h"
#include "BNO055.h"
#include <cstdint>
#include <cstdio>
#include <math.h>
#include <vector>
#include "wav.h"
// #include "arducam.h"


// using and define here
using namespace std;


// 変数定義here
float x=0.0;        // 移動ベクトルのx成分(前:+)
float y=0.0;        // 移動ベクトルのy成分(左:+)
float angle=0.0;    // 角度(左回転:+)
float duty[4]={0,0,0,0};    // 最終的なduty 右前,左前,左後,右後
bool debugger=0;    // 0:off 1:on
bool stop=false;    // 一時停止
bool is_alive=false;// 生存確認 
float unko=2;       // 実質スピード
float goal=0.0;     // 目標値
float now=0.0;      // 現在の値
float ch=0.0;       // 補正
float p=0.01;       // P制御
float out=0.0;      // P制御出力
string wav="";      // wavファイル名


// 回路(PIN)定義here
PwmOut m00(PC_8);
PwmOut m01(PC_7);
PwmOut m10(PA_6);
PwmOut m11(PA_5);
PwmOut m20(PC_9);
PwmOut m21(PD_14);
PwmOut m30(PE_9);
PwmOut m31(PD_15);
PwmOut sound(PE_14);
PwmOut* motor[4][2]={{&m00,&m01},{&m10,&m11},{&m20,&m21},{&m30,&m31}};
DigitalOut led1(LED1);  // main関数生存確認
DigitalOut led2(LED2);  // 通信確認
DigitalOut power(LED3); // 電源オンオフ&確認
BNO055 cjk(PB_11,PB_10);  // 地磁気
// ArduCAM cam(PC_12,PC_11,PC_10,PD_2,PB_9,PB_8);          // カメラ
Ticker keep_aliver;     // 割り込み
Thread sound_th;


// 関数定義here
void cmd_callback(std_msgs::msg::String *msg);  // コマンド
void calculate_duty(geometry_msgs::msg::Twist *twist);  // 移動
void zero(void);    // すべてのモーターストップ
void pls_keep_alive(void);  // 通信生存確認
void get_cjk(void);     // 地磁気処理
float fix(float r);     // 角度を -180~180に変換する
void echo();
void sound_loop();


// プログラムhere
int main(){
    MROS2_INFO("start");
    if (mros2_platform::network_connect()) {            // connect to the network
        MROS2_ERROR("ネットつながってないにょ...");
        return -1;
    } else {
        MROS2_INFO("ネットつながた！---");
    }

    cjk.reset();
    MROS2_INFO("地磁気確認開始");
    while(!cjk.check()){printf("地磁気こないよぉ....\n");}
    MROS2_INFO("地磁気確認ﾖｼ！");

    MROS2_INFO("mini robot起動します");
    mros2::init(0, NULL);       // 初期化
    MROS2_DEBUG("mROS 2初期化完了");

    mros2::Node node = mros2::Node::create_node("mros2_node");  // node生成
    mros2::Subscriber sub_cmd = node.create_subscription<std_msgs::msg::String>("cmd", 10, cmd_callback);   // コマンド入力
    mros2::Subscriber sub_vel = node.create_subscription<geometry_msgs::msg::Twist>("cmd_vel", 10, calculate_duty);     // 移動ベクトル
    // mros2::Publisher pub_cam = node.create_publisher<sensor_msgs::msg::Image>("image",10);

    osDelay(100);
    MROS2_INFO("送受信準備完了！");
    keep_aliver.attach(&pls_keep_alive,300ms);
    for(int i=0;i<4;i++) {motor[i][0]->period_us(900);motor[i][1]->period_us(900);}
    sound_th.start(&sound_loop);
    while (1) {
        if(power and !stop) for(int i=0;i<4;i++) {
            if(duty[i]>0) {motor[i][0]->write(abs(duty[i])); motor[i][1]->write(0);}
            else {motor[i][0]->write(0); motor[i][1]->write(abs(duty[i]));}        
        }
        else if(!stop) for(int i=0;i<4;i++) {zero();}
        ThisThread::sleep_for(10ms);
        get_cjk();
        led1=!led1;
        if(debugger) echo();
    }
    mros2::spin();
    return 0;
}


void calculate_duty(geometry_msgs::msg::Twist *twist){
    is_alive=true;
    x=twist->linear.x;
    y=twist->linear.y;
    angle=twist->angular.z;
    out=p*fix(now-goal);
    if(out>0.5) out=0.5;
    else if(out<-0.5) out=-0.5;
    duty[0]= (x - y + angle)/unko+out;
    duty[1]= (x + y - angle)/unko-out;
    duty[2]= (x - y - angle)/unko-out;
    duty[3]= (x + y + angle)/unko+out;
}


void zero(void){
    for(int i=0;i<4;i++){
        motor[i][0]->write(0.0);
        motor[i][1]->write(0.0);
    }
}


void cmd_callback(std_msgs::msg::String *msg){
    led2=!led2;
    string cmd=msg->data;
    vector<string> cmds=split(cmd.c_str(),' ');
    printf("%s\n",cmd.c_str());
    strswitch(cmds[0].c_str())
    strcase("c")
        power=1;
    strcase("p")
        power=0;
    strcase("z")
        debugger=!debugger;
    // strstart("t")
    //     stop=true;
    //     printf("moter on:%d,%d = %f\n",stoi(cmds[1]),stoi(cmds[2]),stof(cmds[3]));
    //     motor[stoi(cmds[1])][stoi(cmds[2])]->write(stof(cmds[3]));
    //     ThisThread::sleep_for(3s);
    //     stop=false;
    strstart("unko")
        unko=stof(cmds[1]);
    strstart("g")
        goal=stof(cmds[1]);
    strend
    wav=cmd.c_str();
}


void pls_keep_alive(void){
    if(is_alive) {is_alive=false;}
    else {power=0;}
}


void get_cjk(void){
    cjk.setmode(OPERATION_MODE_NDOF);
    cjk.get_calib();
    cjk.get_angles();
    cjk.get_quat();
    now=cjk.euler.yaw;
    if(abs(angle)>0.01) ch=now-goal;
    now-=ch;
}


float fix(float r){
    while(r>180) r-=360;
    while(r<-180) r+=360;
    return r;
}


void echo(){
    // printf("duty: %0.3f,%0.3f,%0.3f,%0.3f\t",duty[0],duty[1],duty[2],duty[3]);
    // printf("x: %0.1f, y: %0.1f, a:%0.1f \t",x,y,angle);
    printf("cjk: %0.3f\t",cjk.euler.yaw);
    printf("now: %0.3f\t",now);
    printf("ch: %0.3f\t",ch);
    printf("out: %0.3f\t",out);
    printf("\n");
}


void sound_loop(){
    while(true){
        if(wav!=""){
            unsigned char *ptr;
            unsigned int len;
            uint32_t rate;
            rate=11000;
            if(wav=="c"){
                ptr=c_wav;
                len=c_wav_len;
            }else if(wav=="p"){
                ptr=p_wav;
                len=p_wav_len;
            }else if(wav=="t"){
                ptr=t_wav;
                len=t_wav_len;
            }else if(wav=="n"){
                ptr=n_wav;
                len=n_wav_len;
            }else{
                wav="";
                continue;
            }
            float wait=(1.0/rate)*1000*1000;
            printf("wait :%0.9f\n",wait);
            sound.period_us((int)wait);
            for(int i=0;i<len;i++){
                sound.write((float)*(ptr+i)/255.0);
                wait_us(40);
            }
            wav="";
        }
    }
}
