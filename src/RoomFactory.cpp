#include <Ice/Config.h>
#include "../include/Room.h"
#include "../include/RoomFactory.h"

RoomFactory::RoomFactory()
        :m_numberOfChats(0)
{}

::Ice::Int RoomFactory::getCPULoad(const Ice::Current &current) {
    return m_numberOfChats;
}

void RoomFactory::removeRoom(const std::string &name, const Ice::Current &current) {
    if(!isRoomOnList(name)) {
        for(auto & i : m_list){
            if(i->getName()==name){
                std::cout<<i<<"Ilosc elementow przed"<<m_numberOfChats<<"\n";
                m_list.erase(std::remove(m_list.begin(),m_list.end(),i),m_list.end());
                m_numberOfChats--;
                std::cout<<i<<"Ilosc elementow po"<<m_numberOfChats<<"\n";
                return;
            }
        }
    }else {
        throw Chat::RoomNotExists();
    }
}

Chat::RoomPrx RoomFactory::addRoom(const std::string &name, const Ice::Current &current) {
    if(!isRoomOnList(name)) {
        Chat::RoomPtr roomPtr = new Room(name);
        Chat::RoomPrx roomPrx = Chat::RoomPrx::uncheckedCast(current.adapter->addWithUUID(roomPtr));
        m_numberOfChats++;
        m_list.push_back(roomPrx);
        return roomPrx;
    }else {
        throw Chat::RoomAlreadyExists();
    }
}

bool RoomFactory::isRoomOnList(const std::string &name) {
    for(auto & i : m_list){
        if(i->getName()==name){
            return true;
        }
    }
    return false;
}

