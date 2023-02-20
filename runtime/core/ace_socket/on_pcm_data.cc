#include "ace_socket/hub_state.h"
#include "ace_socket/protocol_hub.h"
#include "ace_socket/participant.h"

namespace wenet{

void OnPcmData::Enter(const std::string& buffer)
{
    PLOG(INFO) << "TODO(Joseph): may need process data";
    return;
}
void OnPcmData::Execute(const std::string& buffer)
{
    //PLOG(INFO) << "TODO: OnPcmData::Execute() be called";
    if(protocol_hub_->is_on_socket_())
    {
        // socket pcm
        int head_len = 3 + 36;
        int uuid_len = 36;
        std::string uuid = buffer.substr(3, uuid_len);
        std::string signal;
        signal.push_back(buffer[0]);

        if(uuid == protocol_hub_->get_client_()->get_uuid_())
        {
            PLOG(INFO) << "uuid is " << uuid;
            PLOG(INFO) << "signal is " << signal;
            if (signal == "e")
            {
                PLOG(INFO) << "OnPcmData::Execute() socket结束录音,向decoder发送停止信号, 发送等待解码完成，发送result到客户端\n";
                // protocol_hub_->OnSpeechEnd();
                protocol_hub_->HandleClose();  // 切换为wait result状态，等待对端关闭即可
                PLOG(INFO) << "在handle_close()时候,join()线程";
                // protocol_hub_->get_decode_thread_()->join();
            }
            else if(signal == "o")
            {
                // on microphone. Grab the microphone.
                protocol_hub_->get_client_()->get_group_()->SetGroupLeader(protocol_hub_->get_client_());
            }
        }
        
    }
    // if(protocol_hub_->is_on_websocket_())
    // {
    //     PLOG(INFO) << "TODO(Joseph) 等待结束信号";
    // }

    // only one possible.
    if (protocol_hub_->is_on_websocket_() ^ protocol_hub_->is_on_socket_())
    {
        if(!protocol_hub_->is_group_leader_())
        {
            // only group leader can talk.
            return;
        }
        // socket/websocket pcm, pure pcm.
        int num_samples = buffer.length() / sizeof(int16_t);
        const int16_t* pcm_data = reinterpret_cast<const int16_t*>(buffer.data());
        //PLOG(INFO) << "Put pcm data into queue.";
        protocol_hub_->get_feature_pipeline_()->AcceptWaveform(pcm_data, num_samples);
    }
    return;
}
void OnPcmData::Exit()
{
    return;
}
} // namespace wenet