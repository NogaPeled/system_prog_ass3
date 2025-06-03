//nogapeled19@gmail.com
/**
 * @file main.cpp
 * @brief Entry point for the Coup GUI application.
 *
 * This file initializes the Qt application and launches the main GUI window.
 */

#include "CoupGUI.hpp"
#include <QApplication>

int main(int argc, char* argv[]) {
    // Initialize Qt application with command-line arguments
    QApplication app(argc, argv);

    // Create and display the main GUI window
    CoupGUI gui;
    gui.show();

    // Start the Qt event loop
    return app.exec();
}
