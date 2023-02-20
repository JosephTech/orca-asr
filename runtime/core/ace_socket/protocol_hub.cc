
#include <cstring>

#include <boost/json/src.hpp>
#include <boost/asio.hpp>
#include "utils/log.h"

#include "ace_socket/protocol_hub.h"
#include "ace_socket/participant.h"
namespace wenet{

namespace json = boost::json;
namespace asio = boost::asio;

ProtocolHub::ProtocolHub(Participant* client,
                std::shared_ptr<FeaturePipelineConfig> feature_config,
                std::shared_ptr<DecodeOptions> decode_config,
                std::shared_ptr<DecodeResource> decode_resource):
        client_(client),
        feature_config_(std::move(feature_config)),
        decode_config_(std::move(decode_config)),
        decode_resource_(std::move(decode_resource))
{
    // OnSpeechStart("");                
    // states_machine_[kOnFirstTimeConnect]->enter(this);
    first_connect_state_ = new FirstTimeConnect(this);
    // first_connect_state_->PassConfigs(feature_config_,
    //                                     decode_config_,
    //                                     decode_resource_,
    //                                     decoder_,
    //                                     decode_thread_);
    on_pcm_data_state_ = new OnPcmData(this);
    on_tcp_ready_state_ = new OnTcpReady(this);
    on_wait_result_state_ = new OnWaitResult(this);
    on_http_request_state_ = new OnHttpRequest(this);
    on_websocket_state_ = new OnWebSocket(this, on_pcm_data_state_);
    // on_http_request_state_ = new OnHttpRequest();
    hub_state_ = first_connect_state_;
}

ProtocolHub::~ProtocolHub()
{
    PLOG(INFO) << "hub_析构..";
    delete first_connect_state_;
    delete on_pcm_data_state_;
    delete on_tcp_ready_state_;
    delete on_wait_result_state_;
    delete on_http_request_state_;
    delete on_websocket_state_;
}

// int ProtocolHub::ProcessRequest(const std::string& buffer)
int ProtocolHub::ProcessRequest(const char* buf, ssize_t rev)
{
    //const char record_begin[9] = ;
    std::string buffer(buf,rev);

    // 处理每次新的buffer，放在Execute, 切换状态时，上个状态剩余数据处理，放在->enter(中)
    //PLOG(INFO) << "hub_state_ is " << hub_state_->get_hub_state_();
    hub_state_->Execute(buffer);

    // TODO: 状态： on_pcm, 
    // LOG(INFO) << "通信状态: " << connection_state_;
    // switch(connection_state_)
    // {
    //   case kOnFirstTimeConnect:
    //     LOG(INFO) << "TODO: 不是socket起始信号和配置，就是http 请求";
    //     std::string head = buffer.substr(0, 9);
    //     if (head == "suuiduuid")
    //     {
    //       on_socket_ = true;
    //       connection_state_ = kOnPcmData;
    //       string config = "配置信息"；
    //       OnSpeechStart(config);
    //     }
    //     else
    //     {
    //       connection_state_ = kOnHttpRequest;
    //       LOG(INFO) << "TODO: 处理http请求 把请求传递给http server"；
    //     }
    //     break;
    //   case kOnPcmData:
    //     LOG(INFO) << "TODO: 不是socket/websocket 结束信号，就是pcm数据";
    //     if(head == "euuiduuid")
    //     {
    //       printf("socket结束录音\n");
    //     }
    //     else
    //     {
    //       LOG(INFO) << "TODO: 不是socket/websocket 结束信号, 切换到wait状态";
    //     }
    //     break;
    //   case kOnHttpRequest:
    //     LOG(INFO) << "TODO: 把请求传递给http server"； 
    //     LOG(INFO) << "TODO: 不是http 请求，就是需要更新到websocket协议";
    //     LOG(INFO) << "TODO: 如果更新websocket协议，就改 形参&connection_state_"；
    //     LOG(INFO) << "TODO: 如果更新websocket协议，就改 形参&connection_state_"；
    //     break;
    //   case kOnWebSocket:
    //     on_websocket_ = true;
    //     LOG(INFO) << "TODO: 不是开始信号和配置，就是raw pcm数据";
    //     break;
    //   case kOnWaitResult:
    //     break;
    //   // case kOnWaitSocketResult:
    //   //   break;
    //   // case kOnWaitWebsocketResult:
    //   //   break;
    //   // case kOnIdle:
    //   //   if(on_socket_)
    //   //   {

    //   //   }
    //   //   else if (on_websocket_)
    //   //   {

    //   //   }
    //   //   break;
    // }

    // if (true == on_socket_pcm_)
    // {
    //     assert(false == on_websocket_);
    //     //std::cout << "INFO receving pcm data..." << buffer << std::endl;
    //     // memcpy(pcm_buf_[buf_idx_++], buf, rev);
    //     // last_rev_ = rev;
        
    //     //printf("INFO receving string size: %ld\n", buffer.size());
    //     if(record_pcm_)
    //     {
    //         all_pcm_data_.append(buffer);
    //     }
    //     //printf("INFO all_pcm_data_ size: %ld\n\n\n", all_pcm_data_.size());

        
    //     OnSpeechData(buffer);
    // }
    // else if (true == on_websocket_)
    // {
    //     assert(false == on_socket_pcm_);
    //     printf("INFO receving websocket data...\n");
    // }
    // else
    // {
    //     // char str[PROTOCOL_POS+1];
    //     // strncpy(str, buf, PROTOCOL_POS);

    //     // str[PROTOCOL_POS+1] = '\0';
    //     //printf("str is %s\n", str);
    //     std::string head = buffer.substr(0, 9);
    //     if (head == "suuiduuid")
    //     {
    //         OnSpeechStart();
    //     }
    //     else if(head == "euuiduuid")
    //     {
    //         printf("结束录音\n");
    //         on_socket_pcm_ = false;
    //     }
    //     else
    //     {
    //         printf("使用websocket或者http服务\n");
    //         on_websocket_  = true;
    //     }
    // }

    return 0;
}


int ProtocolHub::ProcessRespond()
{
    return 0;
}

Recorder& ProtocolHub::get_recorder_()
{
    return recorder_;
}


std::string ProtocolHub::get_all_pcm_data_()
{
    return all_pcm_data_;
}


int ProtocolHub::SavePcmFile()
{
    recorder_.SavePcmFile(all_pcm_data_);
    //decode_thread_->join();
    return 0;
}

// void ProtocolHub::OnSpeechData(const string& buffer)
// {
//     int num_samples = buffer.length() / sizeof(int16_t);

//     const int16_t* pcm_data = reinterpret_cast<const int16_t*>(buffer.data());
//     //printf("INFO pcm_data is %hd\n", *pcm_data);

//     feature_pipeline_->AcceptWaveform(pcm_data, num_samples);
// }

/*
  params: 配置json
  */
void ProtocolHub::OnSpeechStart()
{    
    //LOG(INFO) << "Receive start signal.";
    // std::shared_ptr<FeaturePipelineConfig> feature_config(new FeaturePipelineConfig(80, 16000));
    // feature_config_ = std::move(feature_config);

    // PLOG(INFO) << "todo:  ProtocolHub::OnSpeechStart()此处需向客户端发送开始信息\n";
    // PLOG(INFO) << "todo: ProtocolHub::OnSpeechStart()此处需解析客户端配置信息\n";
    // PLOG(INFO) << "配置json is: " << config;
    PLOG(INFO) << "TODO(Joseph):" << "此处须向客户端发送start信息";
    feature_pipeline_ = std::make_shared<FeaturePipeline>(*feature_config_);
    decoder_ = std::make_shared<AsrDecoder>(feature_pipeline_, decode_resource_, *decode_config_);
    // on_socket_ = true;
    // hub_state_ = on_pcm_data_state_;
    PLOG(INFO) << "TODO(Joseph):  ProtocolHub::OnSpeechStart()此处需join()线程\n";
    decode_thread_ = std::make_shared<std::thread>(&ProtocolHub::DecodeThreadFunc, this);
}

void ProtocolHub::HandleClose()
{
    if(hub_close_) return;
    // fix a bug. websocket state, receive end signal.
    if (feature_pipeline_)
    {
        feature_pipeline_->set_input_finished();
    }
    hub_close_ = true;
    ChangeHubState(kOnWaitResult, "");
}

// void ProtocolHub::RunDecodeThread()
// {
//     decode_thread_ = std::make_shared<std::thread>(&ProtocolHub::DecodeThreadFunc, this);
// }

//
// from wenet
//
void ProtocolHub::DecodeThreadFunc()
{
    PLOG(INFO) << "TODO decode线程启动\n";
    PLOG(INFO) << "TODO(Joseph): 此处需发送解码结果";
    try {
    while (true) {
      DecodeState state = decoder_->Decode();
      if (state == DecodeState::kEndFeats) {
        decoder_->Rescoring();
        std::string result = SerializeResult(true);
        OnFinalResult(result);
        OnFinish();
        // stop_recognition_ = true;
        break;
      } else if (state == DecodeState::kEndpoint) {
        decoder_->Rescoring();
        std::string result = SerializeResult(true);
        OnFinalResult(result);
        // If it's not continuous decoding, continue to do next recognition
        // otherwise stop the recognition
        if (continuous_decoding_) {
          decoder_->ResetContinuousDecoding();
        } else {
          OnFinish();
        //   stop_recognition_ = true;
          break;
        }
      } else {
        if (decoder_->DecodedSomething()) {
          std::string result = SerializeResult(false);
          OnPartialResult(result);
        }
      }
    }
  } catch (std::exception const& e) {
    LOG(ERROR) << e.what();
  }
    // // try {
    // while (true) {
    //   DecodeState state = decoder_->Decode();

    //   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //   if (state == DecodeState::kEndpoint  || state == DecodeState::kEndFeats) 
    //   {
    //       // 检测到端点  所有特征解码结束  
    //       decoder_->Rescoring();
    //       std::string result = SerializeResult(true);
    //       PLOG(INFO) << "结果是:" << result;
    //       break;
          // if (decoder_->DecodedSomething()) {
          //     //std::string result = SerializeResult(false);
          //     std::string result = "";
          //     for (const DecodeResult& path : decoder_->result()) {
          //       result = path.sentence;
          //       OnPartialResult(result);
          //     }
          //     break;
          // }      
    //   }

      // if (state == DecodeState::kEndFeats) {
      //   decoder_->Rescoring();
      //   std::string result = SerializeResult(true);
      //   // OnFinalResult(result);
      //   // OnFinish();
      //   // stop_recognition_ = true;
      //   break;
      // } else if (state == DecodeState::kEndpoint) {
      //   decoder_->Rescoring();
      //   std::string result = SerializeResult(true);
      //   // OnFinalResult(result);
      //   // // If it's not continuous decoding, continue to do next recognition
      //   // // otherwise stop the recognition
      //   // if (continuous_decoding_) {
      //   //   decoder_->ResetContinuousDecoding();
      //   // } else {
      //   //   OnFinish();
      //   //   stop_recognition_ = true;
      //   //   break;
      //   // }
      // } else {
      //   if (decoder_->DecodedSomething()) {
      //     std::string result = SerializeResult(false);
      //     OnPartialResult(result);
      //   }
      // }
    // }
    // } catch (std::exception const& e) {
    //   LOG(ERROR) << e.what();
    // }
}


void ProtocolHub::OnPartialResult(const std::string& result) {
    //PLOG(INFO) << "Partial result: " << result;
    //PLOG(INFO) << "TODO(Joseph): 此处需发送Partial识别结果";
    json::value rv = {
        {"status", "ok"}, {"type", "partial_result"}, {"nbest", result}};
    // ws_.text(true);
    // ws_.write(asio::buffer(json::serialize(rv)));
    std::string sendbuf = json::serialize(rv);
    sendbuf += "\r\n";
    //PLOG(INFO) << "send buf is " << sendbuf;
    if(nullptr != client_->get_group_())
    {
        client_->get_group_()->BroadcastMessage(sendbuf);
    }
    
}

void ProtocolHub::OnFinalResult(const std::string& result) {
    //PLOG(INFO) << "Final result: " << result;
    // PLOG(INFO) << "n best is " << nbest_;
    // PLOG(INFO) << "continuous_decoding_ is " << continuous_decoding_;
    //PLOG(INFO) << "TODO(Joseph): 此处需发送final识别结果";
    json::value rv = {{"status", "ok"}, {"type", "final_result"}, {"nbest", result}};
    //   ws_.text(true);
    //   ws_.write(asio::buffer(json::serialize(rv)));
    std::string sendbuf = json::serialize(rv);
    sendbuf += "\r\n";

    //PLOG(INFO) << "send buf is " << sendbuf;
    if(nullptr != client_->get_group_())
    {
        client_->get_group_()->BroadcastMessage(sendbuf);
    }
}

void ProtocolHub::OnFinish() {
    //PLOG(INFO) << "TODO(Joseph): 此处需发送识别结束信号"; 
    json::value rv = {{"status", "ok"}, {"type", "speech_end"}};
    // ws_.text(true);
    // ws_.write(asio::buffer(json::serialize(rv)));
    std::string sendbuf = json::serialize(rv);
    sendbuf += "\r\n";

    client_->set_decode_thread_finish_();
    //PLOG(INFO) << "send buf is " << sendbuf;
    // client_->handle_close(ACE_INVALID_HANDLE, 0);  // 如果服务器主动关闭，客户端将收不到这条消息
    if(nullptr != client_->get_group_())
    {
        client_->get_group_()->BroadcastMessage(sendbuf);
    }
}

std::string ProtocolHub::SerializeResult(bool finish) 
{
    //PLOG(INFO) << "正在序列化解析结果...\n";
    json::array nbest;
    for (const DecodeResult& path : decoder_->result()) {
        json::object jpath({{"sentence", path.sentence}});
        if (finish) {
            json::array word_pieces;
            for (const WordPiece& word_piece : path.word_pieces) {
                json::object jword_piece({{"word", word_piece.word},
                                          {"start", word_piece.start},
                                          {"end", word_piece.end}});
                word_pieces.emplace_back(jword_piece);
            }
            jpath.emplace("word_pieces", word_pieces);
        }
        nbest.emplace_back(jpath);

        if (nbest.size() == nbest_) {
          break;
        }
    }
    return json::serialize(nbest);
    // return "";
}

void ProtocolHub::ChangeHubState(ConnectionState state, const string& buffer)
{
  // if (state == kOnPcmData)
  // {

  // }
  hub_state_->Exit();

  switch (state)
  {
  case kOnPcmData:
      hub_state_ = on_pcm_data_state_;
      break;
  case kOnTcpReady:
      hub_state_ = on_tcp_ready_state_;
      break;
  case kOnWaitResult:
      hub_state_ = on_wait_result_state_;
      break;
  case kOnHttpRequest:
      hub_state_ = on_http_request_state_;
      break;
  case kOnWebSocket:
      hub_state_ = on_websocket_state_;
      break;
  }
  PLOG(INFO) << "ProtocolHub::ChangeHubState()切换为" << hub_state_->get_hub_state_() << "状态";
  hub_state_->Enter(buffer);
  // hub_state_->Execute(buffer);  // 每次handle_input()来数据
}

} // namespace wenet
