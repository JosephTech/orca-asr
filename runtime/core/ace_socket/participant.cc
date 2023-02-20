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
    // std::string buffer(buf,rev);
    //printf("%s", request.c_str());
    // if(hub_ && 0 != hub_->ProcessRequest(buffer))
    // {
    //     ACE_ERROR_RETURN((LM_ERROR, "%p\n", "hub_.ProcessRequest()"), -1);
    // }
    
    
    return 0;
}

// int Participant::handle_input(ACE_HANDLE handle)
// {
//     //ACE_DEBUG((LM_DEBUG, ACE_TEXT("Participant::handle_input()被调用..\n")));
    
//     char buf[BUF_LEN_];
//     ssize_t rev = socket().recv(buf, sizeof(buf));
//     if(rev <= 0)
//     {
//         ACE_ERROR_RETURN((LM_ERROR, "%p\n", "sock_.recv()"), -1);
//     }
//     if(rev < BUF_LEN_){
//         buf[rev] = '\0';
//         printf("INFO recvlen: %lu, recv: %s\n",strlen(buf), buf);
//     }

//     //const char record_begin[9] = ;
//     if (true == on_pcm_)
//     {
//         assert(false == on_websocket_);
//         printf("INFO receving pcm data...%d \n", buf_idx_);
//         memcpy(pcm_buf_[buf_idx_++], buf, rev);
//         last_rev_ = rev;
//     }
//     else if (true == on_websocket_)
//     {
//         assert(false == on_pcm_);
//         printf("INFO receving websocket data...\n");
//     }
//     else{
//         char str[PROTOCOL_POS+1];
//         strncpy(str, buf, PROTOCOL_POS);

//         str[PROTOCOL_POS+1] = '\0';
//         printf("str is %s\n", str);
//         if (0 == strcmp(str, "suuiduuid"))
//         {
//             printf("开始录音，收到配置信息\n");
//             on_pcm_ = true;
//         }
//         else if(0  == strcmp(str, "euuiduuid"))
//         {
//             printf("结束录音\n");
//             on_pcm_ = false;
//         }
//         else
//         {
//             printf("使用websocket或者http服务\n");
//             on_websocket_  = true;
//         }
//     }

//     //ACE_DEBUG((LM_DEBUG, ACE_TEXT("收到..\n")));
//     //printf("%s", buf);
//     //ACE_DEBUG((LM_DEBUG, ACE_TEXT(buf)));
//     //ACE_DEBUG((LM_DEBUG, ACE_TEXT("\n")));
//     //sock_.send(buf,strlen(buf));
//     // ACE_DEBUG((LM_DEBUG, ACE_TEXT("收到..%C",buf)));
    
//     //ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s", buf)));
//     // 这里，需要把数据往队列 或者流水线里push，等待record把数据拼接成.pcm

//     return 0;
// }


// int Participant::SavePcmFile()
// {
//     ACE_DEBUG((LM_DEBUG, ACE_TEXT("Participant::storePcmData()被调用..\n")));
//     if (buf_idx_ < 1) return -1;

//     ofstream file;
//     file.open("./test.pcm", ios::binary | ios::out | ios::app);
//     for (unsigned int i = 0; i < buf_idx_-2; ++i){
//         for(unsigned int j = 0; j < MAX_BUF_LEN; ++j)
//         {
//             file << pcm_buf_[i][j];
//         }
//     }
//     // last receive line
//     for (unsigned int j = 0; j < last_rev_; ++j)
//     {
//         file << pcm_buf_[buf_idx_-1][j];
//     }

//     file.close();
//     return 0;
// }
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

    // SavePcmFile();
    // const std::string pcm_data= hub_.get_all_pcm_data_();
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

// void Participant::pass_configs(std::shared_ptr<FeaturePipelineConfig> feature_config,
//                                 std::shared_ptr<DecodeOptions> decode_config,
//                                 std::shared_ptr<DecodeResource> decode_resource)
// {
//     feature_config_ = std::move(feature_config);
//     decode_config_ = std::move(decode_config);
//     decode_resource_= std::move(decode_resource);
//     ACE_DEBUG((LM_DEBUG, ACE_TEXT("Participant::pass_configs feature_config_ use_count%d.\n"), feature_config_.use_count()));
//     ACE_DEBUG((LM_DEBUG, ACE_TEXT("Participant::pass_configs feature_config_ decode_config_%d.\n"), feature_config_.use_count()));
//     ACE_DEBUG((LM_DEBUG, ACE_TEXT("Participant::pass_configs feature_config_ decode_resource_%d.\n"), feature_config_.use_count()));
// }

} // namespace wenet

