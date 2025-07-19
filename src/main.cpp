#include <QApplication>
#include "mainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("lachesis17's Pokedex Checklist");
    app.setOrganizationName("Pokedex Checklist");

    MainWindow window;
    window.show();

    return app.exec();
}
