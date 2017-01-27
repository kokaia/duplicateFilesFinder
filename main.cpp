#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>
#include "common.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QCoreApplication::setApplicationName("Video Widget Example");
  QCoreApplication::setOrganizationName("QtProject");
  QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());
  QCoreApplication::setApplicationVersion(QT_VERSION_STR);

  if (!QSqlDatabase::drivers().contains("QSQLITE")) {
    QMessageBox::critical(0, "Unable to load database",
                          "This demo needs the SQLITE driver");
    return 0;
  }

  Common::init();

  MainWindow w;

  w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(),
                                    qApp->desktop()->availableGeometry()));

  w.show();

  return a.exec();
}
