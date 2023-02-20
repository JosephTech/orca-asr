#include "ace_socket/hub_state.h"
#include "ace_socket/protocol_hub.h"
#include "ace_socket/group.h"
#include "ace_socket/participant.h"


namespace wenet{

void FirstTimeConnect::Enter(const std::string& buffer)
{
    return;

}

void FirstTimeConnect::Execute(const std::string& buffer)
{
    PLOG(INFO) << "FirstTimeConnect::execute()";
    // 
    PLOG(INFO) << "TODO: FirstTimeConnect::execute()不是socket起始信号和配置, 就是http 请求";
    // start_signal: 's' means start, 'e' means end.
    // n_best: n best.
    // continuous_decoding: True 1, False 0. for long speech recognition.
    // uuid: clients with same uuid has the same group.
    // example: 's'+ '3' + '1'` + "ce25a119-fbe1-4c5b-a2ae-0e68d2477c5c" 
    if(buffer.find("HTTP") != std::string::npos)
    {
        // ph->connection_state_ = kOnHttpRequest;
        PLOG(INFO) << "TODO: 处理http请求 把请求传递给http server";
        // ph->states_machine_[ph->connection_state_]->Enter(ph, buffer);
        protocol_hub_->ChangeHubState(kOnHttpRequest, buffer);
    }
    else if (buffer.find("TCP") != std::string::npos)
    {
        int tcp_len = 3;
        int tcp_uuid_len = 3 + 36;
        int pos = buffer.find("\r\n");
        if(tcp_len == pos)
        {
            //std::string response = protocol_hub_->get_client_uuid_() + "\r\n";
            GroupManager::Instance().JoinNewGroup(protocol_hub_->get_client_());
            std::string response = protocol_hub_->get_client_()->get_uuid_() + "\r\n";
            // "TCP\r\n"  new group
            if(-1 == protocol_hub_->get_client_()->socket().send_n(response.c_str(), response.length()))
            {
                PLOG(ERROR) << "send uuid fail. close socket stream.";
                protocol_hub_->get_client_()->handle_close(ACE_INVALID_HANDLE, 0);
                return;     // important. access protocol_hub_ cause segment fault
            }
            protocol_hub_->ChangeHubState(kOnTcpReady, "");
        }
        else if(tcp_uuid_len == pos)
        {
            // "TCPuuid\r\n"  join group
            std::string uuid = buffer.substr(3, 36);
            if(-1 == GroupManager::Instance().JoinGroup(uuid, protocol_hub_->get_client_()))
            {
                PLOG(ERROR) << "uuid not exist. close socket stream.";
                protocol_hub_->get_client_()->handle_close(ACE_INVALID_HANDLE, 0);
                return;
            }
            //PLOG(INFO) << "buffer is " << buffer;
            std::string remain_buf = "";
            if(buffer.length() > pos+2)
            {
                //                          "\r\n"
                remain_buf = buffer.substr(pos+2);
            }
            protocol_hub_->ChangeHubState(kOnTcpReady, remain_buf);
        }
        else
        {
            PLOG(ERROR) << "protocol wrong. close socket stream.";
            protocol_hub_->get_client_()->handle_close(ACE_INVALID_HANDLE, 0);
            return;
        }
    }
    else
    {
        PLOG(ERROR) << "protocol wrong. close socket stream.";
        protocol_hub_->get_client_()->handle_close(ACE_INVALID_HANDLE, 0);
        return;
    }
    return;
}


void FirstTimeConnect::Exit()
{
    return;
}

// void FirstTimeConnect::OnSpeechStart(const std::string& config)
// {
//     PLOG(INFO) << "todo:  ProtocolHub::OnSpeechStart()此处需向客户端发送开始信息\n";
//     PLOG(INFO) << "todo: ProtocolHub::OnSpeechStart()此处需解析客户端配置信息\n";
//     PLOG(INFO) << "配置json is: " << config;

//     protocol_hub_->get_feature_pipeline_() = std::make_shared<FeaturePipeline>(*feature_config_);
//     decoder_ = std::make_shared<AsrDecoder>(protocol_hub_->get_feature_pipeline_(), decode_resource_, *decode_config_);
//     // on_socket_ = true;
//     // protocol_hub_->get_hub_state_() = on_pcm_data_state_;
//     protocol_hub_->ChangeHubState(kOnPcmData, config);
//     protocol_hub_->RunDecodeThread();
//     PLOG(INFO) << "todo:  ProtocolHub::OnSpeechStart()此处需join()线程\n";
//     // decode_thread_ = std::make_shared<std::thread>(&ProtocolHub::DecodeThreadFunc, protocol_hub_);
// }

// void FirstTimeConnect::PassConfigs(std::shared_ptr<FeaturePipelineConfig> feature_config,
//                                     std::shared_ptr<DecodeOptions> decode_config,
//                                     std::shared_ptr<DecodeResource> decode_resource)
// {
//     feature_config_ = std::move(feature_config);
//     decode_config_ = std::move(decode_config);
//     decode_resource_ = std::move(decode_resource);
// }

} // namespace wenet