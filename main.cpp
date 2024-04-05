// include here
#include "mbed.h"
#include "mros2.h"
#include "mros2-platform.h"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"

// 変数定義here
float x=0.0;        // 移動ベクトルのx成分(前:+)
float y=0.0;        // 移動ベクトルのy成分(左:+)
float angle=0.0;    // 角度(左回転:+)
float duty[4]={128,128,128,128};    // 最終的なduty 右前,左前,右後,左後
bool power=0;       // 0:停止 1:動く


// 回路定義here
/*
PwmOut m00();
PwmOut m01();
PwmOut m10();
PwmOut m11();
PwmOut m20();
PwmOut m21();
PwmOut m30();
PwmOut m31();
PwmOut* motor[4][2]={{&m00,&m01},{&m10,&m11},{&m20,&m21},{&m30,&m31}};
*/


// 関数定義here
void cmd_callback(std_msgs::msg::String *msg);
void calculate_duty(geometry_msgs::msg::Twist *twist);
void duty2pwm(void);


// プログラムhere
void calculate_duty(geometry_msgs::msg::Twist *twist){
    x=twist->linear.x;
    y=twist->linear.y;
    angle=twist->angular.z;
    duty[0]= x + y + angle;
    duty[1]= x - y - angle;
    duty[2]= x - y + angle;
    duty[3]= x + y - angle;
}


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

    while (1) {
        duty2pwm();
        if(power) for(int i=0;i<4;i++)
        else 
        ThisThread::sleep_for(10ms);
    }
    mros2::spin();
    return 0;
}
