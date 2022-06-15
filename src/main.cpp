#include "common.h"
#include "mainwindow.h"

#include <QStyle>
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("Duplicate file finder");
    QCoreApplication::setOrganizationName("QtProject");
    QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        QMessageBox::critical(nullptr, "Unable to load database", "This app needs the SQLITE driver");
        return 0;
    }

    Common::init();
    MainWindow w;

    QList<QScreen *> screens = QGuiApplication::screens();
    QScreen *screen = screens.at(0);
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(), screen->availableGeometry()));

    w.show();

    return QApplication::exec();
}
