#include <QApplication>

#include "chip8.h"
#include "emucanvas.h"
#include "window.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    int scale = 15;
    Window window{ scale };
    window.setWindowTitle("Chip 8 Emulator");
    window.show();

    return app.exec();
}
