#!usr/bin/python
# -*- conding: UTF-8 -*-
import pyaudio
import _thread
import tkinter as tk
from recorder import recorder
from tkinter import NORMAL, DISABLED

CHANNELS = 1
RATE = 16000
CHUNK = 1024
FORMAT = pyaudio.paInt16
RECORDER_SECONDS = 15

IP = "192.168.2.123"
PORT = 10010

root = tk.Tk()
root.geometry("300x200")
start_btn = tk.Button(root, text="开始", height=2)
start_btn.pack()
end_btn = tk.Button(root, text="停止", height=2)
end_btn.pack()

def startCallback(eventobj):
    #print("按钮状态", start_btn["state"])
    if start_btn["state"] == "normal":
        start_btn["text"] = "正在录音..."
        start_btn["state"] = DISABLED
        # print("按钮状态", start_btn["state"])
        # print("地址", id(start_btn["state"]))
        # print("类型", type(start_btn["state"]))
        # print("真假", start_btn["state"] is "disabled")
        # rec["state"] = True
        #print("地址是 ", id(rec))
        _thread.start_new_thread(recorder, (CHANNELS, RATE, CHUNK,RECORDER_SECONDS, IP, PORT, start_btn))
    pass

def endCallback(eventobj):
    if start_btn["state"] == "disabled":
        #btn2["state"] = DISABLED
        # 如果解码完毕，关闭socket
        start_btn["text"] = "开始"
        # 可能需要加锁？
        print("TODO: 等待服务器解码完成，再切换按钮状态")
        start_btn["state"] = "normal"
    pass

start_btn.bind("<Button-1>", startCallback)
# btn1.bind("<ButtonRelease-1>",leftRelease)
end_btn.bind("<Button-1>", endCallback)

if __name__ == "__main__":

    root.mainloop()








