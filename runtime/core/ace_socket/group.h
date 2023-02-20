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
    void Join(Participant* cl);
    int Leave(Participant* cl);
    void BroadcastMessage(const std::string& message);
    int Size(){return clients_.size(); }
    int SetGroupLeader(Participant* pa);
private:
    std::set<Participant*> clients_;
};

//
//  singleton.
//
class GroupManager{
private:
    GroupManager(){
    }
    ~GroupManager(){}
    // GroupManager(const GroupManager&);
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