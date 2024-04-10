// include here
#include "mbed.h"
#include "mros2.h"
#include "mros2-platform.h"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "strswitch.h"

// 変数定義here
float x=0.0;        // 移動ベクトルのx成分(前:+)
float y=0.0;        // 移動ベクトルのy成分(左:+)
float angle=0.0;    // 角度(左回転:+)
float duty[4]={0,0,0,0};    // 最終的なduty 右前,左前,右後,左後
bool power=0;       // 0:停止 1:動く
bool debugger=0;    // 0:off 1:on

// 回路定義here
PwmOut m00(PA_6);
PwmOut m01(PA_5);
PwmOut m10(PC_8);
PwmOut m11(PC_7);
PwmOut m20(PC_9);
PwmOut m21(PD_14);
PwmOut m30(PD_15);
PwmOut m31(PE_9);
PwmOut* motor[4][2]={{&m00,&m01},{&m10,&m11},{&m20,&m21},{&m30,&m31}};



// 関数定義here
void cmd_callback(std_msgs::msg::String *msg);
void calculate_duty(geometry_msgs::msg::Twist *twist);
void duty2pwm(void);


// プログラムhere
int main(){
    if (mros2_platform::network_connect()) {            // connect to the network
        MROS2_ERROR("failed to connect and setup network! aborting,,,");
        return -1;
    } else {
        MROS2_INFO("successfully connect and setup network\r\n---");
    }

    MROS2_INFO("app name: mini robot");

    mros2::init(0, NULL);       // 初期化
    MROS2_DEBUG("mROS 2 initialization is completed");

    mros2::Node node = mros2::Node::create_node("mros2_node");  // node生成
    mros2::Subscriber sub_cmd = node.create_subscription<std_msgs::msg::String>("cmd", 10, cmd_callback);   // コマンド入力
    mros2::Subscriber sub_vel = node.create_subscription<geometry_msgs::msg::Twist>("cmd_vel", 10, calculate_duty);     // 移動ベクトル

    osDelay(100);
    MROS2_INFO("ready to pub/sub message\r\n---");

    for(int i=0;i<4;i++) {motor[i][0]->period_ms(1);motor[i][1]->period_ms(1);}
    while (1) {
        if(power) for(int i=0;i<4;i++) {
            if(duty[i]>0) {motor[i][0]->write(abs(duty[i])); motor[i][1]->write(0);}
            else {motor[i][0]->write(0); motor[i][1]->write(abs(duty[i]));}
        }
        else for(int i=0;i<4;i++) {motor[i][0]->write(0); motor[i][1]->write(0);}
        ThisThread::sleep_for(10ms);
    }
    mros2::spin();
    return 0;
}


void calculate_duty(geometry_msgs::msg::Twist *twist){
    x=twist->linear.x;
    y=twist->linear.y;
    angle=twist->angular.z;
    duty[0]= (x + y + angle)/3;
    duty[1]= (x - y - angle)/3;
    duty[2]= (x - y + angle)/3;
    duty[3]= (x + y - angle)/3;
    if(debugger) printf("duty: %f,%f,%f,%f\n",duty[0],duty[1],duty[2],duty[3]);
}


void cmd_callback(std_msgs::msg::String *msg){
    printf("%s\n",msg->data.c_str());
    strswitch(msg->data.c_str())
    strcase("c")
        power=1;
    strcase("p")
        power=0;
    strcase("z")
        debugger=!debugger;
    strend
}