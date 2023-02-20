import pyaudio
import queue
import socket
from consumer import Consumer
from tkinter import Button, Text, Label
from typing import Callable

def recorder(channels: int,
             rate: int,
             chunk: int,
             record_seconds: int,
             peer: socket.socket,
             start_btn: Button,
             text: Text,
             time_label: Label,
             uuid: bytes,
             msg_box_callback: Callable,
             is_leader: bool):
    # 播放器
    pa = pyaudio.PyAudio()
    stream = pa.open(format=pyaudio.paInt16,
                     channels=channels,
                     rate=rate,
                     input=True,
                     frames_per_buffer=chunk)

    q = queue.Queue()
    worker = Consumer(q, peer, start_btn, text, uuid, msg_box_callback, is_leader)
    worker.start()

    # print("状态是: ", rec["state"])
    # print("地址是: ", id(rec))
    print("按钮状态是:", start_btn["state"])
    for i in range(int(record_seconds * rate / chunk)):
        data = stream.read(chunk)
        q.put(data)
        # print("传输第%d个数据块" % i)
        time_label.config(text = "%dS" % int(record_seconds - i*chunk/rate))
        if start_btn["state"] == "normal":
            time_label.config(text=" ")
            break
    q.put("quit")
    time_label.config(text="waitting...")
    print("此处需要等待识别结果")
    worker.join()
    time_label.config(text=" ")
    stream.stop_stream()
    stream.close()
    pa.terminate()
    print("recorder数据处理完成")

    # with open("rec.pcm", 'wb') as f:
    #     f.write(b''.join(frames))
    #     f.close()


if __name__ == "__main__":
    pass
    #recorder(CHANNELS, RATE, FORMAT)