#include <QApplication>
#include <QFile>
#include <QDir>
#include "src/gui/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Clínica Management System");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Universidad");

    // Load stylesheet
    QStringList stylePaths = {
        "resources/styles.qss",
        QDir::currentPath() + "/resources/styles.qss",
        QCoreApplication::applicationDirPath() + "/resources/styles.qss"
    };
    for (const QString& path : stylePaths) {
        QFile f(path);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            app.setStyleSheet(QString::fromUtf8(f.readAll()));
            f.close();
            break;
        }
    }

    MainWindow w;
    w.show();
    return app.exec();
}
