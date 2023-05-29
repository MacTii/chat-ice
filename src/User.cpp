#include "../include/User.h"
#include <iostream>

User::User(std::string userName)
    :m_userName(userName)
{}
std::string User::getName(const Ice::Current &) {
    return m_userName;
}

void User::receiveMessage(const std::string &message, const Chat::UserPrx &sender, const Chat::RoomPrx &room, const Ice::Current &) {
    std::cout << "Kanal: " <<room->getName() << std::endl << "<" << sender->getName()<< ">: " << message << std::endl;
}

void User::receivePrivateMessage(const std::string &message, const Chat::UserPrx &sender, const Ice::Current &) {
    std::cout << "<" << sender->getName() << ">: " << message << std::endl;
}
