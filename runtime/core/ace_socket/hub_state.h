#ifndef HUB_STATE_H_
#define HUB_STATE_H_

#include <string>
#include <map>
#include <memory>
#include <vector>

// #include "ace_socket/protocol_hub.h"
#include "utils/log.h"

namespace wenet{

enum ConnectionState
{    
    kOnFirstTimeConnect,                // http header or socket protocol data with start signal and configs.
    kOnTcpReady,
    kOnPcmData,                         // receive socket end signal or websocket end signal or continue put pcm data into feature_pipeline_ queue.
    kOnHttpRequest,                     // whether http request or need update to websocket protocol.
    kOnWebSocket,                       // receive pcm data or start signal and configs.
    kOnWaitResult,                      // waitting decode result.
    // kOnWaitSocketResult,             // waitting socket decode result，can't receive new request until SocketResultSent event.
    // kOnWaitWebsocketResult,          // watiting websocket decode result, can't receive new request until WebsocketResultSent event.
    // kOnIdle,                         // keep socket/websocket connection, wait for the next speech.
};

class ProtocolHub;

//
//  hub at different has different behavior to process data.
//
class HubState{
public:
    virtual ~HubState(){}
    virtual void Enter(const std::string& buffer) = 0;          // process last buffer at the previous state
    virtual void Execute(const std::string& buffer) = 0;        // handle_input()
    virtual void Exit() = 0;
    virtual ConnectionState get_hub_state_() = 0;
private:
};

class FirstTimeConnect: public HubState{
public:
    // FirstTimeConnect() = default;
    FirstTimeConnect(ProtocolHub* ph):protocol_hub_(ph){}
    ~FirstTimeConnect(){}
    void Enter(const std::string& buffer);
    void Execute(const std::string& buffer);
    void Exit();
    ConnectionState get_hub_state_(){return kOnFirstTimeConnect;}
    // void PassConfigs(std::shared_ptr<FeaturePipelineConfig> feature_config,
    //                     std::shared_ptr<DecodeOptions> decode_config,
    //                     std::shared_ptr<DecodeResource> decode_resource);
    // void OnSpeechStart(const std::string& config);
private:
    ProtocolHub* protocol_hub_;
    // std::shared_ptr<FeaturePipelineConfig> feature_config_;
    // std::shared_ptr<DecodeOptions> decode_config_;
    // std::shared_ptr<DecodeResource> decode_resource_;
};




class OnTcpReady: public HubState{
public:
    // OnPcmData() = default;
    OnTcpReady(ProtocolHub* ph):protocol_hub_(ph){}
    ~OnTcpReady(){}
    void Enter(const std::string& buffer){};
    void Execute(const std::string& buffer);
    void Exit(){};
    ConnectionState get_hub_state_(){return kOnTcpReady;}
private:
    ProtocolHub* protocol_hub_;
};

class OnPcmData: public HubState{
public:
    // OnPcmData() = default;
    OnPcmData(ProtocolHub* ph):protocol_hub_(ph){}
    ~OnPcmData(){}
    void Enter(const std::string& buffer);
    void Execute(const std::string& buffer);
    void Exit();
    ConnectionState get_hub_state_(){return kOnPcmData;}
private:
    ProtocolHub* protocol_hub_;
};

struct RequestHttp{
    std::string method;                         //请求方法get、post等等
    std::string url;                            //请求路径
    std::string version;                        //协议版本
    std::string host;                           //主机
    std::string connection;                     //链接后续状态，是否升级协议
    std::string upgrade;                        //需要升级为什么协议
    std::string cookie;                         //cookie
	std::map<std::string, std::string> param;   //get请求的参数
	std::string sec_websocket_key;              //用于建立websocket协议
    std::string content;		                //主体内容
};

class OnHttpRequest: public HubState{
public:
    // OnHttpRequest() = default;
    OnHttpRequest(ProtocolHub* ph):protocol_hub_(ph){}
    ~OnHttpRequest(){}
    void Enter(const std::string& buffer);
    void Execute(const std::string& buffer);
    void Exit();
    ConnectionState get_hub_state_(){return kOnHttpRequest;}
private:
    int ParseHttpRequest(const std::string& buffer, RequestHttp* rh);
    std::vector<std::string> split(const std::string& str, std::string separator);
    std::string strip_leading_char(const std::string& str, const char lead);
    ProtocolHub* protocol_hub_;
};

// RFC 6455.
struct WebSocketProtocol{
	bool fin;                           //数据帧状态：0b0 it's not last fragment, continue. 0b1 last fragment
	bool mask;                          //是否掩码
    char masking_key[4];                //掩码,若Mask为1则该字段存在，若为0则该字段缺失
    uint8_t opcode;                     //控制码：0x0继续、0x1文本、0x2二进制，0x8关闭，0x9ping，0xApong
    uint64_t payload_len;               //数据长度
    std::string payload;                //数据载荷
};


class OnWebSocket: public HubState{
public:
    // OnPcmData() = default;
    OnWebSocket(ProtocolHub* ph, OnPcmData* pd):protocol_hub_(ph), pcm_processor_(pd){}
    ~OnWebSocket(){}
    void Enter(const std::string& buffer);
    void Execute(const std::string& buf);
    void Exit(){}
    int SendText(const std::string& text);
    ConnectionState get_hub_state_(){return kOnWebSocket;}
private:    
    int ParseFrame(const std::string& buffer, WebSocketProtocol& frame);
    void ProcessTextPayload(const std::string& buffer);
    int PackFrame(bool fin, bool mask, uint8_t opcode, string payload, string& result_frame);
    int ApplyMask(string& payload, uint32_t masking_key);
    ProtocolHub* protocol_hub_;
    OnPcmData* pcm_processor_;
    std::string remain_buffer_ = "";
};



class OnWaitResult: public HubState{
public:
    // OnPcmData() = default;
    OnWaitResult(ProtocolHub* ph):protocol_hub_(ph){}
    ~OnWaitResult(){}
    void Enter(const std::string& buffer){}
    void Execute(const std::string& buffer){}
    void Exit(){}
    ConnectionState get_hub_state_(){return kOnWaitResult;}
private:
    ProtocolHub* protocol_hub_;
};

} // namespace wenet

#endif /* HUB_STATE_H_ */