OBJS	= ClientMain.o Room.o Server.o User.o RoomFactory.o Chat.o
SOURCE	= ClientMain.cpp ../src/Room.cpp ../src/Server.cpp ../src/User.cpp ../src/RoomFactory.cpp ../src/Chat.cpp
HEADER	= ../include/Room.h ../include/Server.h ../include/User.h ../include/RoomFactory.h ../chat/Chat.h ClientMain.h
OUT	= client.out
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -pthread -lIce

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

ClientMain.o: ClientMain.cpp
	$(CC) $(FLAGS) ClientMain.cpp -std=c++11

Room.o: ../src/Room.cpp
	$(CC) $(FLAGS) ../src/Room.cpp -std=c++11

Server.o: ../src/Server.cpp
	$(CC) $(FLAGS) ../src/Server.cpp -std=c++11

User.o: ../src/User.cpp
	$(CC) $(FLAGS) ../src/User.cpp -std=c++11

RoomFactory.o: ../src/RoomFactory.cpp
	$(CC) $(FLAGS) ../src/RoomFactory.cpp -std=c++11

Chat.o: ../chat/Chat.cpp
	$(CC) $(FLAGS) ../chat/Chat.cpp -std=c++11


clean:
	rm -f $(OBJS) $(OUT)