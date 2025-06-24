// LoginDialog.cpp
#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Enter Master Password");
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Master Password:", this);
    layout->addWidget(label);
    pwdEdit = new QLineEdit(this);
    pwdEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(pwdEdit);
    okButton = new QPushButton("OK", this);
    layout->addWidget(okButton);
    connect(okButton, &QPushButton::clicked, this, &LoginDialog::accept);
}

QString LoginDialog::password() const {
    return pwdEdit->text();
}
