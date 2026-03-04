#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("ClinicManager");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("UTN Costa Rica");

    // Set locale for Spanish date/number formatting
    QLocale::setDefault(QLocale(QLocale::Spanish, QLocale::CostaRica));

    MainWindow window;
    window.show();

    return app.exec();
}
