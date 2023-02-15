#ifndef GROUP_H_
#define GROUP_H_

#include <string>
#include <vector>
#include <unordered_map>

namespace wenet{

class Participant;

//
//  each uuid has a Group that manage socket client and websocket client with same uuid. 
//
class Group{
public:
    void Join(Participant* client);
    int Leave(Participant* client);
    void BroadcastMessage(const std::string& message);
    int Size(){return clients_.size(); }
    int set_current_on_microphone_(Participant* pa);
    Participant* get_current_on_microphone_(){return current_on_microphone_;}
private:
    std::vector<Participant*> clients_;
    Participant* current_on_microphone_ = nullptr;          // which client use mic now.
};

//
//  singleton.
//
class GroupManager{
private:
    GroupManager(){
        
    }
    ~GroupManager(){}
    GroupManager(const GroupManager&);
    // GroupManager& operator=(const GroupManager&);
public:
    static GroupManager& Instance(){
        static GroupManager instance;
        return instance;
    }
    int JoinGroup(string uuid, Participant* pa);
    int JoinNewGroup(Participant* pa);
    int LeaveGroup(string uuid, Participant* pa);
    std::string GenerateUuid();
private:
    std::unordered_map<std::string, Group*> uuid_map_ = {};
};



} // namespace wenet

#endif  /* GROUP_H_ */