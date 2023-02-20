
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "utils/log.h"
#include "ace_socket/group.h"
#include "ace_socket/participant.h"


namespace wenet{

int GroupManager::JoinGroup(string uuid, Participant* pa)
{
    if(uuid_map_.count(uuid))
    {
        PLOG(INFO) << "client join uuid is" <<  uuid;
        // multiple clients
        uuid_map_[uuid]->Join(pa);
        pa->set_uuid_(uuid);
        pa->get_hub_()->set_group_leader_(false);
        return 0;
    }
    return -1;
}

int GroupManager::JoinNewGroup(Participant* pa)
{
    // first client
    std::string uuid = GenerateUuid();
    Group* group = new Group();
    group->Join(pa);
    // on microphone. First join client grab the microphone default.
    group->SetGroupLeader(pa);
    uuid_map_.insert({uuid, group});
    PLOG(INFO) << "client join uuid is" <<  uuid;
    pa->set_uuid_(uuid);
    pa->get_hub_()->set_group_leader_(true);
    return 0;
}

int GroupManager::LeaveGroup(string uuid, Participant* pa)
{
    if(uuid_map_.count(uuid))
    {
        uuid_map_[uuid]->Leave(pa);
        if(0 == uuid_map_[uuid]->Size())
        {
            delete uuid_map_[uuid];
            uuid_map_[uuid] = nullptr;
            uuid_map_.erase(uuid);
        }
        return 0;
    }
    // possible peer closed when client didn't get to join group. 
    // in this case, uuid is -1.
    return -1;
}

std::string GroupManager::GenerateUuid()
{
    boost::uuids::random_generator gen;
    boost::uuids::uuid id = gen();
    return boost::uuids::to_string(id);
}

int Group::SetGroupLeader(Participant* pa)
{
    for(auto c : clients_)
    {
        if(c == pa)
        {
            PLOG(INFO) << "set group leader."<<pa->get_hub_()->get_hub_state_()->get_hub_state_();
            pa->get_hub_()->set_group_leader_(true);
        }
        else
        {
            pa->get_hub_()->set_group_leader_(false);
        }
    }
    return 0;
}

void Group::Join(Participant* cl){
    clients_.insert(cl);
    cl->set_group_(this);
}

int Group::Leave(Participant* cl){
    if(clients_.count(cl))
    {
        PLOG(INFO) << "remove client" << cl;
        cl->set_group_(nullptr);
        clients_.erase(cl);   // Participant handle_close() delete client*.
        return 0;
    }
    return -1;
}

//
//  send decode results
//
void Group::BroadcastMessage(const std::string& message){
    PLOG(INFO) << "clients_.size()"<< clients_.size();
    for(auto cl : clients_)
    {
        PLOG(INFO) << "此处需要通过client的socket()/websocket send()向所有客户端广播消息";
        if(cl->get_hub_()->is_on_socket_())
        {
            cl->socket().send(message.c_str(), message.length());
        }
        else if(cl->get_hub_()->is_on_websocket_() && cl->get_hub_()->get_hub_state_()->get_hub_state_() == kOnWebSocket)
        {
            cl->get_hub_()->get_on_websocket_state_()->SendText(message);
        }
    }
}

} // namespace wenet