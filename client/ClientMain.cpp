
#include <regex>
#include "../include/User.h"
#include "ClientMain.h"

int ClientMain::run(int argc, char **argv) {
    Ice::PropertiesPtr properties = communicator()->getProperties();
    int serverPort = properties->getPropertyAsIntWithDefault("server.port.set", 49152);
    Ice::ObjectPrx proxy = communicator()->stringToProxy("server:default -p " + std::to_string(serverPort));
    m_serverPrx = Chat::ServerPrx::uncheckedCast(proxy);
    int portMin = properties->getPropertyAsIntWithDefault("client.port.range.min", 55555);
    int portMax = properties->getPropertyAsIntWithDefault("client.port.range.max", 65535);
    m_adapterPtr = getAdapter(portMin, portMax);
    m_adapterPtr->activate();
    createUser();
    listen();
    if (m_roomPrx != ICE_NULLPTR) {
        m_roomPrx->removeUser(m_userPrx);
    }
    communicator()->shutdown();
    communicator()->waitForShutdown();

    return EXIT_SUCCESS;
}

Ice::ObjectAdapterPtr ClientMain::getAdapter(int portMin, int portMax) {
    while (true)
    {
        srand(time(nullptr));
        int port = portMin + rand() % (portMax - portMin);
        // std::cout << port << std::endl;
        try
        {
            Ice::ObjectAdapterPtr ptr = communicator()->createObjectAdapterWithEndpoints(
                    "client_adapter",
                    "default -p " + std::to_string(port));
            return ptr;
        }
        catch (Ice::SocketException ex)
        {
            continue;
        }
    }
}

void ClientMain::listen() {
    std::cout << "Napisz /help aby sprawdzic wszystkie komendy" << std::endl;
    for(;;)
    {
        std::string command;
        getline(std::cin, command);
        std::vector<std::string> tokenList = buffer(command);
        command = tokenList.at(0);
        if (command == "/help")
        {
            showHelp();
        }
        else if (command == "/rooms")
        {
            showRoomList();
        }
        else if (command == "/users")
        {
            showUserList();
        }
        else if (command == "/leave")
        {
            leaveRoom();
        }
        else if (command == "/join")
        {
            joinRoom(tokenList[1]);
        }
        else if (command == "/create")
        {
            createRoom(tokenList[1]);
        }
        else if (command == "/msgprv")
        {
            std::vector<std::string> args = buffer(tokenList[1]);
            sendPrivateMessage(args[0], args[1]);
        }
        else if (command == "/msg")
        {
            sendMessage(tokenList[1]);
        }
        else if (command == "/delete")
        {
            deleteRoom(tokenList[1]);
        }
        else if (command == "/quit")
        {
            break;
        }
        else if (!command.empty())
        {
            std::cout << "Nie ma takiej komendy" << std::endl;
        }
    }
}

void ClientMain::showHelp() {
    std::cout << "Lista dostepnych komend:" << std::endl;
    std::cout << "/join <RoomName> - dolacz do pokoju" << std::endl;
    std::cout << "/create <RoomName> - tworzy kanal tekstowy" << std::endl;
    std::cout << "/delete <RoomName> - usuwa kanal tekstowy" << std::endl;
    std::cout << "/rooms - wyswietla dostepna liczbe kanalow" << std::endl;
    std::cout << "/leave - wyjscie z kanalu tekstowego" << std::endl;
    std::cout << "/users - lista wszystkich uzytkownikow na czacie" << std::endl;
    std::cout << "/msg <message> - wysyla wiadomosc do kazdego na kanale" << std::endl;
    std::cout << "/msgprv <user> <message> - wysyla wiadomosc prywatna do uzytkownika" << std::endl;
    std::cout << "/help - lista dostepnych komend" << std::endl;
    std::cout << "/quit - rozlacza z kanalu i wychodzi z aplikacji" << std::endl;
}

void ClientMain::leaveRoom() {
    if (m_roomPrx == ICE_NULLPTR) {
        std::cerr << "Nie polaczyles sie z zadnym kanalem" << std::endl;
        return;
    }
    std::string name = m_roomPrx->getName();
    m_roomPrx->removeUser(m_userPrx);
    m_roomPrx = ICE_NULLPTR;
    std::cout << "Wyszedles z pokoju " << name << std::endl;
}

void ClientMain::joinRoom(std::string &name) {
    if (name.empty())
    {
        std::cerr << "Kanal nie zostal zdefiniowany" << std::endl;
        return;
    }
    if (m_roomPrx != ICE_NULLPTR && m_roomPrx->getName() == name)
    {
        std::cerr << "Jestes polaczony z tym kanalem" << std::endl;
        return;
    }
    Chat::roomList roomList = m_serverPrx->getRooms();
    for (auto it = std::begin(roomList); it != std::end(roomList); ++it)
    {
        Chat::RoomPrx room = *it;
        if (room->getName() == name)
        {
            if (m_roomPrx != ICE_NULLPTR)
            {
                leaveRoom();
            }
            std::cout << m_userPrx->getName() << "\n";
            room->addUser(m_userPrx);
            m_roomPrx = room;
            std::cout << "Dolaczyles do pokoju " << name << std::endl;
            return;
        }
    }
    std::cerr << "Kanal " << name << " nie znaleziony" << std::endl;
}

