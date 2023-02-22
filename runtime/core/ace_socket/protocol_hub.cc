
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
    first_connect_state_ = new FirstTimeConnect(this);
    on_pcm_data_state_ = new OnPcmData(this);
    on_tcp_ready_state_ = new OnTcpReady(this);
    on_wait_result_state_ = new OnWaitResult(this);
    on_http_request_state_ = new OnHttpRequest(this);
    on_websocket_state_ = new OnWebSocket(this, on_pcm_data_state_);
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

int ProtocolHub::ProcessRequest(const char* buf, ssize_t rev)
{
    std::string buffer(buf,rev);

    // 处理每次新的buffer，放在Execute, 切换状态时，上个状态剩余数据处理，放在->enter(中)
    //PLOG(INFO) << "hub_state_ is " << hub_state_->get_hub_state_();
    hub_state_->Execute(buffer);
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

void ProtocolHub::OnSpeechStart()
{    
    PLOG(INFO) << "TODO(Joseph):" << "此处须向客户端发送start信息";
    feature_pipeline_ = std::make_shared<FeaturePipeline>(*feature_config_);
    decoder_ = std::make_shared<AsrDecoder>(feature_pipeline_, decode_resource_, *decode_config_);
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
}


void ProtocolHub::OnPartialResult(const std::string& result) {
    //PLOG(INFO) << "Partial result: " << result;
    //PLOG(INFO) << "TODO(Joseph): 此处需发送Partial识别结果";
    json::value rv = {
        {"status", "ok"}, {"type", "partial_result"}, {"nbest", result}};
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
}

} // namespace wenet
