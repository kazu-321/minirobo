# mros2-mbed

mROS 2 (formally `mros2`) realizes a agent-less and lightweight runtime environment compatible with ROS 2 for embedded devices.
mROS 2 mainly offers pub/sub APIs compatible with [rclcpp](https://docs.ros2.org/foxy/api/rclcpp/index.html) for embedded devices.

mROS 2 consists of communication library for pub/sub APIs, RTPS protocol, UDP/IP stack, and real-time kernel.
This repository provides the reference implementation of mROS 2 that can be operated on the Mbed enabled board.
Please also check [mros2 repository](https://github.com/mROS-base/mros2) for more details and another implementations.

## Supported environment

- Mbed device
  - Board: Mbed enabled boards having an Ethernet port  
    For now, these boards below are confirmed to run the example on them.  
    - [STM32 NUCLEO-F429ZI](https://www.st.com/en/evaluation-tools/nucleo-f429zi.html)
    - [STM32 NUCLEO-F767ZI](https://www.st.com/en/evaluation-tools/nucleo-f767zi.html)
    - [Seeed Arch Max V1.1](https://wiki.seeedstudio.com/Arch_Max_v1.1/)
  - Kernel: [Mbed OS 6](https://github.com/ARMmbed/mbed-os)
- Host environment
  - [ROS 2 Foxy Fitzroy](https://docs.ros.org/en/foxy/index.html) on Ubuntu 20.04 LTS
  - [ROS 2 Dashing Diademata](https://docs.ros.org/en/dashing/index.html) on Ubuntu 18.04 LTS

## Getting Starred
1. Prepare these items below.
- PC having an Ethernet port whose IP address is 192.168.11.x(x is not 2) and a docker environment.
- Mbed board having an Ethernet port(listed above).
2. Build Mbed executable binary using Mbed CLI2.
```
git clone https://github.com/mROS-base/mros2-mbed
cd mros2-mbed
#(Please replace the [TARGET] with the ones as below.)
# +-------------------+---------------+
# | Your target board | [TARGET]      |
# +-------------------+---------------+
# | NUCLEO-F429ZI     | NUCLEO_F429ZI |
# | NUCLEO-F767ZI     | NUCLEO_F767ZI |
# | Arch Max v1.1     | ARCH_MAX      |
# +-------------------+---------------+
docker run --rm -it --mount=type=bind,source="$(pwd)",destination=/var/mbed -w /var/mbed \
  ghcr.io/armmbed/mbed-os-env \
  /bin/bash -c "mbed-tools deploy && mbed-tools compile -m [TARGET] -t GCC_ARM"
```
After that, you will find an executable binary is created in the path below.
```
cmake_build/[TARGET]/develop/GCC_ARM/mros2-mbed.bin
```
3. Connect the PC and Mbed Board with USB and LAN cables.
4. Open Serial Console of the Mbed board. (115200bps)
5. Copy the executable binary above to the Mbed Board.
   (you may find it in the Nautilus file manager as NODE_F429ZI, F767ZI or DAPLINK.)
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
docker run --rm -it --net=host ros:foxy /bin/bash \
  -c "source /opt/ros/foxy/setup.bash &&
  cd &&
  git clone https://github.com/mROS-base/mros2-host-examples &&
  cd mros2-host-examples &&
  colcon build --packages-select mros2_echoback_string &&
  source install/setup.bash &&
  (ros2 run mros2_echoback_string sub_node &) &&
  echo 'wait for sub_node connection' &&
  sleep 10 &&
  ros2 run mros2_echoback_string pub_node"
```
Then, we can confirm the communication between the PC and Mbed board via ROS2.
```
Cloning into 'mros2-host-examples'...
remote: Enumerating objects: 432, done.
remote: Counting objects: 100% (432/432), done.
remote: Compressing objects: 100% (272/272), done.
remote: Total 432 (delta 237), reused 316 (delta 127), pack-reused 0
Receiving objects: 100% (432/432), 42.50 KiB | 10.63 MiB/s, done.
Resolving deltas: 100% (237/237), done.
Starting >>> mros2_echoback_string
Finished <<< mros2_echoback_string [5.28s]                     

Summary: 1 package finished [5.39s]
wait for sub_node connection
[INFO] [pub_mros2]: Publishing msg: 'Hello, world! 0'
[INFO] [mros2_sub]: Subscribed msg: 'Hello, world! 0'
[INFO] [pub_mros2]: Publishing msg: 'Hello, world! 1'
[INFO] [mros2_sub]: Subscribed msg: 'Hello, world! 1'
[INFO] [pub_mros2]: Publishing msg: 'Hello, world! 2'
[INFO] [mros2_sub]: Subscribed msg: 'Hello, world! 2'
...(SNIPPED)...
```

## Examples

This repository contains some example applications in `examples/` to communicate with ROS 2 nodes on the host.
Please also check [mROS-base/mros2-host-examples](https://github.com/mROS-base/mros2-host-examples) repository for more detail about the host examples.

You can change the example by editing [`add_executable` in CMakeLists.txt](https://github.com/mROS-base/mros2-mbed/blob/main/CMakeLists.txt).
Of course you can also create a new program file and specify it as your own application.

### echoreply_string (default)

- Description:
  - The mROS 2 node on the embedded board subscribes `string` (`std_msgs::msg::String`) message from `/to_stm` topic.
  - And then publishes this `string` message as it is to `/to_linux` as the reply.
- Host operation:
  - `$ ros2 launch mros2_echoback_string launch.py`
  - or, at two terminals:
    - `$ ros2 run mros2_echoback_string pub_node`
    - `$ ros2 run mros2_echoback_string sub_node`

### echoreply_uint16

- Description:
  - The mROS 2 node on the embedded board subscribes `uint16` (`std_msgs::msg::UInt16`) message from `/to_stm` topic.
  - And then publishes this `uint16` message as it is to `/to_linux` as the reply.
- Host operation:
  - `$ ros2 launch mros2_echoback_uint16 launch.py`
  - or, at two terminals:
    - `$ ros2 run mros2_echoback_uint16 pub_node`
    - `$ ros2 run mros2_echoback_uint16 sub_node`

### echoreply_float32

- Description:
  - The mROS 2 node on the embedded board subscribes `float32` (`std_msgs::msg::Float32`) message from `/to_stm` topic.
  - And then publishes this `float32` message as it is to `/to_linux` as the reply.
    - Note that this application just print whether the value of message is less than 0.0, between 0.0 and 1.0, or greater than 1.0.
    - If you want to print float value in serial console, you need to add `"target.printf_lib": "std"` into mbed_app.json (see [detail](https://forums.mbed.com/t/float-printf-doesnt-work-in-desktop-version/9164)). Note that linking std lib will increase the size of Flash memory.
- Host operation:
  - `$ ros2 launch mros2_echoback_float32 launch.py`
  - or, at two terminals:
    - `$ ros2 run mros2_echoback_float32 pub_node`
    - `$ ros2 run mros2_echoback_float32 sub_node`


## Submodules and Licenses

The source code of this repository itself is published under [Apache License 2.0](https://github.com/mROS-base/mros2/blob/main/LICENSE).  
Please note that this repository contains the following stacks as the submodules, and also check their Licenses.

- [mros2](https://github.com/mROS-base/mros2): the pub/sub APIs compatible with ROS 2 Rclcpp
  - [embeddedRTPS](https://github.com/mROS-base/embeddedRTPS): RTPS communication layer (including lwIP and Micro-CDR)
- [Mbed OS 6](https://github.com/ARMmbed/mbed-os): an open source embedded operating system designed specifically for the "things" in the Internet of Things.

