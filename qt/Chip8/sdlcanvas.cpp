#include <iostream>
#include "sdlcanvas.h"

SDLCanvas::SDLCanvas(QWidget *parent, const QPoint &Position, const QSize &Size, unsigned int FrameTime) :
    QWidget(parent), initialized(false)
{
    // Setup some states to allow direct rendering into the widget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // Set strong focus to enable keyboard events to be received
    setFocusPolicy(Qt::StrongFocus);

    // Setup the widget geometry
    move(Position);
    resize(Size);

    // Setup the timer
    myTimer.setInterval(FrameTime);
}

SDLCanvas::~SDLCanvas()
{
}

void SDLCanvas::showEvent(QShowEvent *) {
    if (!initialized)
    {
        // Under X11, we need to flush the commands sent to the server to ensure that
        // SDL will get an updated view of the windows
        #ifdef Q_WS_X11
            XFlush(QX11Info::display());
        #endif

        // Create the SDL window with the widget handle
        window = SDL_GetWindowFromID(winId());

        // Let the derived class do its specific stuff
        OnInit();

        // TODO:INVESTIGATE
        // This was brought over from SFML and for some reason it seems to break things. Find out why (also why is this needed)
        // Setup the timer to trigger a refresh at specified framerate
        // connect(&myTimer, SIGNAL(timeout()), this, SLOT(repaint()));
        // myTimer.start();

        initialized = true;
    }
}

QPaintEngine* SDLCanvas::paintEngine() const {
    return nullptr;
}

void SDLCanvas::paintEvent(QPaintEvent *) {
    OnUpdate();

    // Display some ish
    // This is from the SFMLWidget instruction...update to however tf we do this with SDL
    // Display();
}
