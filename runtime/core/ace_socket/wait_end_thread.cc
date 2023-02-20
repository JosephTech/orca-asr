#include "ace_socket/participant.h"
#include "ace_socket/wait_end_thread.h"


namespace wenet{

void WaitEndThread::Update()
{
    while(true)
    {
        Check();
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    
}

void WaitEndThread::Add(Participant* cl)
{
    std::lock_guard<std::mutex> guard(mutex_);
    clients_.insert(cl); 
    PLOG(INFO) << "wait_end_threads add new client. wait_end_threads.size():" << clients_.size();
}

void WaitEndThread::Check()
{
    //PLOG(INFO) << "Check() wait_end_threads.size():" << clients_.size();
    std::lock_guard<std::mutex> guard(mutex_);
    for(auto cl : clients_)
    {
        if(cl->is_decode_thread_finish_())
        {
            PLOG(INFO) << "wait_end_threads remove client" << cl;
            cl->handle_close(ACE_INVALID_HANDLE, 0);
            clients_.erase(cl);
            PLOG(INFO) << "wait_end_threads.size():" << clients_.size();
        }
    }
}

} // namespace wenet