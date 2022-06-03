#ifndef EMUCANVAS_H
#define EMUCANVAS_H

#include <iostream>

#include "chip8.h"
#include "sdlcanvas.h"

class EmuCanvas : public SDLCanvas
{
public:
    EmuCanvas(QWidget* Parent, const QPoint& Position, const QSize& Size) :
        SDLCanvas(Parent, Position, Size)
    {

    }

private :
    Chip8 *emu;
    QString rom;

    int scale;
    bool loaded;
    bool running;

    void OnInit();
    void OnUpdate();
};

#endif // EMUCANVAS_H
