#!usr/bin/python
# -*- coding:UTF-8 -*-

import threading, queue, time, socket
import json
import struct


class Consumer(threading.Thread):
    def __init__(self, q: queue,
                 ip: str,
                 port: int):
        self.__q = q
        self.__s = socket.socket()
        self.__s.connect((ip, port))
        self.__s.sendall(b"TCP\r\n")
        self.__uuid = b""
        # start_signal += b"config_msg"
        # uuid = str(uuid)
        # start_signal = dict(signal="start", n_best=3, continuous_decoding=True, uuid=uuid)
        # start_json = json.dumps(start_signal, ensure_ascii=True)
        # print("start_signal is ", start_json)
        # start_bytes = bytes(start_signal, encoding="ascii")
        start_signal = b's'     # s: start e: end
        start_signal += b'1'  # n_best
        start_signal += b'0'  # continuous_decoding, True, False
        self.__uuid = self.__s.recv(36)
        start_signal += self.__uuid
        print("start_signal is ", start_signal)     # 39 = 1+36+1+1
        self.__s.sendall(start_signal)
        print("建立连接",)
        super().__init__()

    def run(self):
        # self.__s.setblocking(False)                     # nonblocking
        buf = b''
        while True:
            data = self.__q.get()                   # blocking
            if isinstance(data, str) and data=="quit":
                sendbuf = b'e10' + self.__uuid
                self.__s.sendall(sendbuf)      # end signal

                break
            self.__s.sendall(data)
            try:
                self.__s.settimeout(0.03)           # 无感
                buf = self.__s.recv(1024)
                print("buf is", buf)
            except Exception as e:
                print(e)
            # try:
            #     buf = self.__s.recv(8)
            #     print("buf is: ", buf)
            # except Exception as e:
            #     print(e)
            print("consumer数据处理中")

        # time.sleep(2)
        # for i in range(10):
        #     buf = self.__s.recv()  # 阻塞
        #     print(" ")
        #     print("buf is ",buf)
            # length = struct.unpack("!h", buf)
            # print(length)
            # buf = self.__s.recv(length[0])
            # print(buf)
        # time.sleep(10)          # 临时
        #print("buf is", buf)
        print("等待解码结果60秒钟")
        # time.sleep(30)
        # self.__s.setblocking(True)          # 因为是阻塞的，读到之后，就。。
        for i in range(1200):                 # 1200 = 60/0.05
            try:
                self.__s.settimeout(0.05)  # 无感
                buf = self.__s.recv(1024)
                print("buf is", buf)
            except Exception as e:
                print(e)
        print("识别结束，关闭socket")
        self.__s.close()

