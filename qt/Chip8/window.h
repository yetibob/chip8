#ifndef WINDOW_H
#define WINDOW_H

#include "chip8.h"
#include "sdlcanvas.h"

#include <QMainWindow>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class window; }
QT_END_NAMESPACE

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(int scale, QWidget *parent = nullptr);
    ~Window();

    QString rom;
    void setCanvas(SDLCanvas *);
private slots:
    void setRomDirectory();
    void selectRom();
    void launchEmu();

private:
    Ui::window *ui;
    int scale;

    void createActions();
    void createMenus();

    QMenu *fileMenu;
    QAction *setRomDir;
    QAction *setRom;
    QAction *launch;
    // QLabel *infoLabel;

    QString romDir;
};
#endif // WINDOW_H
