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
#include "SDL_net.h"
#include "Board.hpp"

#define MAX_PACKET_SIZE 512

enum PacketType
{
    PTConnReq = 0,
    PTConnEnd = 1,
    PTState = 2
};

struct Packet
{
    PacketType type;
    union
    {
        bool connOk;
        Board::State state;
    };
};

class Server
{
    public:
        Server();
        ~Server();

        void listen();

    private:
        static const int maxClients;

        void procPacket();

        UDPsocket _socket;
        SDLNet_SocketSet _socketSet;
        UDPpacket *_packet;
        std::vector<IPaddress> _clients;
};

class Client
{
    public:
        Client();
        ~Client();

        void connect();
        void listen();

    private:
        void preparePacket(const Packet &p);
        void sendPacket();
        void initSocket();

        UDPsocket _socket;
        SDLNet_SocketSet _socketSet;
        UDPpacket *_packet;
        IPaddress _ipServer;
};

#endif //NET_HPP
