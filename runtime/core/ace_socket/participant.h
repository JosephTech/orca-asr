#ifndef PARTICIPANT_H_
#define PARTICIPANT_H_
#include <ace/Reactor.h>
#include <ace/Event_Handler.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Log_Msg.h>
#include <cassert>
#include <fstream>
#include <sstream>

#include "ace_socket/protocol_hub.h"
#include "ace_socket/group.h"
#include "ace_socket/wait_end_thread.h"
// class ProtocolHub;

#define MAX_BUF_LEN (64*1024) 

namespace wenet{



class Participant: public ACE_Event_Handler{
public:
    Participant(std::shared_ptr<FeaturePipelineConfig> feature_config,
                        std::shared_ptr<DecodeOptions> decode_config,
                        std::shared_ptr<DecodeResource> decode_resource)
            :
            feature_config_(std::move(feature_config)),
            decode_config_(std::move(decode_config)),
            decode_resource_(std::move(decode_resource)){}
    ~Participant(){}
    
    int open();
    int handle_input(ACE_HANDLE handle = ACE_INVALID_HANDLE);
    int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

    ACE_SOCK_Stream& socket(){return sock_;}
    ACE_HANDLE get_handle () const
        { return sock_.get_handle (); }
    
    std::shared_ptr<ProtocolHub> get_hub_(){return hub_; }
    void set_uuid_(std::string uuid){uuid_ = uuid; }
    std::string& get_uuid_(){return uuid_; }
    Group* get_group_(){return group_; }
    void set_group_(Group* gp){group_ = gp; }
    void set_decode_thread_finish_(){decode_thread_finish_ = true; }
    bool is_decode_thread_finish_(){return decode_thread_finish_; }

// private:
//     int SavePcmFile();
private:
    ACE_SOCK_Stream sock_;

    std::shared_ptr<FeaturePipelineConfig> feature_config_;
    std::shared_ptr<DecodeOptions> decode_config_;
    std::shared_ptr<DecodeResource> decode_resource_;
    std::string uuid_ = "-1";
    std::shared_ptr<ProtocolHub> hub_ = nullptr;
    Group* group_ = nullptr;
    bool decode_thread_finish_ = false;
};

} // namespace wenet

#endif /* PARTICIPANT_H_ */
