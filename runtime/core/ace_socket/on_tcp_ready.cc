#include "ace_socket/hub_state.h"
#include "ace_socket/protocol_hub.h"
#include "ace_socket/group.h"
#include "ace_socket/participant.h"

namespace wenet{

void OnTcpReady::Execute(const std::string& buffer)
{
    int head_len = 3 + 36;
    if(buffer.length() < head_len)
    {
        PLOG(ERROR) << "wrong protocol.";
        return;
    }
    
    int uuid_len = 36;
    std::string uuid = buffer.substr(3, uuid_len);
    std::string signal;
    signal.push_back(buffer[0]);
    PLOG(INFO) << "uuid is " << uuid;
    PLOG(INFO) << "signal is " << signal;
    if (signal == "s" && uuid == protocol_hub_->get_client_()->get_uuid_())
    {
        // ph->on_socket_ = true;
        // ph->connection_state_ = kOnPcmData;
        
        PLOG(INFO) << "TODO: FirstTimeConnect::execute()切换为pcm_data状态";
        
        protocol_hub_->set_nbest_(int(buffer[1] - '0'));
        if(buffer[2] == '0' || buffer[2] == '1')
        {
            protocol_hub_->set_continuous_decoding_(int(buffer[2] - '0'));
        }
        protocol_hub_->set_on_socket_(true);
        protocol_hub_->OnSpeechStart();
        protocol_hub_->ChangeHubState(kOnPcmData, "");
    }

}

} // namespace wenet