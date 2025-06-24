#include <QApplication>
#include "LoginDialog.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    LoginDialog dlg;
    if (dlg.exec() == QDialog::Accepted) {
        QString pwd = dlg.password();
        MainWindow w(pwd);
        w.show();
        return a.exec();
    }
    return 0;
}
