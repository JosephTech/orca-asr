#include "ace_socket/participant_acceptor.h"
#include "ace_socket/participant.h"

namespace wenet{

ParticipantAcceptor::~ParticipantAcceptor(){
    handle_close(ACE_INVALID_HANDLE, 0);
}

int ParticipantAcceptor::open (const ACE_INET_Addr &listen_addr)
{
    if (this->acceptor_.open (listen_addr, 1) == -1)
    {
        printf("open port fail\n");
        return -1;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("register accept event handler...\n")));
    return this->reactor ()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);
}

int ParticipantAcceptor::handle_input (ACE_HANDLE fd )
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("ParticipantAcceptor::handle_input() be called.\n")));
    Participant *client = new Participant(feature_config_, decode_config_, decode_resource_);
    // client->pass_configs(feature_config_, decode_config_, decode_resource_);
    // 此处堆内存需要释放
    //auto_ptr<ClientService> p (client);
    // std::shared_ptr<Participant> client(new Participant(feature_config_, decode_config_, decode_resource_));

    if (this->acceptor_.accept (client->socket ()) == -1)
    {
        printf("accept client fail\n");
        return -1;
    }
    //p.release ();
    client->reactor (this->reactor ());
    if (client->open () == -1)
    {
        client->handle_close (ACE_INVALID_HANDLE, 0);
    }

    // if (!feature_config_)
    // {
    //     ACE_DEBUG((LM_DEBUG, ACE_TEXT("feature_config_ pointer error.\n"), feature_config_));
    // }
    // ACE_DEBUG((LM_DEBUG, ACE_TEXT("ParticipantAcceptor::handle_input() feature_config_ use_count%d.\n"), feature_config_.use_count()));
    // ACE_DEBUG((LM_DEBUG, ACE_TEXT("ParticipantAcceptor::handle_input() feature_config_ decode_config_%d.\n"), feature_config_.use_count()));
    // ACE_DEBUG((LM_DEBUG, ACE_TEXT("ParticipantAcceptor::handle_input() feature_config_ decode_resource_%d.\n"), feature_config_.use_count()));

    return 0;
}
// int ParticipantAcceptor::handle_input(ACE_HANDLE fd){
//     printf("ParticipantAcceptor::handle_input()被调用..\n");
//     //ACE_DEBUG((LM_CRITICAL, ACE_TEXT("ACCEPT, new client fking!!!!!!!!!!!\n")));

//     Participant* client = new Participant();

//     printf("accept等待客户端连接...\n");
//     // accept阻塞等待，等待客户端连接
//     if (acceptor_.accept(client->socket()) == -1){
//         printf("accept client failed");
//         return -1;
//     }

//     client->reactor(this->reactor());
//     if(client->open() == -1){
//         client->handle_close(ACE_INVALID_HANDLE, 0);
//         //ACE_ERROR_RETURN((LM_ERROR, "%p\n", "acceptor_.accept()"), -1);
//     }
    
//     // else{
//     //     通信成功，需要在particapant::handle_input()中处理业务
//     //     可以在此处加入聊天室等
//     // }
//     // 需要stream pool 存储所有clients, ParticipantAcceptor析构 的时候，要清理stream_pool
//     // 
//     // 这里可以加个decorder，每个decorder服务于一个client  从队列中获取  1024的pcm数据，进行处理（比如拼接成pcm文件）
//     // 
//     // （没必要）main函数那边，可以加个线程池，用于接受客户端的连接（acceptor），每个线程都run_event_loop()了，所以是每个线程都有一个reactor()
    
//     return 0;
// }

int ParticipantAcceptor::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask m){
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("ParticipantAcceptor::handle_close()被调用..\n")));
    if(acceptor_.get_handle() != ACE_INVALID_HANDLE){
        ACE_Reactor_Mask mask = ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL;

        reactor()->remove_handler(this, mask);
        acceptor_.close();
    }
    return 0;

}

// void ParticipantAcceptor::pass_configs(std::shared_ptr<FeaturePipelineConfig> feature_config,
//                                 std::shared_ptr<DecodeOptions> decode_config,
//                                 std::shared_ptr<DecodeResource> decode_resource)
// {
//     feature_config_ = std::move(feature_config);
//     decode_config_ = std::move(decode_config);
//     decode_resource_= std::move(decode_resource);
//     ACE_DEBUG((LM_DEBUG, ACE_TEXT("ParticipantAcceptor::pass_configs feature_config_ use_count%d.\n"), feature_config_.use_count()));
//     ACE_DEBUG((LM_DEBUG, ACE_TEXT("ParticipantAcceptor::pass_configs feature_config_ decode_config_%d.\n"), feature_config_.use_count()));
//     ACE_DEBUG((LM_DEBUG, ACE_TEXT("ParticipantAcceptor::pass_configs feature_config_ decode_resource_%d.\n"), feature_config_.use_count()));
// }

} // namespace wenet
