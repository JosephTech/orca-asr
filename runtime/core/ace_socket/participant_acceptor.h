#ifndef PARTICIPANTACCEPTOR_H_
#define PARTICIPANTACCEPTOR_H_

#include <ace/Reactor.h>
#include <ace/Event_Handler.h>
#include <ace/SOCK_Acceptor.h>

#include "decoder/asr_decoder.h"
#include "frontend/feature_pipeline.h"

#include "ace_socket/participant.h"
#include "ace_socket/recorder.h"

namespace wenet{

class ParticipantAcceptor: public ACE_Event_Handler{
public:
    ParticipantAcceptor(std::shared_ptr<FeaturePipelineConfig> feature_config,
                        std::shared_ptr<DecodeOptions> decode_config,
                        std::shared_ptr<DecodeResource> decode_resource)
            :
            feature_config_(std::move(feature_config)),
            decode_config_(std::move(decode_config)),
            decode_resource_(std::move(decode_resource)){}
    
    ~ParticipantAcceptor();

    int open(const ACE_INET_Addr &listen_addr);
    int handle_input (ACE_HANDLE fd);
    int handle_close (ACE_HANDLE handle,
                        ACE_Reactor_Mask close_mask);
    ACE_HANDLE get_handle () const
        { return this->acceptor_.get_handle (); }
private:
    ACE_SOCK_Acceptor acceptor_;
    std::shared_ptr<FeaturePipelineConfig> feature_config_;
    std::shared_ptr<DecodeOptions> decode_config_;
    std::shared_ptr<DecodeResource> decode_resource_;
};

} // namespace wenet

#endif /* PARTICIPANTACCEPTOR_H_ */