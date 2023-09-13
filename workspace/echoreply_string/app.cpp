/* mros2 example
 * Copyright (c) 2021 smorita_emb
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mros2.h"
#include "mros2-platform.h"
#include "std_msgs/msg/string.hpp"


mros2::Subscriber sub;
mros2::Publisher pub;

void userCallback(std_msgs::msg::String *msg)
{
  printf("subscribed msg: '%s'\r\n", msg->data.c_str());
  printf("publishing msg: '%s'\r\n", msg->data.c_str());
  pub.publish(*msg);
}

int main() {
  printf("%s start!\r\n", MROS2_PLATFORM_NAME);
  printf("app name: echoback_string\r\n");

  /* connect to the network */
  mros2_platform::network_connect();

  mros2::init(0, NULL);
  MROS2_DEBUG("mROS 2 initialization is completed\r\n");

  mros2::Node node = mros2::Node::create_node("mros2_node");
  pub = node.create_publisher<std_msgs::msg::String>("to_linux", 10);
  sub = node.create_subscription<std_msgs::msg::String>("to_stm", 10, userCallback);
  osDelay(100);
  MROS2_INFO("ready to pub/sub message\r\n");

  mros2::spin();
  return 0;
}