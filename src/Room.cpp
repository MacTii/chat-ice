#include "../include/User.h"
#include "../include/Room.h"

Room::Room(std::string name)
    :m_name(std::move(name))
{}

Chat::userList Room::presentUsers(const Ice::Current &) {
    return m_userList;
}

void Room::sendMessage(const std::string &message, const Chat::UserPrx &sender, const Ice::Current &current) {
    for (auto& chatUserPrx : m_userList)
    {
        Chat::RoomPtr roomPtr = this;
        Chat::RoomPrx roomPrx = Chat::RoomPrx::checkedCast(current.adapter->addWithUUID(roomPtr));
        chatUserPrx->receiveMessage(message,sender,roomPrx);
    }
}

void Room::addUser(const Chat::UserPrx &who, const Ice::Current &current) {
    m_userList.push_back(who);
    std::cout << "Uzytkownik " << who->getName() << " dolaczyl" << std::endl;
}

void Room::removeUser(const Chat::UserPrx &who, const Ice::Current &current) {
    if(isUserOnList(who->getName(),current)) {
        for (auto &i : m_userList) {
            if (i->getName() == who->getName()) {
                m_userList.erase(m_userList.begin() + i);
                break;
            }
        }
    }else{
        throw Chat::UserNotExists();
    }
}

std::string Room::getName(const Ice::Current &) {
    return m_name;
}

bool Room::isUserOnList(const std::string &name, const Ice::Current &current) {
    for(auto & i : m_userList){
        if(i->getName()==name){
            return true;
        }
    }
    return false;
}