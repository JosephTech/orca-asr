#!usr/bin/python
# -*- conding: UTF-8 -*-
import pyaudio
import _thread
import socket
import yaml
import tkinter as tk
from recorder import recorder
from tkinter import NORMAL, DISABLED
import tkinter.messagebox
from PIL import ImageTk, Image
from typing import Callable

yamlPath = "./config.yaml"

f = open(yamlPath, 'r', encoding='utf-8')
# 字符串
cfg = f.read()
# 字典
configs = yaml.load(cfg, Loader = yaml.FullLoader)

CHANNELS = 1
RATE = 16000
CHUNK = 1024
FORMAT = pyaudio.paInt16
RECORDER_SECONDS = 180

IP = configs["IP"]
PORT = int(configs["PORT"])
print(IP, " ", PORT)

class BaseFace(object):
    def __init__(self, master):
        self.master = master
        self.master.config()
        self.master.title("Orca-ASR 虎鲸ASR")
        self.master.geometry("800x500")
        GroupFace(self.master, self.msg_box_callback)
    def msg_box_callback(self, type:str):
        if type == "group":
            tk.messagebox.showwarning(title="group wrong", message="Please check IP, port, uuid...")
            pass
        elif type == "uuid":
            tk.messagebox.showwarning(title="uuid wrong", message="Please check uuid...")
            pass
        elif type == "socket":
            tk.messagebox.showwarning(title="socket wrong", message="Please check IP, port...")
            pass
        elif type == "remote_colse":
            tk.messagebox.showwarning(title="socket wrong", message="Network error or remote connection close, please check uuid...")
            pass
        elif type == "mic":
            tk.messagebox.showwarning(title="mic wrong", message="Please click start button first and make sure you're in the group.")
            pass
        pass
    pass


class GroupFace(object):
    def __init__(self,
                 master: tk.Tk,
                 msg_box_callback: Callable):
        self.master = master
        self.msg_box_callback = msg_box_callback
        self.group_face = tk.Frame(self.master,
                                   height=500,
                                   width=800,
                                   bg="#F0FFF0")
        self.group_face.place(x=0, y=0)
        tip_lable = tk.Label(self.group_face,
                         font = ("consolas", 10),
                         text="Paste the 36-byte uuid here, or create a new group")
        tip_lable.place(x=420, y=150)

        self.entry = tk.Entry(self.group_face,
                              highlightcolor="red",
                              highlightthickness=1,
                              cursor="xterm",
                              font = "consolas",
                              width = 40)
        self.entry.place(x=420, y=200)

        join_btn = tk.Button(self.group_face,
                             text = "Join Group",
                             height=1)
        join_btn.place(x=450, y=300)
        join_btn.bind("<Button-1>", self.join_group_callback)
        new_group_btn = tk.Button(self.group_face,
                                  text = "New Group",
                                  height=1)
        new_group_btn.place(x=650, y=300)
        new_group_btn.bind("<Button-1>", self.new_group_callback)
        canv = tk.Canvas(self.group_face,
                         width=400,
                         height=500,
                         bg="#F0FFF0")
        canv.place(x=0, y=0)
        # img = tk.PhotoImage(file="img\dance_orca.png")
        img = ImageTk.PhotoImage(Image.open("img\dance_orca.bmp"))
        canv.create_image(72, 72, anchor="nw",image=img)

        head_label = tk.Label(self.group_face,
                              bg="#F0FFF0",
                              font=("consolas", 20),
                              text = "Orca-ASR 虎鲸ASR")
        head_label.place(x=80, y=20, anchor="nw")
        img_label = tk.Label(self.group_face,
                             bg="#F0FFF0",
                             font=("consolas", 10),
                             text="Orca should have a language system that\n humans may one day be able to communicate with them.")
        # img_label.place(x=0,y=0, height = 500, width = 400)
        img_label.place(x=0,y=380)
        url_label = tk.Label(self.group_face,
                             bg="#F0FFF0",
                             font=("consolas", 10),
                             text="The image is generated by AI.")
        url_label.place(x=80, y=460, anchor="nw")
        self.group_face.mainloop()      # show image

    def join_group_callback(self, eventobj):
        uuid = bytes(self.entry.get(), encoding="ascii")
        print("uuid is", uuid)
        #print("type is", type(uuid))
        print("len is", len(uuid))
        if len(uuid) != 36:
            self.msg_box_callback("uuid")
            print("please check, uuid need 36 bytes")
            return
        print("切换到Asr界面")
        self.group_face.destroy()
        AsrFace(self.master, self.msg_box_callback, uuid)
        pass

    def new_group_callback(self, eventobj):
        print("建立socket连接，发送")
        print("切换到Asr界面")
        self.group_face.destroy()
        AsrFace(self.master, self.msg_box_callback)
        pass
    pass


