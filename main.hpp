// include here
#include "mbed.h"
#include "mros2.h"
#include "mros2-platform.h"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "strswitch.h"
#include <cstdint>
#include <cstdio>
#include <math.h>
#include <vector>


// using and define here
using namespace std;


// 変数定義here
float x=0.0;        // 移動ベクトルのx成分(前:+)
float y=0.0;        // 移動ベクトルのy成分(左:+)
float angle=0.0;    // 角度(左回転:+)
float duty[4]={0,0,0,0};    // 最終的なduty 右前,左前,左後,右後
float max_duty=0.7;
bool debugger=0;    // 0:off 1:on
bool stop=false;    // 一時停止
bool is_alive=false;// 生存確認 



// 回路(PIN)定義here
PwmOut m00(PC_8);
PwmOut m01(PC_7);
PwmOut m10(PA_6);
PwmOut m11(PA_5);
PwmOut m20(PC_9);
PwmOut m21(PD_14);
PwmOut m30(PE_9);
PwmOut m31(PD_15);
PwmOut sound(PF_9);
PwmOut* motor[4][2]={{&m00,&m01},{&m10,&m11},{&m20,&m21},{&m30,&m31}};
DigitalOut led1(LED1);  // main関数生存確認
DigitalOut led2(LED2);  // 通信確認
DigitalOut power(LED3); // 電源オンオフ&確認
Ticker keep_aliver;     // 割り込み


// 関数定義here
void cmd_callback(std_msgs::msg::String *msg);  // コマンド
void calculate_duty(geometry_msgs::msg::Twist *twist);  // 移動
void zero(void);    // すべてのモーターストップ
void pls_keep_alive(void);  // 通信生存確認
void echo();
template<typename T>
T minmax(T value,T minv,T maxv);
