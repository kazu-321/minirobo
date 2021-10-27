# mros2-mbed

mROS 2 (formally `mros2`) realizes a agent-less and lightweight runtime environment compatible with ROS 2 for embedded devices.
mROS 2 mainly offers pub/sub APIs compatible with [rclcpp](https://docs.ros2.org/dashing/api/rclcpp/index.html) for embedded devices.

mROS 2 consists of communication library for pub/sub APIs, RTPS protocol, UDP/IP stack, and real-time kernel.
This repository provides the reference implementation of mROS 2 that can be operated on the Mbed enabled board.
Please also check [mros2 repository](https://github.com/mROS-base/mros2) for more details and another implementations.

## Supported environment

- Mbed device
  - Board: Mbed enabled boards having an Ethernet port  
    For now, these boards below are confirmed to run the example on them.  
    - [STM32 NUCLEO-F429ZI](https://www.st.com/en/evaluation-tools/nucleo-f429zi.html)
    - [STM32 NUCLEO-F767ZI](https://www.st.com/en/evaluation-tools/nucleo-f767zi.html)

  - Kernel: [Mbed OS 6](https://github.com/ARMmbed/mbed-os)
- Host environment
  - [ROS 2 Dashing Diademata](https://docs.ros.org/en/dashing/index.html) on Ubuntu 18.04 LTS
  - [WiP] [ROS 2 Foxy Fitzroy](https://docs.ros.org/en/foxy/index.html) on Ubuntu 20.04 LTS

## Getting Starred
1. Prepare these items below.
- PC having an Ethernet port whose IP address is 192.168.11.x(x is not 2) and a docker environment.
- Mbed board having an Ethernet port(listed above).
2. Build Mbed executable binary using Mbed CLI2.
(Please replace F429ZI with F797ZI if you use F767ZI)
```
git clone https://github.com/mROS-base/mros2-mbed
cd mros2-mbed
docker run --rm -it --mount=type=bind,source="$(pwd)",destination=/var/mbed -w /var/mbed \
   ghcr.io/armmbed/mbed-os-env \
   /bin/bash -c "mbed-tools deploy && mbed-tools compile -m NUCLEO_F429ZI -t GCC_ARM"
```
After that, you will find an executable binary is created in the path below.
```
cmake_build/NUCLEO_F429ZI/develop/GCC_ARM/mros2-mbed.bin
```
3. Connect the PC and Mbed Board with USB and LAN cables.
4. Open Serial Console of the Mbed board. (9600bps)
5. Copy the executable binary above to the Mbed Board.
    (you may find it in the Nautilus file manager as NODE_F429ZI or F797ZI).
```
mbed mros2 start!
[MROS2LIB] mros2_init task start
mROS 2 initialization is completed
[MROS2LIB] create_node
[MROS2LIB] start creating participant
[MROS2LIB] successfully created participant
[MROS2LIB] create_publisher complete.
[MROS2LIB] create_subscription complete. data memory address=0x2001e170[MROS2LIB] Initilizing Domain complete

ready to pub/sub message
```
6. On the PC console, type the command below.
```
docker run --rm -it --net=host  ros:dashing /bin/bash -c  "source /opt/ros/dashing/setup.bash &&
       cd &&
       git clone https://github.com/mROS-base/mros2-host-examples &&
       cd mros2-host-examples &&
       colcon build && source install/setup.bash &&
       (ros2 run mros2_echoback sub_node &) &&
       echo 'wait for sub_node connection' &&
       sleep 10 &&
       ros2 run mros2_echoback pub_node"
```
Then, we can confirm the communication between the PC and Mbed board via ROS2.
```
Cloning into 'mros2-host-examples'...
remote: Enumerating objects: 35, done.
remote: Counting objects: 100% (35/35), done.
remote: Compressing objects: 100% (23/23), done.
remote: Total 35 (delta 14), reused 29 (delta 11), pack-reused 0
Unpacking objects: 100% (35/35), done.
Starting >>> mros2_echoback
Finished <<< mros2_echoback [8.14s]

Summary: 1 package finished [8.38s]
wait for sub_node connection
[INFO] [pub_mros2]: Publishing msg: 'Hello, world! 0'
[INFO] [mros2_sub]: Subscribed msg: 'Hello, world! 0'
[INFO] [pub_mros2]: Publishing msg: 'Hello, world! 1'
[INFO] [mros2_sub]: Subscribed msg: 'Hello, world! 1'
[INFO] [pub_mros2]: Publishing msg: 'Hello, world! 2'
[INFO] [mros2_sub]: Subscribed msg: 'Hello, world! 2'
...(SNIPPED)...
```

## Submodules and Licenses

The source code of this repository itself is published under [Apache License 2.0](https://github.com/mROS-base/mros2/blob/main/LICENSE).  
Please note that this repository contains the following stacks as the submodules, and also check their Licenses.

- [mros2](https://github.com/mROS-base/mros2): the pub/sub APIs compatible with ROS 2 Rclcpp
  - [embeddedRTPS](https://github.com/mROS-base/embeddedRTPS): RTPS communication layer (including lwIP and Micro-CDR)
- [Mbed OS 6](https://github.com/ARMmbed/mbed-os): an open source embedded operating system designed specifically for the "things" in the Internet of Things.
