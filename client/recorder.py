import pyaudio, queue
from consumer import Consumer
from tkinter import Button

def recorder(channels: int,
             rate: int,
             chunk: int,
             record_seconds: int,
             ip: str,
             port: int,
             btn: Button):
    # 播放器
    pa = pyaudio.PyAudio()
    stream = pa.open(format=pyaudio.paInt16,
                     channels=channels,
                     rate=rate,
                     input=True,
                     frames_per_buffer=chunk)

    q = queue.Queue()
    worker = Consumer(q, ip, port)
    worker.start()

    # print("状态是: ", rec["state"])
    # print("地址是: ", id(rec))
    print("按钮状态是:", btn["state"])
    for i in range(int(record_seconds * rate / chunk)):
        data = stream.read(chunk)
        q.put(data)
        print("传输第%d个数据块" % i)
        if btn["state"] == "normal":
            break
    q.put("quit")
    print("此处需要等待识别结果")
    worker.join()
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