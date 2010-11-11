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
#include <cassert>
#include "Net.hpp"

namespace
{
    const int serverPort = 8888;
    const int clientPort = 8887;
    const std::string serversFName("servers");

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

    unsigned int
    timerCB(unsigned int interval, void *param)
    {
        SDL_Event event;
        event.type = EVT_CHECK_NET;
        SDL_PushEvent(&event);

        return interval;
    }

    void
    readServerFromFile(const std::string &serversFile, std::string &server,
            int &port)
    {
        std::ifstream fs(serversFile.c_str());
        if (fs.fail()) //if it doesn't exist
        {
            server = std::string("cubata.homelinux.net");
            port = serverPort;
        }
        else
        {
            fs >> server;
            fs >> port;
            //TODO: enable more than one server
#if 0
            while (fs.good())
            {
                std::string line;
                getline(fs,line);

                if (!fs.good())
                    break;
            }
#endif
        }
    }
}

NetLogger::NetLogger(const std::string &logFile):
    _fs(logFile.c_str())
{
}

NetLogger &
NetLogger::operator()(const std::string &msg)
{
    _fs << time(NULL) << ": " << msg << std::endl;

    return *this;
}

const int Server::maxClients = 2;

Server::Server():
    _playing(false),
    _log("server.log")
{
    initSDLNet();
	_socket = SDLNet_UDP_Open(serverPort);
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
		int num = SDLNet_CheckSockets(_socketSet, 5000);
        if (num)
        {
            //this is redundant now, as we only have 1 socket
            if (SDLNet_SocketReady(_socket))
            {
                SDLNet_UDP_Recv(_socket, _packet);
                procPacket();
            }
        }
    }
}

void
Server::sendPacket()
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
Server::procPacket()
{
    Packet *p = reinterpret_cast<Packet *>(_packet->data);
    switch (p->type)
    {
        case PTConnReq:
            if (p->protocol != PROTOCOL)
                p->protocol = ECOldProtocol;
            else
            {
                p->protocol = ECFull;
                if (int(_clients.size()) < maxClients)
                {
                    p->id = _clients.size();
                    _clients.push_back(_packet->address);
                    std::ostringstream ss;
                    ss << "New client from " << _packet->address.host
                        << "; Clients: " << _clients.size();
                    _log(ss.str());
                    p->protocol = ECNoError;
                }
            }
            p->time = time(NULL);
            sendPacket();
            if (!_playing && int(_clients.size()) == maxClients)
            {
                startGame();
            }
            break;
        case PTConnEnd:
            break;
        case PTStartGame:
            break;
        case PTState:
            relayPacket();
            break;
        default:
            throw std::runtime_error("Unknown packet rcvd!");
    }
}

void
Server::startGame()
{
    Packet *p = reinterpret_cast<Packet *>(_packet->data);
    p->time = time(NULL);
    p->type = PTStartGame;
    for (unsigned int i = 0; i < _clients.size(); ++i)
    {
        _packet->address = _clients[i];
        sendPacket();
    }

    _gOver.resize(_clients.size(), false);
    _playing = true;
    _log("Game started");
}

void
Server::gameOver()
{
    _clients.clear();
    _gOver.clear();
    _playing = false;
    _log("Game over");
}

void
Server::relayPacket()
{
    Packet *p = reinterpret_cast<Packet *>(_packet->data);
    const int id = p->id;
    assert (id >= 0 && id < int(_clients.size()));

    for (unsigned int i = 0; i < _clients.size(); ++i)
    {
        if (id == int(i)) //don't relay to oneself
            continue;
        _packet->address = _clients[i];
        sendPacket();
    }

    //TODO: temporary way
    if (p->state.gameOver)
    {
        _gOver[id] = true;
        bool gOver = true;
        for (unsigned int i = 0; i < _gOver.size(); ++i)
            if (!_gOver[i])
                gOver = false;
        if (gOver)
            gameOver();
    }
}

Client::Client():
    _socket(0),
    _socketSet(0),
    _id(-1),
    _connected(false),
    _timerID(0),
    _startGame(false),
    _newStatePend(false)
{
    initSDLNet();
    _packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    initSocket();
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
Client::newGame()
{
    _connected = false;
    _startGame = false;
    _newStatePend = false;
}

void
Client::preparePacket(const Packet &p)
{
    _packet->channel = -1; //no channel
    const size_t pLen = sizeof(p);
    memcpy(_packet->data, &p, pLen);
    _packet->len = pLen;
    if (_packet->len > _packet->maxlen)
        throw std::runtime_error("Error: Packet too big!");
    _packet->address = _ipServer;
}

void
Client::sendPacket()
{
    Packet *p = reinterpret_cast<Packet *>(_packet->data);
    p->time = time(NULL);
    p->id = _id;
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
	_socket = SDLNet_UDP_Open(clientPort);
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

bool
Client::connect()
{
    const std::string serversFile = getPath(FolderConf) + serversFName;
    std::string server;
    int port;
    readServerFromFile(serversFile, server, port);
    std::cout << "Connecting to " << server << ":" << port << " ..." <<
        std::endl;
    if (SDLNet_ResolveHost(&_ipServer, server.c_str(), port) == -1)
        throw std::runtime_error("Error resolving server IP");
    Packet p;
    p.type = PTConnReq;
    p.protocol = PROTOCOL;
    preparePacket(p);
    sendPacket();
    listen(5000); //wait for up to 5 sec. for connection
    return _connected;
}

bool
Client::listen(int timeout)
{
    int num = SDLNet_CheckSockets(_socketSet, timeout);
    if (num)
    {
        //this is redundant now, as we only have 1 socket
        if (SDLNet_SocketReady(_socket))
        {
            SDLNet_UDP_Recv(_socket, _packet);
            procPacket();
            //std::string str((const char *)_packet->data, _packet->len);
            //SDLNet_UDP_Send(_socket, -1, _packet);
        }
        return true;
    }
    return false;
}

bool
Client::startGame()
{
    return _startGame;
}

void
Client::sendState(const Board::State &state)
{
    Packet p;
    p.type = PTState;
    p.state = state;
    preparePacket(p);
    sendPacket();
}

bool
Client::newState(Board::State &state)
{
    if (_newStatePend)
    {
        state = _newState;
        _newStatePend = false;
        return true;
    }

    return false;
}

void
Client::procPacket()
{
    Packet *p = reinterpret_cast<Packet *>(_packet->data);
    switch (p->type)
    {
        case PTConnReq:
            if (p->protocol == ECNoError)
            {
                _id = p->id;
                _connected = true;
                _timerID = SDL_AddTimer(100, timerCB, NULL);
                if (!_timerID)
                {
                    std::ostringstream msg;
                    msg << "Error creating timer: " << SDL_GetError();
                    throw std::runtime_error(msg.str());
                }
            }
            else
            {
                //TODO: deal with errors
                std::cout << "Error connecting" << std::endl;
            }
            break;
        case PTConnEnd:
            break;
        case PTStartGame:
            _startGame = true;
            break;
        case PTState:
            _newState = p->state;
            _newStatePend = true;
            break;
        default:
            throw std::runtime_error("Unknown packet rcvd!");
    }
}
