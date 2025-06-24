// MainWindow.cpp
#include "MainWindow.h"
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <filesystem>
#include <chrono>
#include <thread>

#include "../compression/Huffman.h"

MainWindow::MainWindow(const QString &masterPwd, QWidget *parent)
    : QMainWindow(parent), masterPassword(masterPwd) {
    // Initialize backend: data files in working directory
    QString dataFile = "accounts.dat";
    QString logFile = "transactions.dat";
    QString vaultFile = "vault.dat";
    bank = std::make_unique<Bank>(dataFile.toStdString(), logFile.toStdString(), masterPwd.toStdString());
    pwdMgr = std::make_unique<PasswordManager>(vaultFile.toStdString(), masterPwd.toStdString());
    bool ok1 = bank->load();
    bool ok2 = pwdMgr->load();
    if (!ok1) {
        QMessageBox::warning(this, "Error", "Failed to load bank data. Starting fresh.");
    }
    if (!ok2) {
        QMessageBox::warning(this, "Error", "Failed to load vault. Starting fresh.");
    }
    setupUI();
    loadData();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    // Accounts Tab
    accountsTab = new QWidget(this);
    QVBoxLayout *accLayout = new QVBoxLayout(accountsTab);
    accountsTable = new QTableWidget(this);
    accountsTable->setColumnCount(3);
    accountsTable->setHorizontalHeaderLabels({"Account No", "Holder", "Balance"});
    accLayout->addWidget(accountsTable);
    QHBoxLayout *accBtnLayout = new QHBoxLayout();
    addAccBtn = new QPushButton("Add Account", this);
    depositBtn = new QPushButton("Deposit", this);
    withdrawBtn = new QPushButton("Withdraw", this);
    accBtnLayout->addWidget(addAccBtn);
    accBtnLayout->addWidget(depositBtn);
    accBtnLayout->addWidget(withdrawBtn);
    accLayout->addLayout(accBtnLayout);
    tabs->addTab(accountsTab, "Accounts");
    connect(addAccBtn, &QPushButton::clicked, this, &MainWindow::onAddAccount);
    connect(depositBtn, &QPushButton::clicked, this, &MainWindow::onDeposit);
    connect(withdrawBtn, &QPushButton::clicked, this, &MainWindow::onWithdraw);

    // Vault Tab
    vaultTab = new QWidget(this);
    QVBoxLayout *vaultLayout = new QVBoxLayout(vaultTab);
    vaultTable = new QTableWidget(this);
    vaultTable->setColumnCount(3);
    vaultTable->setHorizontalHeaderLabels({"Service", "Username", "Password"});
    vaultLayout->addWidget(vaultTable);
    QHBoxLayout *vaultBtnLayout = new QHBoxLayout();
    addVaultBtn = new QPushButton("Add Entry", this);
    delVaultBtn = new QPushButton("Delete Entry", this);
    vaultBtnLayout->addWidget(addVaultBtn);
    vaultBtnLayout->addWidget(delVaultBtn);
    vaultLayout->addLayout(vaultBtnLayout);
    tabs->addTab(vaultTab, "Password Vault");
    connect(addVaultBtn, &QPushButton::clicked, this, &MainWindow::onAddVaultEntry);
    connect(delVaultBtn, &QPushButton::clicked, this, &MainWindow::onDeleteVaultEntry);

    // Logs Tab
    logsTab = new QWidget(this);
    QVBoxLayout *logLayout = new QVBoxLayout(logsTab);
    archiveBtn = new QPushButton("Archive Transaction Logs", this);
    viewArchiveBtn = new QPushButton("View Archived Log", this);
    logLayout->addWidget(archiveBtn);
    logLayout->addWidget(viewArchiveBtn);
    tabs->addTab(logsTab, "Logs");
    connect(archiveBtn, &QPushButton::clicked, this, &MainWindow::onArchiveLogs);
    connect(viewArchiveBtn, &QPushButton::clicked, this, &MainWindow::onViewArchive);
}

void MainWindow::loadData() {
    // Populate accounts table
    onRefreshAccounts();
    // Populate vault
    onRefreshVault();
}

void MainWindow::onRefreshAccounts() {
    const auto &accounts = bank->getAllAccounts();
    accountsTable->setRowCount(int(accounts.size()));
    for (int i = 0; i < int(accounts.size()); ++i) {
        auto &acc = accounts[i];
        accountsTable->setItem(i, 0, new QTableWidgetItem(QString::number(acc.getAccountNumber())));
        accountsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(acc.getHolderName())));
        accountsTable->setItem(i, 2, new QTableWidgetItem(QString::number(acc.getBalance())));
    }
}

void MainWindow::onAddAccount() {
    bool ok;
    QString holder = QInputDialog::getText(this, "Add Account", "Holder Name:", QLineEdit::Normal, "", &ok);
    if (!ok || holder.isEmpty()) return;
    double initDep = QInputDialog::getDouble(this, "Initial Deposit", "Amount:", 0.0, 0.0, 1e12, 2, &ok);
    if (!ok) return;
    BankAccount* acc = bank->createAccount(holder.toStdString(), initDep);
    if (acc) {
        bank->save();
        onRefreshAccounts();
    } else {
        QMessageBox::warning(this, "Error", "Failed to create account.");
    }
}

