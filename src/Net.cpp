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

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include "Net.hpp"

namespace
{
    void
    initSDLNet()
    {
        if (SDLNet_Init() == -1)
        {
            std::ostringstream msg;
            msg << "Could not initialize SDL_net: " << SDLNet_GetError();
            throw std::runtime_error(msg.str());
        }
    }
}

const int Server::maxClients = 2;

Server::Server()
{
    initSDLNet();
	_socket = SDLNet_UDP_Open(8888);
	if (_socket == NULL )
        throw std::runtime_error("Err!");
	_socketSet = SDLNet_AllocSocketSet(1);
	if ( _socketSet == NULL )
        throw std::runtime_error("Err!");
	SDLNet_UDP_AddSocket(_socketSet, _socket);
    _packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
}

Server::~Server()
{
    SDLNet_FreeSocketSet(_socketSet);
    SDLNet_UDP_Close(_socket);
    SDLNet_FreePacket(_packet);
    SDLNet_Quit();
}

void
Server::listen()
{
	while (1)
    {
		/* Wait for events */
		int num = SDLNet_CheckSockets(_socketSet, 5000);
        if (!num)
            std::cout << "Timeout!" << std::endl;
        else
        {
            /* Check for new connections */
            if ( SDLNet_SocketReady(_socket) ) {
                std::cout << "Incom!!!" << std::endl;
                SDLNet_UDP_Recv(_socket, _packet);
                procPacket();
                //std::string str((const char *)_packet->data, _packet->len);
                //std::cout << str << std::endl;
                //SDLNet_UDP_Send(_socket, -1, _packet);
            }
        }
    }
}

void
Server::procPacket()
{
    Packet *p = (Packet *)_packet->data;
    switch (p->type)
    {
        case PTConnReq:
            std::cout << "Conn req.!" << std::endl;
            p->connOk = false;
            if (_clients.size() < maxClients)
            {
                _clients.push_back(_packet->address);
                p->connOk = true;
            }
            _packet->len = sizeof(*p);
            SDLNet_UDP_Send(_socket, -1, _packet);
            break;
        case PTConnEnd:
            std::cout << "Conn end" << std::endl;
            break;
        case PTState:
            std::cout << "State" << std::endl;
            break;
        default:
            throw std::runtime_error("Unknown packet rcvd!");
    }
}

Client::Client():
    _socket(0),
    _socketSet(0)
{
    initSDLNet();
    _packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
}

Client::~Client()
{
    SDLNet_FreePacket(_packet);
    if (_socketSet)
        SDLNet_FreeSocketSet(_socketSet);
    if (_socket)
        SDLNet_UDP_Close(_socket);
    SDLNet_Quit();
}

void
Client::preparePacket(const Packet &p)
{
    _packet->channel = -1; //no channel
    const size_t pLen = sizeof(p);
    memcpy(_packet->data, &p, pLen);
    _packet->len = sizeof(pLen);
    if (_packet->len > _packet->maxlen)
        throw std::runtime_error("Error: Packet too big!");
    _packet->address = _ipServer;
}

void
Client::sendPacket()
{
    int sent = SDLNet_UDP_Send(_socket, _packet->channel, _packet);
    if (!sent)
    {
        std::ostringstream msg;
        msg << "Error sending packet: " << SDLNet_GetError();
        throw std::runtime_error(msg.str());
    }
}

void
Client::initSocket()
{
	_socket = SDLNet_UDP_Open(8887);
	if (_socket == NULL)
    {
        std::ostringstream msg;
        msg << "Error opening UDP socket: " << SDLNet_GetError();
        throw std::runtime_error(msg.str());
    }
	_socketSet = SDLNet_AllocSocketSet(1);
	if (_socketSet == NULL)
    {
        std::ostringstream msg;
        msg << "Error allocating socket set: " << SDLNet_GetError();
        throw std::runtime_error(msg.str());
    }
	SDLNet_UDP_AddSocket(_socketSet, _socket);
}

void
Client::connect()
{
    initSocket();
    if (SDLNet_ResolveHost(&_ipServer, "localhost", 8888) == -1)
        throw std::runtime_error("Error resolving server IP");
    Packet p;
    p.type = PTConnReq;
    preparePacket(p);
    sendPacket();
}

void
Client::listen()
{
    /* Wait for events */
    int num = SDLNet_CheckSockets(_socketSet, 10);
    if (!num)
        std::cout << "Timeout!" << std::endl;
    else
    {
        /* Check for new connections */
        if ( SDLNet_SocketReady(_socket) ) {
            std::cout << "Incom!!!" << std::endl;
            SDLNet_UDP_Recv(_socket, _packet);
            Packet p;
            memcpy(&p, _packet->data, _packet->len);
            std::cout << p.type << std::endl;
            if (p.connOk)
                std::cout << "Conn. ok" << std::endl;
            else
                std::cout << "Conn. ref" << std::endl;
            //std::string str((const char *)_packet->data, _packet->len);
            //std::cout << str << std::endl;
            //SDLNet_UDP_Send(_socket, -1, _packet);
        }
    }
}
