// MainWindow.h
#pragma once
#include <QMainWindow>
#include <memory>

namespace Ui { class MainWindow; } // if using .ui; here we build UI in code.

class QTabWidget;
class QTableWidget;
class QPushButton;

#include "../core/Bank.h"
#include "../password/PasswordManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(const QString &masterPwd, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddAccount();
    void onDeposit();
    void onWithdraw();
    void onRefreshAccounts();

    void onAddVaultEntry();
    void onDeleteVaultEntry();
    void onRefreshVault();

    void onArchiveLogs();
    void onViewArchive();

private:
    std::unique_ptr<Bank> bank;
    std::unique_ptr<PasswordManager> pwdMgr;
    QString masterPassword;

    // UI elements
    QTabWidget *tabs;
    // Accounts tab
    QWidget *accountsTab;
    QTableWidget *accountsTable;
    QPushButton *addAccBtn;
    QPushButton *depositBtn;
    QPushButton *withdrawBtn;

    // Password vault tab
    QWidget *vaultTab;
    QTableWidget *vaultTable;
    QPushButton *addVaultBtn;
    QPushButton *delVaultBtn;

    // Logs tab
    QWidget *logsTab;
    QPushButton *archiveBtn;
    QPushButton *viewArchiveBtn;

    void setupUI();
    void loadData();
};
