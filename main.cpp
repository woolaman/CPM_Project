#include "Mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("CPM: crystal position map program");
    window.show();
    return app.exec();
}