void MainWindow::onDeposit() {
    QModelIndexList sel = accountsTable->selectionModel()->selectedRows();
    if (sel.empty()) { QMessageBox::information(this, "Info", "Select an account row."); return; }
    int row = sel.first().row();
    int accNo = accountsTable->item(row, 0)->text().toInt();
    bool ok;
    double amt = QInputDialog::getDouble(this, "Deposit", "Amount:", 0.0, 0.01, 1e12, 2, &ok);
    if (!ok) return;
    if (bank->deposit(accNo, amt)) {
        bank->save();
        onRefreshAccounts();
    } else {
        QMessageBox::warning(this, "Error", "Deposit failed.");
    }
}

void MainWindow::onWithdraw() {
    QModelIndexList sel = accountsTable->selectionModel()->selectedRows();
    if (sel.empty()) { QMessageBox::information(this, "Info", "Select an account row."); return; }
    int row = sel.first().row();
    int accNo = accountsTable->item(row, 0)->text().toInt();
    bool ok;
    double amt = QInputDialog::getDouble(this, "Withdraw", "Amount:", 0.0, 0.01, 1e12, 2, &ok);
    if (!ok) return;
    if (bank->withdraw(accNo, amt)) {
        bank->save();
        onRefreshAccounts();
    } else {
        QMessageBox::warning(this, "Error", "Withdraw failed (insufficient funds?).");
    }
}

void MainWindow::onRefreshVault() {
    auto entries = pwdMgr->listEntries();
    vaultTable->setRowCount(int(entries.size()));
    for (int i = 0; i < int(entries.size()); ++i) {
        const auto &e = entries[i];
        vaultTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(e.service)));
        vaultTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(e.username)));
        vaultTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(e.password)));
    }
}

void MainWindow::onAddVaultEntry() {
    bool ok;
    QString service = QInputDialog::getText(this, "Add Vault Entry", "Service:", QLineEdit::Normal, "", &ok);
    if (!ok || service.isEmpty()) return;
    QString username = QInputDialog::getText(this, "Username", "Username:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    QString password = QInputDialog::getText(this, "Password", "Password:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    if (pwdMgr->addEntry(service.toStdString(), username.toStdString(), password.toStdString())) {
        onRefreshVault();
    } else {
        QMessageBox::warning(this, "Error", "Failed to add (maybe duplicate service).");
    }
}

void MainWindow::onDeleteVaultEntry() {
    QModelIndexList sel = vaultTable->selectionModel()->selectedRows();
    if (sel.empty()) { QMessageBox::information(this, "Info", "Select an entry row."); return; }
    int row = sel.first().row();
    QString service = vaultTable->item(row, 0)->text();
    if (pwdMgr->deleteEntry(service.toStdString())) {
        onRefreshVault();
    } else {
        QMessageBox::warning(this, "Error", "Delete failed.");
    }
}

void MainWindow::onArchiveLogs() {
    QString inPath = "transactions.dat";
    if (!std::filesystem::exists(inPath.toStdString())) {
        QMessageBox::information(this, "Info", "No transaction log to archive.");
        return;
    }
    // Decrypt log to plaintext
    QString tempPlain = "temp_decrypted_log.txt";
    if (!CryptoUtils::decryptFile(inPath.toStdString(), tempPlain.toStdString(), masterPassword.toStdString())) {
        QMessageBox::warning(this, "Error", "Failed to decrypt log.");
        return;
    }
    // Choose output archive path
    auto now = std::chrono::system_clock::now();
    auto t_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#if defined(_WIN32)
    localtime_s(&tm, &t_c);
#else
    localtime_r(&t_c, &tm);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "archive_%Y%m%d_%H%M%S.huff", &tm);
    QString outPath = QFileDialog::getSaveFileName(this, "Save Archive As", buf, "Huffman Archive (*.huff)");
    if (outPath.isEmpty()) {
        std::filesystem::remove(tempPlain.toStdString());
        return;
    }
    // Compress
    bool ok = Huffman::compressFile(tempPlain.toStdString(), outPath.toStdString());
    std::filesystem::remove(tempPlain.toStdString());
    if (!ok) {
        QMessageBox::warning(this, "Error", "Compression failed.");
        return;
    }
    QMessageBox::information(this, "Success", "Archive created: " + outPath);
    // Optionally clear the log: here we truncate encrypted file
    std::ofstream empty("temp_empty.txt", std::ios::trunc);
    empty.close();
    CryptoUtils::encryptFile("temp_empty.txt", inPath.toStdString(), masterPassword.toStdString());
    std::filesystem::remove("temp_empty.txt");
}

void MainWindow::onViewArchive() {
    QString inPath = QFileDialog::getOpenFileName(this, "Select Archive", "", "Huffman Archive (*.huff)");
    if (inPath.isEmpty()) return;
    QString tempOut = "temp_decompressed_log.txt";
    if (!Huffman::decompressFile(inPath.toStdString(), tempOut.toStdString())) {
        QMessageBox::warning(this, "Error", "Decompression failed.");
        return;
    }
    // Display in a dialog
    std::ifstream in(tempOut.toStdString());
    std::string content((std::istreambuf_iterator<char>(in)), {});
    in.close();
    std::filesystem::remove(tempOut.toStdString());
    QDialog dlg(this);
    dlg.setWindowTitle("Archived Log Contents");
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    QTextEdit *textEdit = new QTextEdit(&dlg);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(QString::fromStdString(content));
    layout->addWidget(textEdit);
    QPushButton *closeBtn = new QPushButton("Close", &dlg);
    layout->addWidget(closeBtn);
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    dlg.exec();
}
