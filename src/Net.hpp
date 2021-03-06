/*
    Copyright 2010 Carles Pagès

    This file is part of Ziip.

    Ziip is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ziip is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ziip.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef NET_HPP
#define NET_HPP

#include <vector>
#include <ctime>
#include <fstream>
#include "SDL_net.h"
#include "Board.hpp"

#define MAX_PACKET_SIZE 512
#define PROTOCOL 1

enum PacketType
{
    PTConnReq = 0,
    PTConnEnd = 1,
    PTStartGame = 2,
    PTState = 3
};

enum ErrorConnection
{
    ECNoError = 0,
    ECOldProtocol = -1,
    ECFull = -2
};

struct Packet
{
    long long time; //time_t depends on x86/x86_64, long long is always 64 bits
    int id;
    PacketType type;
    union
    {
        int protocol;
        Board::State state;
    };
};

class NetLogger
{
    public:
        NetLogger(const std::string &logFile);

        NetLogger& operator()(const std::string &msg);

    private:
        std::ofstream _fs;
};

class Server
{
    public:
        Server();
        ~Server();

        void listen();

    private:
        static const int maxClients;

        void sendPacket();
        void procPacket();
        void startGame();
        void gameOver();
        void relayPacket();

        UDPsocket _socket;
        SDLNet_SocketSet _socketSet;
        UDPpacket *_packet;
        std::vector<IPaddress> _clients;
        std::vector<bool> _gOver;
        bool _playing;
        NetLogger _log;
};

class Client
{
    public:
        Client();
        ~Client();

        void newGame();
        bool connect();
        bool listen(int timeout);
        bool startGame();
        void sendState(const Board::State &state);
        bool newState(Board::State &state);

    private:
        void preparePacket(const Packet &p);
        void sendPacket();
        void initSocket();
        void procPacket();

        UDPsocket _socket;
        SDLNet_SocketSet _socketSet;
        UDPpacket *_packet;
        IPaddress _ipServer;
        int _id;
        bool _connected;
        SDL_TimerID _timerID;
        bool _startGame;
        Board::State _newState;
        bool _newStatePend;
};

#endif //NET_HPP
