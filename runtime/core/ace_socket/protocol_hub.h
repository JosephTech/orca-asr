#ifndef PROTOCOL_HUB_H_
#define PROTOCOL_HUB_H_

#include <cassert>
#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>


#include "utils/log.h"

#include "decoder/asr_decoder.h"
#include "frontend/feature_pipeline.h"


#include "ace_socket/recorder.h"
#include "ace_socket/hub_state.h"


namespace wenet{
    


// class ProtocolHub;  // 前置声明




// class HubState;  // 前置声明
// class FirstTimeConnect;

class Participant;

class ProtocolHub{
public:
    ProtocolHub(Participant* client,
                std::shared_ptr<FeaturePipelineConfig> feature_config,
                std::shared_ptr<DecodeOptions> decode_config,
                std::shared_ptr<DecodeResource> decode_resource);

    ~ProtocolHub();
            
    int ProcessRequest(const char* buf, ssize_t rev);
    int ProcessRespond();
    Recorder& get_recorder_();
    int SavePcmFile();

    
    // void OnSpeechData(const string& buffer);
    // void RunDecodeThread();
    void DecodeThreadFunc();
    void OnSpeechStart();
    void OnPartialResult(const std::string& result);
    void OnFinalResult(const std::string& result);
    void OnFinish();
    void HandleClose();
    

    void ChangeHubState(ConnectionState state, const string& buffer);
    std::string SerializeResult(bool finish);

    void set_nbest_(int n){nbest_ = n;}
    void set_continuous_decoding_(bool flag){continuous_decoding_ = flag;}

    std::string get_all_pcm_data_();
    bool get_record_pcm_(){return record_pcm_;}
    void set_on_socket_(bool flag){on_socket_ = flag;}
    bool is_on_socket_(){return on_socket_;}

    void set_request_http_(RequestHttp rh){request_http_ = rh;}
    RequestHttp& get_request_http_(){return request_http_;}

    void set_on_websocket_(bool flag){on_websocket_ = flag;}
    bool is_on_websocket_(){return on_websocket_;}
    Participant* get_client_(){return client_;}
    HubState* get_hub_state_(){return hub_state_;}

    std::shared_ptr<FeaturePipeline>& get_feature_pipeline_(){return feature_pipeline_;}
    std::shared_ptr<std::thread>& get_decode_thread_(){return decode_thread_;}
    // std::shared_ptr<DecodeResource>& get_decode_resource_(){return decode_resource_;}



private:
    bool on_socket_ = false;
    bool on_websocket_ = false;             // connection state.
    bool record_pcm_ = false;               // whether to save the pcm data.

    int64_t nbest_ = 1;
    bool continuous_decoding_ = false;
    std::string all_pcm_data_;
    Recorder recorder_;

    //std::unordered_map<CommunicationState, HubState*> states_machine_;

    std::shared_ptr<FeaturePipelineConfig> feature_config_;
    std::shared_ptr<DecodeOptions> decode_config_;
    std::shared_ptr<DecodeResource> decode_resource_;
    
    std::shared_ptr<FeaturePipeline> feature_pipeline_ = nullptr;
    std::shared_ptr<AsrDecoder> decoder_ = nullptr;
    std::shared_ptr<std::thread> decode_thread_ = nullptr;

    // CommunicationState connection_state_ = kOnFirstTimeConnect;

    Participant* client_;
    HubState* hub_state_;
    FirstTimeConnect* first_connect_state_;
    OnTcpReady* on_tcp_ready_state_;
    OnPcmData* on_pcm_data_state_;
    OnWaitResult* on_wait_result_state_;
    OnHttpRequest* on_http_request_state_;
    OnWebSocket* on_websocket_state_;
    
    // OnHttpRequest* on_http_request_state_;
    RequestHttp request_http_;
};


} // namespace wenet

#endif /* PROTOCOL_HUB_H_ */