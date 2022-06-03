#ifndef SDLCANVAS_H
#define SDLCANVAS_H

#include <SDL2/SDL.h>
#include <QWidget>
#include <QTimer>

class SDLCanvas : public QWidget
{
public:
    explicit SDLCanvas(QWidget* Parent, const QPoint& Position, const QSize& Size, unsigned int FrameTime = 0);
    // explicit SDLCanvas(QWidget *parent = nullptr);

    virtual ~SDLCanvas();
    SDL_Window *window;

private :
    virtual void OnInit() = 0;
    virtual void OnUpdate() = 0;
    virtual QPaintEngine* paintEngine() const;
    virtual void showEvent(QShowEvent*);
    virtual void paintEvent(QPaintEvent*);

    QTimer myTimer;
    bool   initialized;

};

#endif // SDLCANVAS_H
