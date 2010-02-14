#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include "SDL.h"

class Resources
{
    public:
        Resources();
        ~Resources();

        void prepareBoardGraphics(float proportion);
        SDL_Surface *screen();
        SDL_Surface *mainMenu();
        SDL_Surface *pieces();
        SDL_Surface *board();
        SDL_Surface *player();
        SDL_Surface *gameOver();
        void getScreenSize(int &width, int &height);
        int getPieceSize();

    private:
        void prepareBGGraphics();

        SDL_Surface *_screen;
        SDL_Surface *_origMMenu;
        SDL_Surface *_mMenu;
        SDL_Surface *_origPieces;
        SDL_Surface *_pieces;
        SDL_Surface *_origBoard;
        SDL_Surface *_board;
        SDL_Surface *_origPlayer;
        SDL_Surface *_player;
        SDL_Surface *_origGOver;
        SDL_Surface *_gOver;
        int _currPieceSize;
};

#endif //RESOURCES_HPP