void ClientMain::createRoom(std::string &name) {
    if (name.empty())
    {
        std::cerr << "Nazwa kanalu nie zdefiniowana" << std::endl;
        return;
    }
    try
    {
        m_serverPrx->addRoom(name);
        std::cout << "Kanal " << name << " stworzony, dolacz /join " << name << " komenda" << std::endl;
    }
    catch (Chat::RoomAlreadyExists ex)
    {
        std::cerr << "Czat z ta nazwa istnieje, wybierz inny" << std::endl;
    }
}

void ClientMain::sendPrivateMessage(std::string &username, std::string &message) {
    if (m_roomPrx == ICE_NULLPTR) {
        std::cerr << "Nie polaczyles z zadnym kanalem" << std::endl;
        return;
    }
    if (username.empty()) {
        std::cerr << "Nazwa uzytkownika nie zdefiniowana" << std::endl;
        return;
    }
    if (message.empty()) {
        std::cerr << "Wiadomosc nie zdefiniowana" << std::endl;
        return;
    }
    if (username == m_userPrx->getName()) {
        std::cerr << "Nie mozesz wyslac wiadomosci do siebie" << std::endl;
        return;
    }
    Chat::userList userList = m_roomPrx->presentUsers();
    for (auto it = std::begin(userList); it != std::end(userList); ++it) {
        Chat::UserPrx user = *it;
        if (user->getName() == username) {
            user->receivePrivateMessage(message, m_userPrx);
            return;
        }
    }
    std::cerr << "Uzytkownik " << username << " nie znaleziony" << std::endl;
}

void ClientMain::sendMessage(std::string &message) {
    if (m_roomPrx == ICE_NULLPTR) {
        std::cerr << "Nie polaczyles sie z zadnym kanalem" << std::endl;
        return;
    }
    if (message.empty()) {
        std::cerr << "Wiadomosc nie zdefiniowana" << std::endl;
        return;
    }
    Chat::userList userList = m_roomPrx->presentUsers();
    for (auto it = std::begin(userList); it != std::end(userList); ++it) {
        Chat::UserPrx user = *it;
        user->receiveMessage(message, m_userPrx, m_roomPrx);
    }
}

void ClientMain::showRoomList() {
    Chat::roomList roomList = m_serverPrx->getRooms();
    if (roomList.empty())
    {
        std::cout << "Nie ma dostepnych kanalow" << std::endl;
        return;
    }
    std::cout << "Dostepne kanaly:" << std::endl;
    for (auto it = std::begin(roomList); it != std::end(roomList); ++it)
    {
        Chat::RoomPrx room = *it;
        std::cout << "Kanal: " << room->getName() << ", Uzytkownicy: " << room->presentUsers().size() << std::endl;
    }
}

void ClientMain::showUserList() {
    if (m_roomPrx == ICE_NULLPTR)
    {
        std::cerr << "Nie polaczyles sie z zadnym kanalem" << std::endl;
        return;
    }
    Chat::userList userList = m_roomPrx->presentUsers();
    if (userList.size() == 1)
    {
        std::cout << "Jestes sam" << std::endl;
        return;
    }
    std::cout << "Dostepni uzytkownicy:" << std::endl;
    for (auto it = std::begin(userList); it != std::end(userList); ++it)
    {
        Chat::UserPrx user = *it;
        std::cout << user<<"\n";
        std::cout << user->getName() << std::endl;
    }
}

std::vector<std::string> ClientMain::buffer(std::string &command) {
    command = std::regex_replace(command, std::regex("^ +| +$|( ) +"), "$1");
    std::vector<std::string> tokenList(2);
    int pos = command.find(" ");
    tokenList[0] = command.substr(0, pos);
    if (pos != std::string::npos)
    {
        tokenList[1] = command.substr(pos + 1);
    }
    return tokenList;
}

Chat::UserPrx ClientMain::createUser() {
    std::string username;
    std::cout << "Wybierz uzytkownika" << std::endl;
    for (;;) {
        std::cout << "Nazwa uzytkownika: ";
        std::getline(std::cin, username);
        Chat::UserPtr userPtr = new User(username);
        m_userPrx = Chat::UserPrx::uncheckedCast(m_adapterPtr->addWithUUID(userPtr));

        return m_userPrx;
    }

}

void ClientMain::deleteRoom(std::string &name) {
    if (name.empty())
    {
        std::cerr << "Kanal nie zostal zdefiniowany" << std::endl;
        return;
    }
    try
    {
        m_serverPrx->removeRoom(name);
        std::cout << "Kanal " << name << " usuniety " << std::endl;
    }
    catch (Chat::RoomNotExists ex)
    {
        std::cerr << "Czat z ta nazwa istnieje, wybierz inny" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    ClientMain app;
    return app.main(argc, argv, "../chat.prop");
}