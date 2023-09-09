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

#include "mbed.h"
#include "mros2.h"
#include "mros2-platform.h"
#include "geometry_msgs/msg/pose.hpp"


void userCallback(geometry_msgs::msg::Pose *msg)
{
  MROS2_INFO("subscribed Pose msg!!");
}

int main() {
  printf("%s start!\r\n", MROS2_PLATFORM_NAME);
  printf("app name: sub_pose\r\n");

  /* connect to the network */
  mros2_target::network_connect();

  mros2::init(0, NULL);
  MROS2_DEBUG("mROS 2 initialization is completed\r\n");

  mros2::Node node = mros2::Node::create_node("sub_pose");
  mros2::Subscriber sub = node.create_subscription<geometry_msgs::msg::Pose>("cmd_vel", 10, userCallback);
  osDelay(100);
  MROS2_INFO("ready to pub/sub message");

  mros2::spin();
  return 0;
}