#include "emucanvas.h"

void EmuCanvas::OnInit()
{
    emu = new Chip8{ rom.toStdString(), scale, window };
    if (loaded == false) {
        std::cout << "No rom loaded. Please load rom and try again"
                  << std::endl;
        return;
    }
    emu->running = true;
}

// TODO:INVESTIGATE
// Figure out what to do about delta time
// Figure out how to hide canvas on destroy
void EmuCanvas::OnUpdate()
{
    auto prev = std::chrono::steady_clock::now();

    double delta;
    double rate{ 1000 / 60 };

    if (emu->running) {
        auto                          now  = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = now - prev;

        // TODO:INVESTIGATE
        // Is this getting me the ms between frames???
        delta = diff.count() * 1000;
        prev  = now;

        emu->handleEvents();
        emu->handleTimers(delta, rate);
        emu->handleOp();
        emu->draw();

    } else {
        emu->destroy();
    }
}
