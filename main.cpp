// include here
#include "ThisThread.h"
#include "mbed.h"
#include "mros2.h"
#include "mros2-platform.h"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "strswitch.h"

using namespace std;

// 変数定義here
float x=0.0;        // 移動ベクトルのx成分(前:+)
float y=0.0;        // 移動ベクトルのy成分(左:+)
float angle=0.0;    // 角度(左回転:+)
float duty[4]={0,0,0,0};    // 最終的なduty 右前,左前,左後,右後
bool power=0;       // 0:停止 1:動く
bool debugger=0;    // 0:off 1:on
bool stop=false;
bool is_alive=false;
float unko=2;

// 回路定義here
PwmOut m00(PC_8);
PwmOut m01(PC_7);
PwmOut m10(PA_6);
PwmOut m11(PA_5);
PwmOut m20(PC_9);
PwmOut m21(PD_14);
PwmOut m30(PE_9);
PwmOut m31(PD_15);
PwmOut* motor[4][2]={{&m00,&m01},{&m10,&m11},{&m20,&m21},{&m30,&m31}};
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

Ticker keep_aliver;


// 関数定義here
void cmd_callback(std_msgs::msg::String *msg);
void calculate_duty(geometry_msgs::msg::Twist *twist);
void duty2pwm(void);
void zero(void);
void pls_keep_alive(void);

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
    // mros2::Subscriber::callback_handler<std_msgs::msg::String>(rtps::)
    

    osDelay(100);
    MROS2_INFO("ready to pub/sub message\r\n---");
    keep_aliver.attach(&pls_keep_alive,50ms);
    for(int i=0;i<4;i++) {motor[i][0]->period_us(900);motor[i][1]->period_us(900);}
    while (1) {
        if(power and !stop) for(int i=0;i<4;i++) {
            if(duty[i]>0) {motor[i][0]->write(abs(duty[i])); motor[i][1]->write(0);}
            else {motor[i][0]->write(0); motor[i][1]->write(abs(duty[i]));}            
            led3=1;
        }
        else if(!stop) for(int i=0;i<4;i++) {zero();}
        ThisThread::sleep_for(10ms);
        led1=!led1;
    }
    mros2::spin();
    return 0;
}

//11 02
void calculate_duty(geometry_msgs::msg::Twist *twist){
    is_alive=true;
    x=twist->linear.x;
    y=twist->linear.y;
    angle=twist->angular.z;
    duty[0]= (x - y + angle)/unko;
    duty[1]= (x + y - angle)/unko;
    duty[2]= (x - y - angle)/unko;
    duty[3]= (x + y + angle)/unko;
    if(debugger) printf("duty: %f,%f,%f,%f\n",duty[0],duty[1],duty[2],duty[3]);
}

void zero(void){
    for(int i=0;i<4;i++){
        motor[i][0]->write(0.0);
        motor[i][1]->write(0.0);
    }
    led3=0;
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
    strstart("t")
        stop=true;
        printf("moter on:%d,%d = %f\n",stoi(cmds[1]),stoi(cmds[2]),stof(cmds[3]));
        motor[stoi(cmds[1])][stoi(cmds[2])]->write(stof(cmds[3]));
        ThisThread::sleep_for(3s);
        stop=false;
    strstart("unko")
        unko=stof(cmds[1]);
    strend
}

void pls_keep_alive(void){
    if(is_alive) {is_alive=false;}
    else power=0;
}