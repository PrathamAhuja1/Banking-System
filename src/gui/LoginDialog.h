#pragma once
#include <QDialog>

class QLineEdit;
class QPushButton;

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString password() const;

private:
    QLineEdit *pwdEdit;
    QPushButton *okButton;
};
