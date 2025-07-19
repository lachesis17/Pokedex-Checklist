#include <QApplication>
#include "mainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("lachesis17's Pokédex Checklist");
    app.setOrganizationName("Pokédex Checklist");

    MainWindow window;
    window.show();

    return app.exec();
}
