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
5. The Group function allows one client to enter a voice and multiple clients to display the recognition result synchronously. It is suitable for large conferences with poor sound effect and network meeting for the hearing impaired.


## Run with Local Build
At this stage, this project relies on WeNet speech recognition engine. Entering this branch, [wenet-ace-socket](https://github.com/JosephTech/wenet-ace-socket), you can build and run.


## Release


## Client


## Web


## TODOs


## Final Project Goal



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
5. Group功能支持一个客户端输入语音，多客户端同步显示识别结果。适用于大型会议录音笔收音效果不佳，听障人士网络会议等场景。

## 本地构建运行
现阶段本项目依赖于WeNet语音识别引擎，进入此分支，[wenet-ace-socket](https://github.com/JosephTech/wenet-ace-socket)，即可构建运行。

1. 进入[文件夹](https://github.com/JosephTech/wenet-ace-socket/tree/main/runtime/libtorch)
2. 构建  
   `mkdir build && cd build && cmake .. && cmake --build .`
3. 运行  
    进入build/bin/文件夹
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

## 发布

## 客户端

## Web端

## TODOs

## 项目最终目标
