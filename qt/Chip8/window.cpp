#include "emucanvas.h"
#include "window.h"
#include "ui_window.h"

#include <QFileDialog>
#include <QVBoxLayout>

Window::Window(int scale, QWidget *parent) : QMainWindow(parent) , ui(new Ui::window), scale(scale) {
    romDir = QDir::homePath() + "/.chip8/roms";
    rom = romDir + "/INVADERS";
    QString message = qPrintable(rom);
    statusBar()->showMessage(message);

    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    // QWidget *topFiller = new QWidget;
    // topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // infoLabel = new QLabel(tr("<i>Select Rom</i>"));
    // infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    // infoLabel->setAlignment(Qt::AlignCenter);

    // QWidget *bottomFiller = new QWidget;
    // bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // QVBoxLayout *layout = new QVBoxLayout;
    // layout->setContentsMargins(5, 5, 5, 5);
    // layout->addWidget(topFiller);
    // layout->addWidget(bottomFiller);

    // widget->setLayout(layout);

    createActions();
    createMenus();
    int addHeight = 3*scale;
    setFixedSize(D_WIDTH*scale, D_HEIGHT*scale+addHeight);
    // ui->setupUi(this);
}

Window::~Window()
{
    delete ui;
}

void Window::setCanvas(SDLCanvas *sdlCanvas) {
   canvas = sdlCanvas;
}

void Window::setRomDirectory() {
    romDir = QFileDialog::getExistingDirectory(this, tr("Rom Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly);
    QString message = tr(qPrintable(romDir));
    statusBar()->showMessage(message);
}

void Window::selectRom() {
    rom = QFileDialog::getOpenFileName(this, tr("Rom"), romDir);
    QString message = tr(qPrintable(rom));
    statusBar()->showMessage(message);
}

void Window::launchEmu() {
    // TODO:INVESTIGATE
    // Alright. This is where things get tricky. Either figure out how to create an OpenGL OR SDL context
    // in QT or launch as separate process...maybe try out both?
    // Single Process:
    //     How to include emulator
    //         Include in compilation?
    //         Static/Shared/Dynamic Library type thing?
    //         Is there some way to to dynamic/runtime loading?

    // This below implementation is kind of garbage. The SDL loop blocks QT completely.
    // The model of calling the Chip8 emu "as is" works better as a separate process using some kind of IPC
    // The best bet for doing it this bad way is to either completely rewrite Chip8 to use QT/OpenGL directly
    // OR heavily modify the Chip8 emulator to play well with QT and live in EmuCanvas (IE, get rid of emu.run()
    // and have the emu loop occur via EmuCanvas::OnUpdate()

    EmuCanvas canvas(centralWidget(), pos(), QSize(D_WIDTH*scale, D_HEIGHT*scale));
    canvas.show();

    // Multi Process:
    //     Will need IPC to enable debugging and other currently unknown features
}

void Window::createActions() {
    setRomDir = new QAction(tr("&Set Rom Directory"), this);
    setRomDir->setShortcut(QKeySequence(tr("Ctrl+Shift+R")));
    setRomDir->setStatusTip(tr("Set Rom Directory"));
    connect(setRomDir, &QAction::triggered, this, &Window::setRomDirectory);

    setRom = new QAction(tr("&Select Rom"), this);
    setRom->setShortcut(QKeySequence(tr("Ctrl+R")));
    setRom->setStatusTip(tr("Select Rom"));
    connect(setRom, &QAction::triggered, this, &Window::selectRom);

    launch = new QAction(tr("&Launch Emualator"), this);
    launch->setShortcut(QKeySequence(tr("Ctrl+L")));
    launch->setStatusTip(tr("Starts Emulator with chosen rom"));
    connect(launch, &QAction::triggered, this, &Window::launchEmu);
}

void Window::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(setRomDir);
    fileMenu->addAction(setRom);
    fileMenu->addAction(launch);
    fileMenu->addSeparator();
}
