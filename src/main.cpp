#include "gui/MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    scapms::MainWindow window;
    window.show();
    return app.exec();
}
