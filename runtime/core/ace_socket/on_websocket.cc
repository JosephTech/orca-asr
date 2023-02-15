#include <iterator>
#include <string.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <boost/json.hpp>
//#include <boost/json/src.hpp>
#include <cassert>
#include <stdlib.h>
#include <time.h>


#include "ace_socket/hub_state.h"
#include "ace_socket/protocol_hub.h"
#include "ace_socket/participant.h"
#include "ace_socket/base64.h"
#include "ace_socket/group.h"

#include <chrono>
#include <thread>

namespace wenet{

namespace json = boost::json;

void OnWebSocket::Enter(const std::string& buffer)
{
    RequestHttp rh = protocol_hub_->get_request_http_();
    PLOG(INFO) << "TODO(Joseph): 此处需计算base64(sha1(websocket_key + .....))";
    // 258EAFA5-E914-47DA-95CA-C5AB0DC85B11

    //string temp = rh.sec_websocket_key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    //unsigned const char ibuf[] = 
    
    // char w_key[rh.sec_websocket_key.length()+key.length()] = rh.sec_websocket_key.c_str();
    // const unsigned char temp[rh.sec_websocket_key.length()+key.length()] = strcat(w_key, key.c_str());

    // std::vector<BYTE> myData;
    // ...
    // std::string encodedData = base64_encode(&myData[0], myData.size());
    // std::vector<BYTE> decodedData = base64_decode(encodedData);

    char temp[128];
    
    unsigned char obuf[20];
    string key = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    strcpy(temp, rh.sec_websocket_key.c_str());
    PLOG(INFO) << "temp is " << temp;
    strcat(temp, key.c_str());
    PLOG(INFO) << "temp is" << temp;
    const unsigned char* ibuf = (const unsigned char*)temp;

    int len = rh.sec_websocket_key.length() + key.length();
    SHA1(ibuf, len, obuf);
    // 
    PLOG(INFO) << "************************";
    for(int i = 0; i < 20; ++i)
    {
        std::cout << std::hex << int(obuf[i]);
    }
    std::cout << std::endl;

    std::vector<BYTE> sha1_res(std::begin(obuf), std::end(obuf));
    std::string SecWebSocketAccept = base64_encode(&sha1_res[0], sha1_res.size());
    
    PLOG(INFO) << "SecWebSocketAccept is " << SecWebSocketAccept;

    std::string response;
    response.append("HTTP/1.1 101 Switching Protocols\r\n");
	response.append("Upgrade: websocket\r\n");
    response.append("Connection: Upgrade\r\n");
	response.append("Sec-WebSocket-Accept:" + SecWebSocketAccept + "\r\n");
    response.append("\r\n");
    
    PLOG(INFO) << "websocket server response is "<< response;

    if(-1 == protocol_hub_->get_client_()->socket().send_n(response.c_str(), response.length()))
    {
        //protocol_hub_->get_client_()->handle_close(ACE_INVALID_HANDLE, 0);
        PLOG(ERROR) << "websocket response error.";
    }
    // for(int i = 0; i < 100; ++i)
    // {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // }

    PLOG(INFO) << "发送成功" ;

}

void OnWebSocket::Execute(const std::string& buf)
{
    // all buffer is raw pcm data.
    // pcm_processor_->Execute(buffer);
    string buffer = remain_buffer_ + buf;
    //PLOG(INFO) << "TODO(Joseph): wrapping websocket code ";
    WebSocketProtocol frame;
    int ret = ParseFrame(buffer, frame);
    if(buffer.length() > ret)
    {
        PLOG(INFO) << "剩余有" << buffer.length() - ret;
        remain_buffer_ = buffer.substr(ret);
    }
    else
    {
        remain_buffer_ = "";
    }

    //PLOG(INFO) << "fin is "<< (int)frame.fin;
    // PLOG(INFO) << "opcode is " << (int)frame.opcode
    PLOG(INFO) << "TOOD(Joseph): 处理fin为0的情况";
    switch(frame.opcode)
    {
    case 0x0:
        // continuation frame
        break;
    case 0x1:
        // text frame
        PLOG(INFO) << "TODO(Joseph): process websocket start signal";
        ProcessTextPayload(frame.payload);
        break;
    case 0x2:
        // binary frame
        //ProcessBinaryPayload();
        //PLOG(INFO) << "websocket receive pcm data: " << frame.payload; 
        //PLOG(INFO) << "TODO(Joseph): process websocket pcm data";
        pcm_processor_->Execute(frame.payload);
        break;
    case 0x8:
        // connection close
        protocol_hub_->get_client_()->handle_close(ACE_INVALID_HANDLE, 0);
        break;
    case 0x9:
        // ping frame
        break;
    case 0xA:
        // pong frame
        break;
    }
    
}

int OnWebSocket::SendText(const std::string& text)
{
    string result_frame = "";
    if(-1 == PackFrame(true, false, 0x1, text, result_frame))
    {
        PLOG(ERROR) << "can't send, please check.";
        return -1;
    }
    protocol_hub_->get_client_()->socket().send_n(result_frame.c_str(), result_frame.length());
    return 0;
}
int OnWebSocket::PackFrame(bool fin, bool mask, uint8_t opcode, string payload, string& result_frame)
{
    PLOG(INFO) << "TODO(Joseph) apply mask";
    result_frame = "";
    PLOG(INFO) << "payload is" << payload;
    PLOG(INFO) << "payload_len is" << payload.length();
    uint32_t payload_len = payload.length();
    uint8_t m_bit = 0x80;

    //uint32_t int seed = 1008810088;
    srand((uint32_t)time(nullptr));
    uint32_t masking_key = rand();
    assert(opcode <= 0xf);
    if(payload_len < 126)
    {
        uint8_t buf[2];
        if(fin)
        {
            
            buf[0] = (m_bit | opcode) ;
        }
        else
        {
            buf[0] = opcode;
        }
        //        mask
        if(mask)
        {
            buf[1] = (m_bit | (uint8_t)payload_len);
        }
        else
        {
            buf[1] = (uint8_t)payload_len;
        }
        
        PLOG(INFO) << "buf[0] is" << (int)buf[0];
        PLOG(INFO) << "buf[1] is" << (int)buf[1];
        for(int i = 0; i < 2; ++i)
        {
            result_frame += (char)buf[i];
        }
        if(mask)
        {
            char mk[4];
            memcpy(mk, &masking_key, 4);
            for(int i = 0; i < 4; ++i)
            {
                result_frame += mk[i];
            }
            ApplyMask(payload, masking_key);
        }
        
        result_frame.append(payload);
    }
    else if (payload_len <= 0xffff)
    {
        uint8_t buf[4];
        if(fin)
        {
            buf[0] = static_cast<uint8_t>(m_bit | opcode);
        }
        else
        {
            buf[0] = opcode;
        }
        if(mask)
        {
            buf[1] = (m_bit | (uint8_t)126);
        }
        else
        {
            buf[1] = (uint8_t)126;
        }
        
        payload_len;
        uint16_t temp = static_cast<uint16_t>(payload_len);
        temp = htons(temp);
        memcpy(&buf[2], &temp, 2);
        for(int i = 0; i < 4; ++i)
        {
            result_frame += (char)buf[i];
        }
        if(mask)
        {
            char mk[4];
            memcpy(mk, &masking_key, 4);
            for(int i = 0; i < 4; ++i)
            {
                result_frame += mk[i];
            }
            ApplyMask(payload, masking_key);
        }
        result_frame.append(payload);
    }
    else if(payload_len <= 0xffffffff)
    {
        uint8_t buf[6];
        if(fin)
        {
            buf[0] = static_cast<uint8_t>(m_bit | opcode);
        }
        else
        {
            buf[0] = opcode;
        }
        if(mask)
        {
            buf[1] = (m_bit | (uint8_t)127);
        }
        else
        {
            buf[1] = (uint8_t)127;
        }
        
        uint32_t temp = payload_len;
        temp = htonl(temp);
        memcpy(&buf[2], &temp, 4);
        for(int i = 0; i < 6; ++i)
        {
            result_frame += (char)buf[i];
        }
        if(mask)
        {
            char mk[4];
            memcpy(mk, &masking_key, 4);
            for(int i = 0; i < 4; ++i)
            {
                result_frame += mk[i];
            }
            ApplyMask(payload, masking_key);
        }

        result_frame.append(payload);
    }
    PLOG(INFO) << "result_frame .length is " << result_frame.length();
    for(int i = 0; i < 2; ++i)
    {
        unsigned char ch = result_frame.c_str()[i];
        int b[8];
        for(int j = 7; j >= 0; --j)
        {
            b[j] = ((ch >> j) & 1);
            PLOG(INFO) << "send websocket head is" << b[j];
        }
    }
    

    return 0;
}

int OnWebSocket::ApplyMask(string& payload, uint32_t masking_key)
{
    char mk[4];
    memcpy(mk, &masking_key, 4);
    for(int i = 0; i < payload.length(); ++i)
    {
        int j = i % 4;
        payload[i] = payload[i] ^ mk[j];
    }

    return 0;
}

int OnWebSocket::ParseFrame(const std::string& buffer, WebSocketProtocol& frame)
{
    int index = 0;
    // first 16 bits
    // for(int i = 0; i < 2; ++i)
    // {
    //     // bits
    //     int b[8];
    //     unsigned char ch = buffer[i];
    //     for(int j = 7; j >= 0; --j)
    //     {
    //         b[j] =  ((ch >> j) & 1);
    //         PLOG(INFO) << b[j];
    //     }
    // }

    // uint8_t can't be LOG()
    frame.fin = static_cast<bool>((buffer[index] >> 7) & 0x1);
    frame.opcode = static_cast<uint8_t>(buffer[index] & 0xf);
    index++;
    //PLOG(INFO) << "fin is "<< frame.fin;
    PLOG(INFO) << "opcode is" << (int)frame.opcode;

    frame.mask = static_cast<bool>((buffer[index] >> 7) & 0x1);
    uint8_t temp_payload_len = static_cast<uint8_t>(buffer[index] & 0x7f); 
    //PLOG(INFO) << "mask is " << frame.mask;
    //PLOG(INFO) << "temp_payload_len is " << (int)temp_payload_len;

    // RFC 6455. [0,125] actual length. 126, following 16bit is length. 127, following 64bit is length.
    index++;
    if(125 >= temp_payload_len)
    {
        frame.payload_len = temp_payload_len;
        
    }
    else if(126 == temp_payload_len)
    {
        // LOG() 16 bits
        // for(int i = 0; i < 2; ++i){
        //     int b[8];
        //     unsigned char ch = buffer[index+i];
        //     for(int j = 0; j < 8; ++j)
        //     {
        //         b[j] =  ((ch >> j) & 1);
        //         PLOG(INFO) << b[j];
        //     }
        // }


        int16_t tmp;
        memcpy(&tmp, &buffer[index],2);
        frame.payload_len = ntohs(tmp);
        index += 2;
        //PLOG(INFO) << "126payload_len is" << frame.payload_len;
    }
    else if(127 == temp_payload_len)
    {
        int64_t tmp;
        memcpy(&tmp, &buffer[index],4);
        frame.payload_len = tmp;
        index += 4;
        PLOG(INFO) << "127payload_len is" << frame.payload_len;
    }
    PLOG(INFO) << "mask is " << frame.mask;
    if(0x1 == frame.mask)
    {
        memcpy(frame.masking_key, &buffer[index], 4);
        index += 4;
    }
    else
    {
        // RFC 6455. must have mask.
        PLOG(ERROR) << "RFC 6455. websocket protocol error, message must have mask.";
        protocol_hub_->get_client_()->handle_close(ACE_INVALID_HANDLE, 0);
    }

    frame.payload = buffer.substr(index, frame.payload_len);
    index += frame.payload_len;

    for(int i = 0; i < frame.payload.length(); ++i)
    {
        int j = i % 4;
        frame.payload[i] = frame.payload[i] ^ frame.masking_key[j];
    }

    return index;
}

void OnWebSocket::ProcessTextPayload(const std::string& text)
{
    PLOG(INFO)<< "websocket receive text: " << text;
    auto decode_val = json::parse(text);
    if(decode_val.is_object())
    {
        json::object obj = decode_val.get_object();
        boost::json::string signal = obj["signal"].as_string();
        if("new_group" == signal)
        {
            GroupManager::Instance().JoinNewGroup(protocol_hub_->get_client_());
            json::value rv = {"uuid", protocol_hub_->get_client_()->get_uuid_().c_str()};
            std::string response = json::serialize(rv);
            SendText(response);
        }
        else if("join_group" == signal)
        {
            if(obj.find("uuid") != obj.end() && obj["uuid"].is_string())
            {
                std::string uuid = json::value_to<std::string>(obj["uuid"]);
                PLOG(INFO) << "new group, uuid is" << uuid;
                if(-1 == GroupManager::Instance().JoinGroup(uuid, protocol_hub_->get_client_()))
                {
                    PLOG(ERROR) << "uuid not exist. close socket stream.";
                    protocol_hub_->get_client_()->handle_close(ACE_INVALID_HANDLE, 0);
                }
            }
            else
            {
                PLOG(ERROR) << "wrong protocol. close socket stream.";
                protocol_hub_->get_client_()->handle_close(ACE_INVALID_HANDLE, 0);
            }
        }
        else if("on_mic" == signal)
        {
            // on microphone. Grab the microphone.
            int ret = protocol_hub_->get_client_()->get_group_()->set_current_on_microphone_(protocol_hub_->get_client_());
            if(-1 == ret)
            {
                PLOG(ERROR) << "program logic error. client not in group. close socket stream.";
                protocol_hub_->get_client_()->handle_close(ACE_INVALID_HANDLE, 0);
            }
        }
        else if("start" == signal)
        {
            // wenet compatible
            if(nullptr == protocol_hub_->get_client_()->get_group_())
            {
                GroupManager::Instance().JoinNewGroup(protocol_hub_->get_client_());
            }
            
            // configs
            if(obj.find("nbest") != obj.end() && obj["nbest"].is_int64())
            {
                protocol_hub_->set_nbest_(obj["nbest"].as_int64());
            }
            if(obj.find("continuous_decoding") != obj.end() && obj["continuous_decoding"].is_bool())
            {
                //PLOG(INFO) << "TODO(Joseph): continuous_decoding";
                protocol_hub_->set_continuous_decoding_(obj["continuous_decoding"].as_bool());
            }
            PLOG(INFO) << "TODO(Joseph)开始录音";
            protocol_hub_->OnSpeechStart();
            //protocol_hub_->ChangeHubState(kOnPcmData, "");
        }
        else if("end" == signal)
        {
            protocol_hub_->HandleClose();
        }

    }
    else
    {
        PLOG(INFO) << ("wrong protocol");
        protocol_hub_->get_client_()->handle_close(ACE_INVALID_HANDLE, 0);
    }

}

} // namespace wenet