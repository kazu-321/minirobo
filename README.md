# minirobo
全国ロボコン交流会のミニロボ競技会向けのロボット制御ソフトウェア

## 概要
ROS 2を用いて、ミニロボ競技会に参加するロボットの制御を行うためのソフトウェアパッケージです。

natto_libraryを使用しています

## 起動
```bash
ros2 launch minirobo_launch simulation.launch.xml
```

実機

```bash
ros2 launch minirobo_launch minirobo.launch.xml
```
