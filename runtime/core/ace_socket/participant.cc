#include "ace_socket/participant.h"
#include "ace_socket/recorder.h"

// #include "ace_socket/protocol_hub.h"

namespace wenet{

int Participant::open()
{
    hub_ = std::make_shared<ProtocolHub>(this, feature_config_, decode_config_, decode_resource_);
    // ACE_DEBUG((LM_DEBUG, ACE_TEXT("Participant::open() feature_config_ use_count%d.\n"), feature_config_.use_count()));
    // ACE_DEBUG((LM_DEBUG, ACE_TEXT("Participant::open() feature_config_ decode_config_%d.\n"), feature_config_.use_count()));
    // ACE_DEBUG((LM_DEBUG, ACE_TEXT("Participant::open() feature_config_ decode_resource_%d.\n"), feature_config_.use_count()));
    // ACE_DEBUG((LM_DEBUG, "注册READ事件处理器!!!!!!!!!!!\n"));
    return this->reactor ()->register_handler(this, ACE_Event_Handler::READ_MASK);
}

int Participant::handle_input(ACE_HANDLE handle)
{
    char buf[MAX_BUF_LEN];
    ssize_t rev = socket().recv(buf, sizeof(buf));
    if(rev <= 0)
    {
        ACE_ERROR_RETURN((LM_ERROR, "%p\n", "sock_.recv()"), -1);
    }
    //printf("receive buffer length: %ld\n",sizeof(buf));
    if(hub_ && 0 != hub_->ProcessRequest(buf, rev))
    {
        ACE_ERROR_RETURN((LM_ERROR, "%p\n", "hub_.ProcessRequest()"), -1);
    }
    
    return 0;
}

int Participant::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("Participant::handle_close()被调用..\n")));
    // Blocking prevention
    if (!decode_thread_finish_)
    {
        if(hub_)
        {
            hub_->HandleClose();
        }
        WaitEndThread::Instance().Add(this);
        return 0;
    }

    // hub_.get_recorder_().SavePcmFile(pcm_data);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("handle_close()这里阻塞，导致收不到事件LeaveGroup..\n")));
    GroupManager::Instance().LeaveGroup(uuid_, this);

    // hub_->OnSpeechEnd();
    PLOG(INFO) << "对端突然关闭,join等待解码结果";
    if(hub_ && hub_->get_decode_thread_())
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("handle_close()这里阻塞，导致收不到事件,join 线程..\n")));
        hub_->get_decode_thread_()->join();
    }
    
    if(sock_.get_handle() != ACE_INVALID_HANDLE)
    {
        // remove all events, close stream, delete this pointer
        ACE_Reactor_Mask m = ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL;
        reactor()->remove_handler(this, m);
        sock_.close();
        delete this;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("handle_close()这里阻塞，导致收不到事件..\n")));
    return 0;
}

} // namespace wenet

