#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    auto icon = QIcon(":/icon/resources/4oddnig.png");
    app.setWindowIcon(icon);
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}

