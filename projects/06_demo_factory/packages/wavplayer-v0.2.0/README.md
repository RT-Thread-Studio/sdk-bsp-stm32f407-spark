# wavplayer

## 1. 简介

**wavplayer** 是一个简易的 wav 格式的音乐播放器，提供播放和录制 wav 文件的功能，支持播放、停止、暂停、恢复，以及音量调节等功能。

### 1.1. 文件结构

| 文件夹 | 说明 |
| ---- | ---- |
| src  | 核心源码，主要实现 wav 播放和录音，以及导出 Finsh 命令行 |
| inc  | 头文件目录 |

### 1.2 许可证

wavplayer package 遵循 Apache 2.0 许可，详见 `LICENSE` 文件。

### 1.3 依赖

- RT-Thread 4.0+
- RT-Thread Audio 驱动框架
- optparse 命令行参数解析软件包

### 1.4 配置宏说明

```shell
 --- WavPlayer: Minimal music player for wav file play and record.   
 [*]   Enable support for play                                       
 (sound0) The play device name                                       
 [*]   Enable support for record                                     
 (sound0) The record device name                                      
       Version (v1.0.0)  --->  
```

**Enable support for play**：使能wav播放功能  
**The play device name**：指定播放使用的声卡设备，默认`sound0`  
**Enable support for record**：使能wav录音功能  
**The record device name**：指定录音使用的声卡设备，默认和播放一致，使用`sound0`  

## 2. 使用

wavplayer 的常用功能已经导出到 Finsh 命令行，以便开发者测试和使用。命令主要分为播放和录音两个类别，分别提供不同的功能。

**播放命令提供的功能如下 **

```shell
msh />wavplay -help
usage: wavplay [option] [target] ...

usage options:
  -h,     --help                     Print defined help message.
  -s URI, --start=URI                Play wav music with URI(local files).
  -t,     --stop                     Stop playing music.
  -p,     --pause                    Pause the music.
  -r,     --resume                   Resume the music.
  -v lvl, --volume=lvl               Change the volume(0~99).
  -d,     --dump                     Dump play relevant information.
```

**录音命令提供的功能如下**

```shell
msh />wavrecord -h
usage: wavrecord [option] [target] ...

usage options:
  -h,     --help                        Print defined help message.
  -s file --start=file  <samplerate> <channels> <samplebits> 
                                        record wav music to filesystem.
  -t,     --stop                        Stop record.
```

### 2.1 播放功能

- 开始播放

```shell
msh />
msh />wavplay -s song_44.wav
Information:
sampletate 44100
channels 2
sample bits width 16
[I/WAV_PLAYER] play start, uri=song_44.wav
```

- 停止播放

```shell
msh />wavplay -t
[I/WAV_PLAYER] play end
```

- 暂停播放

```shell
msh />
msh />wavplay -p
msh />
```

- 恢复播放

```shell
msh />
msh />wavplay -r
msh />
```

- 设置音量

```shell
msh />
msh />wavplay -v 88
msh />
```

### 2.2 录音功能

- 开始录音

```shell
msh />wavrecord -s test.wav
Information:
sampletate 8000
channels 2
```

- 停止录音

```shell
msh />
msh />wavrecord -t
msh />
```

## 3. 注意事项

- 仅支持采样位数为 16bit 的音频

## 4. 联系方式

- 维护：Zero-Free
- 主页：https://github.com/RT-Thread-packages/wavplayer
