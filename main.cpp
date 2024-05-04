#include "main.hpp"


int main(){
    MROS2_INFO("start");
    if (mros2_platform::network_connect()) {            // connect to the network
        MROS2_ERROR("ネットつながってないにょ...");
        return -1;
    } else {
        MROS2_INFO("ネットつながた！---");
    }


    MROS2_INFO("mini robot起動します");
    mros2::init(0, NULL);       // 初期化
    MROS2_DEBUG("mROS 2初期化完了");

    mros2::Node node = mros2::Node::create_node("mros2_node");  // node生成
    mros2::Subscriber sub_cmd = node.create_subscription<std_msgs::msg::String>("cmd", 10, cmd_callback);   // コマンド入力
    mros2::Subscriber sub_vel = node.create_subscription<geometry_msgs::msg::Twist>("cmd_vel", 10, calculate_duty);     // 移動ベクトル

    osDelay(100);
    MROS2_INFO("送受信準備完了！");
    keep_aliver.attach(&pls_keep_alive,300ms);
    for(int i=0;i<4;i++) {motor[i][0]->period_us(900);motor[i][1]->period_us(900);}

    while (1) {
        if(power and !stop) for(int i=0;i<4;i++) {
            if(duty[i]>0) {motor[i][0]->write(abs(duty[i])); motor[i][1]->write(0);}
            else {motor[i][0]->write(0); motor[i][1]->write(abs(duty[i]));}        
        }
        else if(!stop) for(int i=0;i<4;i++) {zero();}
        ThisThread::sleep_for(10ms);
        led1=!led1;
        if(debugger) echo();
    }
    mros2::spin();
    return 0;
}


void calculate_duty(geometry_msgs::msg::Twist *twist){
    is_alive=true;
    x=twist->linear.x/2;
    y=twist->linear.y;
    angle=twist->angular.z;
    duty[0]=minmax( (float)((x - y + angle)/2) , -max_duty , max_duty);
    duty[1]=minmax( (float)((x + y - angle)/2) , -max_duty , max_duty);
    duty[2]=minmax( (float)((x - y - angle)/2) , -max_duty , max_duty);
    duty[3]=minmax( (float)((x + y + angle)/2) , -max_duty , max_duty);
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
    strstart("test")
        stop=true;
        printf("moter on:%d,%d = %f\n",stoi(cmds[1]),stoi(cmds[2]),stof(cmds[3]));
        motor[stoi(cmds[1])][stoi(cmds[2])]->write(stof(cmds[3]));
        ThisThread::sleep_for(3s);
        stop=false;
    strend
}


void pls_keep_alive(void){
    if(is_alive) {is_alive=false;}
    else {power=0;}
}


void echo(){
    printf("duty: %0.3f,%0.3f,%0.3f,%0.3f\t",duty[0],duty[1],duty[2],duty[3]);
    // printf("x: %0.1f, y: %0.1f, a:%0.1f \t",x,y,angle);
    // printf("cjk: %0.3f\t",cjk.euler.yaw);
    // printf("now: %0.3f\t",now);
    // printf("ch: %0.3f\t",ch);
    // printf("out: %0.3f\t",out);
    printf("\n");
}


template<typename T>
T minmax(T value,T minv,T maxv){ return min(max(value,minv),maxv); }
