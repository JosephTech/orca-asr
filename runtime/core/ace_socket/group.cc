
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
        // multiple clients
        uuid_map_[uuid]->Join(pa);
        pa->set_uuid_(uuid);
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
    group->set_current_on_microphone_(pa);
    uuid_map_.insert({uuid, group});
    pa->set_uuid_(uuid);
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
        }
        uuid_map_.erase(uuid);
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

int Group::set_current_on_microphone_(Participant* pa)
{
    for(auto c : clients_)
    {
        if(c == pa)
        {
            current_on_microphone_ = pa;
            return 0;
        } 
    }
    return -1;
}

void Group::Join(Participant* client){
    clients_.push_back(client);
    client->set_group_(this);
}

int Group::Leave(Participant* client){
    for(auto iter = clients_.begin(); iter != clients_.end(); ++iter)
    {
        if (*iter == client)
        {
            clients_.erase(iter);   // Participant handle_close() delete client*.
            printf("INFO remove client %p", client);
            return 0;
        }
    }
    return -1;
}

void Group::BroadcastMessage(const std::string& message){
    for(auto cl : clients_)
    {
        PLOG(INFO) << "此处需要通过client的socket() send()向所有客户端广播消息";
    }
}

} // namespace wenet