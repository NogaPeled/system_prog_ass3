#include "CoupGUI.hpp"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    CoupGUI gui;
    gui.show();
    return app.exec();
}
