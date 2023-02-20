#!usr/bin/python
# -*- coding:UTF-8 -*-

import threading
import queue
import time
import socket
import json
import struct
from tkinter import Text, Button, END
from typing import Callable


class Consumer(threading.Thread):
    def __init__(self,
                 q: queue,
                 peer: socket.socket,
                 start_btn: Button,
                 text: Text,
                 uuid: bytes,
                 msg_box_callback: Callable,
                 is_leader=True):
        self.end_flag = False
        self.quit_flag = False
        self.q = q
        self.peer = peer
        self.text = text
        self.uuid = uuid
        self.msg_box_callback = msg_box_callback
        self.is_leader = is_leader
        self.particial_len = 0
        self.start_btn = start_btn
        print("开始识别",)
        super().__init__()

    def parse_result(self, buf: bytes):
        # print("buf is", buf)
        if len(buf) < 0:
            return
        print("TODO(Joseph): 解决TCP粘包 拆包问题")
        li = buf.split(b"\r\n")
        for item in li:
            if len(item) > 0:
                item_str = item.decode(encoding="utf-8")
                # print(item_str)
                result = json.loads(item_str)
                print(result)
                if result["status"] == "ok":
                    if result["type"] == "partial_result":
                        # print(result["nbest"])
                        # print(type(result["nbest"]))
                        nb = json.loads(result["nbest"].strip('[').strip(']'))
                        # print(nb)
                        # print(type(nb))
                        print(nb["sentence"])
                        print("需要清理后上屏")
                        # self.text.delete(self.end_index - self.particial_len, self.end_index)
                        # self.end_index -= self.particial_len
                        # self.text.insert(self.end_index, nb["sentence"])
                        for i in range(self.particial_len):
                            self.text.delete("end-2chars", END)
                        self.text.insert(END, nb["sentence"])
                        self.particial_len = len(nb["sentence"])
                        # self.end_index += self.particial_len
                        print("particial_len", self.particial_len)
                    elif result["type"] == "final_result":
                        nb = json.loads(result["nbest"].strip('[').strip(']'))
                        print(nb["sentence"])
                        print("需要清理后上屏")
                        # self.particial_len = len(nb["sentence"])
                        # self.text.delete(self.end_index - self.particial_len, self.end_index)
                        # self.end_index -= self.particial_len
                        # self.text.insert(self.end_index, nb["sentence"])
                        # self.end_index += float(len(nb["sentence"]))
                        for i in range(self.particial_len):
                            self.text.delete("end-2chars", END)
                        self.text.insert(END, nb["sentence"])
                        if ",\n" != self.text.get("end-2chars", END):
                            self.text.insert(END, ",")
                        self.particial_len = 0
                        print("particial_len", self.particial_len)
                    elif result["type"] == "speech_end":
                        self.end_flag = True
        pass

    def run(self):
        # self.__s.setblocking(False)                     # nonblocking
        buf = b''
        if self.is_leader is True:
            while True:
                data = self.q.get()                   # blocking
                if isinstance(data, str) and data=="quit":
                    sendbuf = b'e10' + self.uuid
                    self.peer.sendall(sendbuf)      # end signal
                    self.quit_flag = True
                    break
                elif self.quit_flag is False:
                    try:
                        self.peer.sendall(data)
                    except Exception as e:
                        print(e)
                        self.msg_box_callback("remote_colse")
                        break
                try:
                    self.peer.settimeout(0.03)           # 客户端无感
                    buf = self.peer.recv(4096)
                    self.parse_result(buf)
                except Exception as e:
                    # print(e)
                    pass
                # print("consumer数据处理中")
        for i in range(3600):                 # 3600 = 180s/0.05
            if self.end_flag is True:
                break
            try:
                self.peer.settimeout(0.05)  # 客户端无感
                buf = self.peer.recv(4096)
                if len(buf) > 0:
                    self.parse_result(buf)
                    # print(buf)
            except Exception as e:
                # print(e)
                # print("如果是超时跳过，如果是其它，就设置self.end_flag")
                pass
        print("识别结束，关闭socket")
        self.peer.close()
        # 切换按钮状态
        if self.start_btn["state"] == "disabled":
            self.start_btn["text"] = "Start"
            self.start_btn["state"] = "normal"