class AsrFace(object):
    def __init__(self,
                 master: tk.Tk,
                 msg_box_callback: Callable,
                 uuid=b""):
        self.master = master
        self.msg_box_callback = msg_box_callback
        self.join_group_flag = False
        if len(uuid) == 36:
            self.uuid = uuid
            self.join_group_flag = True
        self.peer = None
        self.asr_face = tk.Frame(self.master,
                                 height=500,
                                 width=800,
                                 bg="#F0FFF0")
        self.asr_face.place(x=0, y=0)
        tip_lable = tk.Label(self.asr_face,
                             font="consolas",
                             text = "ASR result: ",
                             bg = "#F0FFF0")
        tip_lable.place(x=2, y=2)
        self.time_lable = tk.Label(self.asr_face,
                             font="consolas",
                             bg="#F0FFF0")
        self.time_lable.place(x=390, y=2)
        self.uuid_text = tk.Text(self.asr_face,
                                font="consolas",
                                height = 1,
                                width=50,
                                bg="#F0FFF0")
        self.uuid_text.place(x=200, y=420)
        self.uuid_text.config(relief="flat")
        #self.uuid_text.config(state="disabled")

        self.text = tk.Text(self.asr_face,
                            cursor="xterm",
                            font="consolas",
                            height = 20,
                            width=86,
                            bg="#F0FFF7")
        self.text.place(x=10, y=32, anchor="nw")
        self.start_btn = tk.Button(self.asr_face,
                                   text="Start",
                                   width=10,
                                   height=1)
        self.start_btn.place(x=200, y=450)
        self.start_btn.bind("<Button-1>", self.start_callback)
        self.end_btn = tk.Button(self.asr_face,
                                 text="End",
                                 width=10,
                                 height=1)
        self.end_btn.place(x=500, y=450)
        self.end_btn.bind("<Button-1>", self.end_callback)
        self.mic_btn = tk.Button(self.asr_face,
                                 text = "Grab Mic",
                                 width = 10,
                                 height = 1)
        self.mic_btn.place(x=350, y=450)
        self.mic_btn.bind("<Button-1>", self.grab_microphone_callback)
        pass

    def start_callback(self, eventobj):
        is_leader = True
        if self.join_group_flag is True:
            is_leader = False
            try:
                self.peer = socket.socket()
                self.peer.connect((IP, PORT))
                buf = b"TCP" + self.uuid + b"\r\n"
                self.peer.sendall(buf)
            except Exception as e:
                print(e)
                self.msg_box_callback("group")
                return
            # 开始信号, 即使是group member, 开始之后，服务器才支持抢麦克风
            try:
                start_signal = b's'         # s: start e: end
                start_signal += b'1'        # n_best
                start_signal += b'1'        # continuous_decoding, True, False
                start_signal += self.uuid
                print("start_signal is ", start_signal)  # 39 = 1+36+1+1
                start_signal += b"\r\n"
                self.peer.sendall(start_signal)
            except Exception as e:
                print(e)
                self.msg_box_callback("socket")
        else:
            try:
                self.peer = socket.socket()
                # buf = b"TCP" + uuid + b"\r\n"
                self.peer.connect((IP, PORT))
                # 协议选择
                self.peer.settimeout(0.3)
                self.peer.sendall(b"TCP\r\n")
            except Exception as e:
                self.msg_box_callback("socket")
                return
            # 开始信号
            try:
                start_signal = b's'  # s: start e: end
                start_signal += b'1'  # n_best
                start_signal += b'1'  # continuous_decoding, True, False
                self.uuid = self.peer.recv(36)
                start_signal += self.uuid
                print("start_signal is ", start_signal)  # 39 = 1+36+1+1
                self.peer.sendall(start_signal)
            except Exception as e:
                print(e)
                self.msg_box_callback("socket")
        self.uuid_text.insert(1.0, "group uuid:" + bytes.decode(self.uuid))
        #print("按钮状态", start_btn["state"])
        if self.start_btn["state"] == "normal":
            self.start_btn["text"] = "recording..."
            self.start_btn["state"] = DISABLED
            _thread.start_new_thread(recorder,
                                     (CHANNELS,
                                      RATE,
                                      CHUNK,
                                      RECORDER_SECONDS,
                                      self.peer,
                                      self.start_btn,
                                      self.text,
                                      self.time_lable,
                                      self.uuid,
                                      self.msg_box_callback,
                                      is_leader))
        pass
    def grab_microphone_callback(self, eventobj):
        if self.start_btn["state"] == "normal":
            self.msg_box_callback("mic")
            return
        # 抢麦信号
        try:
            mic_signal =  b'o'  # s: start e: end
            mic_signal += b'1'  # n_best
            mic_signal += b'1'  # continuous_decoding, True, False
            mic_signal += self.uuid
            print("mic_signal is ", mic_signal)  # 39 = 1+36+1+1
            self.peer.sendall(mic_signal)
        except Exception as e:
            print(e)
            self.msg_box_callback("socket")
        pass

    def end_callback(self, eventobj):
        self.uuid_text.delete("1.0", "end")
        self.time_lable.config(text="")
        if self.start_btn["state"] == "disabled":
            #btn2["state"] = DISABLED
            # 如果解码完毕，关闭socket
            self.start_btn["text"] = "Start"
            # 可能需要加锁？
            print("TODO: 等待服务器解码完成，再切换按钮状态")
            self.start_btn["state"] = "normal"
        pass
    pass

if __name__ == "__main__":
    root = tk.Tk()
    BaseFace(root)
    root.mainloop()








