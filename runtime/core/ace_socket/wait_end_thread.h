#ifndef WAITENDTHREAD_H_
#define WAITENDTHREAD_H_

#include <chrono>
#include <thread>

#include "utils/log.h"

#include <mutex>

namespace wenet{

class Participant;

// 
// 解决阻塞问题，若handle_close()被Reactor调用时仍正在解码，则进入此队列等待。待解码完成，再次调用handle_close()
//
class WaitEndThread{
private:
    WaitEndThread(){}
    ~WaitEndThread(){}
    // WaitEndThread(const WaitEndThread&);
    // WaitEndThread& operator=(const WaitEndThread&);
public:
    static WaitEndThread& Instance(){
        static WaitEndThread instance;
        return instance;
    }
    void Update();
    void Add(Participant* cl);
private:
    void Check();
    std::set<Participant*> clients_;
    std::mutex mutex_;
};

} // namespace wenet

#endif /* WAITENDTHREAD_H_ */