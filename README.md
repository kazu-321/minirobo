# minirobo
全国ロボコン交流会のミニロボ競技会向けのロボット制御ソフトウェア

## 概要
ROS 2を用いて、ミニロボ競技会に参加するロボットの制御を行うためのソフトウェアパッケージです。

natto_library, sound_controller_ros2を使用しています

## 必要なパッケージ
srcディレクトリに以下のパッケージをクローンしてください。
### natto_library
```bash
git clone https://github.com/NITIC-Robot-Club/natto_library
```

### audio_common
```bash
git clone -b humble_hotfix https://github.com/teapfw/audio_common
```

### sound_controller_ros2
```bash
git clone https://github.com/kazu-321/sound_controller_ros2
```

## ビルド
srcと同じ階層で以下のコマンドを実行してください。
```bash
colcon build --symlink-install
source install/setup.bash
```


## 起動
```bash
ros2 launch minirobo_launch simulation.launch.xml
```

実機

```bash
ros2 launch minirobo_launch minirobo.launch.xml
```
