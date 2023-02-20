# Orca-ASR 虎鲸-自动语音识别

<div align=center>
    <img width="256" height="256" src="https://github.com/JosephTech/orca-asr/blob/main/images/dance_orca.bmp"/>
</div>
<p align="center">the image is generate by AI.  
 https://hotpot.ai </p>  



## Introduction
Orca ASR provides open source, full stack, speech recognition solutions. It supports server-side model deployment and provides client-side sample programs. ACE network middleware and wenet speech recognition engine are used to realize high concurrency server architecture based on Reactor network pattern. The simple network protocol is designed to support tcp long connection of clients, and the support of http protocol and websocket protocol is realized, compatible with wenet websocket protocol.

## Class Diagram

![class image](https://github.com/JosephTech/orca-asr/blob/main/images/class_diagram.bmp)


## Features
1. Pure c++ server to ensure that the server can run with high performance and stability.
2. Carefully designed to achieve high concurrency and non-blocking server.
3. Designed simple network protocol to support client TCP long connection.
4. The support for our own protocol, http protocol and websocket protocol is realized by using state design pattern. With only one server deployed, multiple types of clients highly concurrent/non-blocking connections can be supported.
5. The Group function allows one client to enter a voice and multiple clients to display the recognition result synchronously. It is suitable for large meetings where the sound effect of the recorder is poor, the network meeting of the hearing impaired, and the computer has no microphone.


## Run with Local Build
At present, this project relies on WeNet speech recognition engine. Entering this branch, [wenet-ace-socket](https://github.com/JosephTech/wenet-ace-socket), you can build and run.
1. Enter [Folder](https://github.com/JosephTech/wenet-ace-socket/tree/main/runtime/libtorch)
2. Build  
   `mkdir build && cd build && cmake .. && cmake --build .`
3. Run  
    cd ./build/bin/
```
    export GLOG_logtostderr=1
    export GLOG_v=2
    model_dir="/root/Model/20210815_unified_conformer_libtorch"
    ./ace_socket_server_main \
    --port 10010 \
    --chunk_size 16 \
    --model_path $model_dir/final.zip \
    --unit_path $model_dir/units.txt 2>&1 | tee server.log
```
## Client
![client_demo](https://github.com/JosephTech/orca-asr/blob/main/images/client_demo.gif)

## Web
Web is a slightly modified version of WeNet.   
![web_demo](https://github.com/JosephTech/orca-asr/blob/main/images/web_demo.gif)

## Multi-Terminal Synchronous Display

![multi_terminal](https://github.com/JosephTech/orca-asr/blob/main/images/multi_terminal_demo.gif)

## TODOs
1. Improve HTTP support.
2. Improve the support for WebSocket protocol.
3. Change to smart pointer.
4. Add a small program demo.


## Final Project Goal
1. Access ChatGPT.
2. Access TTS system.
3. Become the metaverse NPC server and the robot question answering system server.
4. The virtual and the real meet in this way.

## License






---
## 介绍
虎鲸ASR提供了开源的，全栈的，语音识别解决方案。它支持服务端模型部署，提供客户端示例程序。采用ACE网络中间件和wenet语音识别引擎，基于Reactor网络模式实现了高并发服务器架构。设计了简洁的网络协议以支持客户端tcp长连接，同时实现了对http协议和websocket协议的支持，兼容WeNet websocket协议。

## 类图

## 特点
1. 纯粹的c++服务器，确保服务器可以高性能稳定运行。
2. 精心设计实现了高并发和非阻塞服务器。
3. 设计简洁的网络协议,支持客户端TCP长连接.
4. 采用状态设计模式实现了对自有协议，http协议和websocket协议的支持。只需部署一个服务器，即可支持多种客户端高并发/非阻塞连接。
5. Group功能支持一个客户端输入语音，多客户端同步显示识别结果。适用于大型会议录音笔收音效果不佳，听障人士网络会议，电脑没有麦克风等场景。

## 本地构建运行
现阶段本项目依赖于WeNet语音识别引擎，进入此分支，[wenet-ace-socket](https://github.com/JosephTech/wenet-ace-socket)，即可构建运行。

1. 进入[文件夹](https://github.com/JosephTech/wenet-ace-socket/tree/main/runtime/libtorch)
2. 构建  
   `mkdir build && cd build && cmake .. && cmake --build .`
3. 运行  
    cd ./build/bin
```
    export GLOG_logtostderr=1
    export GLOG_v=2
    model_dir="/root/Model/20210815_unified_conformer_libtorch"
    ./ace_socket_server_main \
    --port 10010 \
    --chunk_size 16 \
    --model_path $model_dir/final.zip \
    --unit_path $model_dir/units.txt 2>&1 | tee server.log
```
## 客户端

![client_demo](https://github.com/JosephTech/orca-asr/blob/main/images/client_demo.gif)

## Web端
Web端是在WeNet的基础上稍作修改  
![web_demo](https://github.com/JosephTech/orca-asr/blob/main/images/web_demo.gif)

## 多端协同  
![multi_terminal](https://github.com/JosephTech/orca-asr/blob/main/images/multi_terminal_demo.gif)


## TODOs
1. 完善对HTTP协议的支持
2. 完善对WebSocket协议的支持
3. 改为智能指针
4. 添加小程序demo

## 项目最终目标
1. 接入ChatGPT
2. 接入语音合成系统
3. 成为元宇宙NPC服务器，同时成为机器人问答系统服务器
4. 虚拟和现实以此种方式交汇