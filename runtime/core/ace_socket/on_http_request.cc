#include "ace_socket/hub_state.h"
#include "ace_socket/protocol_hub.h" 

namespace wenet{

void OnHttpRequest::Enter(const std::string& buffer)
{
    RequestHttp rh;
    if(-1 == ParseHttpRequest(buffer, &rh))
    {
        PLOG(ERROR) << "invalid protocol.";
        return;
    }
    PLOG(INFO) << rh.method;
    PLOG(INFO) << rh.url;
    PLOG(INFO) << rh.version;
    PLOG(INFO) << rh.host;
    PLOG(INFO) << rh.connection;
    PLOG(INFO) << rh.upgrade;
    PLOG(INFO) << rh.sec_websocket_key;
    //PLOG(INFO) << rh.param;
    if(0 == rh.version.compare("HTTP/1.1") && rh.connection.find("Upgrade") == std::string::npos)
    {
        PLOG(INFO) << "TODO(Joseph): 此处继续使用http协议, 获取网页";
    }
    else if(0 == rh.version.compare("HTTP/1.1") && rh.connection.find("Upgrade") != std::string::npos && rh.upgrade.find("websocket") != std::string::npos)
    {
        PLOG(INFO) << "TODO(Joseph): change to websocket protocol";
        protocol_hub_->set_on_websocket_(true);
        protocol_hub_->set_request_http_(rh);
        protocol_hub_->ChangeHubState(kOnWebSocket, "");
    }
    return;
}

void OnHttpRequest::Execute(const std::string& buffer)
{
    return;
}

void OnHttpRequest::Exit()
{
    return;
}

int OnHttpRequest::ParseHttpRequest(const std::string& buffer, RequestHttp* rh)
{
    PLOG(INFO) << "http request buffer is " << buffer;
    std::vector<std::string> request_lines = split(buffer, "\r\n");
    if (request_lines.size() < 1)
    {
        PLOG(ERROR) << "http \r\n wrong";
        return -1;
    }
    // header
    std::vector<std::string> request_headers = split(request_lines[0], " ");
    // PLOG(INFO) << "headers size: " << request_headers.size();
    if (request_headers.size() == 3)
    {
        rh->method = request_headers[0];
        rh->url = request_headers[1];
        rh->version = request_headers[2];
        if(rh->url.find("?") != std::string::npos)
        {
            string content = split(rh->url, "?")[1];
            std::vector<std::string> params = split(content, "&");
            for(auto param : params)
            {
                std::vector<std::string> param_pair = split(param, "=");
                if(param_pair.size() != 2)
                {
                    PLOG(ERROR) << "http param wrong.";
                    return -1;
                }
                rh->param.insert({param_pair[0], param_pair[1]});
            }
        }
    }
    else
    {
        PLOG(ERROR) << "http header wrong.";
        return -1;
    }

    // request line
    for (int i = 1; i < request_lines.size()-1; ++i)
    {
        std::vector<std::string> segs = split(request_lines[i], ": ");
        if(segs.size() == 2)
        {
            if(0 == segs[0].compare("Host"))
            {
                std::string tmp = strip_leading_char(segs[1], ' ');
                rh->host = tmp;
            }
            else if(0 == segs[0].compare("Connection"))
            {
                std::string tmp = strip_leading_char(segs[1], ' ');
                rh->connection = tmp;
            }
            else if (0 == segs[0].compare("Upgrade"))
            {
                std::string tmp = strip_leading_char(segs[1], ' ');
                rh->upgrade = tmp;
            } 
            else if (0 == segs[0].compare("Sec-WebSocket-Key"))
            {
                std::string tmp = strip_leading_char(segs[1], ' ');
                rh->sec_websocket_key = tmp;
            }
        }
    }
    // last line is empty
    return 0;
}

std::vector<std::string> OnHttpRequest::split(const std::string& str, std::string separator)
{
    if(str.length() <= separator.length())
    {
        return {};
    }
    std::vector<std::string> results;
    int begin = 0;
    for(int i = 0; i <= str.length()-separator.length(); ++i)
    {
        if(0 == str.compare(i,separator.length(),separator))
        {
            results.push_back(str.substr(begin, i-begin));
            begin = i + separator.length();
        }
    }
    // last segment
    if(0 != begin && begin < str.length())
    {
        results.push_back(str.substr(begin));
    }
    return results;
}

std::string OnHttpRequest::strip_leading_char(const std::string& str, const char lead)
{
    int i = 0;
    for(; i < str.length(); ++i)
    {
        if(str[i] != lead) break;
    }
    return str.substr(i);
}

} // namespace wenet