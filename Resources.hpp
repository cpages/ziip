#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include "SDL.h"

class Resources
{
    public:
        Resources();
        ~Resources();

        void prepareBoardGraphics(int newBlockSize);
        SDL_Surface *screen();
        SDL_Surface *mainMenu();
        SDL_Surface *mmNG();
        SDL_Surface *mmQ();
        SDL_Surface *mmSel();
        SDL_Surface *pieces();
        SDL_Surface *board();
        SDL_Surface *player();
        SDL_Surface *gameOver();
        void getScreenSize(int &width, int &height);
        int getBlockSize();

    private:
        void prepareBGGraphics();

        SDL_Surface *_screen;
        SDL_Surface *_origMMenu;
        SDL_Surface *_mMenu;
        SDL_Surface *_origMMNG;
        SDL_Surface *_mmNG;
        SDL_Surface *_origMMQ;
        SDL_Surface *_mmQ;
        SDL_Surface *_origMMSel;
        SDL_Surface *_mmSel;
        SDL_Surface *_origPieces;
        SDL_Surface *_pieces;
        SDL_Surface *_origBoard;
        SDL_Surface *_board;
        SDL_Surface *_origPlayer;
        SDL_Surface *_player;
        SDL_Surface *_origGOver;
        SDL_Surface *_gOver;
        int _currBlockSize;
};

#endif //RESOURCES_HPP
