#include "mainwindow.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUI();
    setupStyles();
}

void MainWindow::setupUI()
{
    setWindowTitle("FileForge");
    resize(980, 620);

    QWidget *central = new QWidget(this);
    QVBoxLayout *root = new QVBoxLayout(central);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(14);

    QLabel *title = new QLabel("FileForge", this);
    title->setObjectName("title");
    root->addWidget(title);

    QHBoxLayout *bar = new QHBoxLayout();
    lblPath = new QLabel("No folder selected", this);
    lblPath->setObjectName("pathLabel");
    btnBrowse = new QPushButton("Browse", this);
    btnPreview = new QPushButton("Preview", this);
    btnOrganize = new QPushButton("Organize", this);
    btnPreview->setEnabled(false);
    btnOrganize->setEnabled(false);
    bar->addWidget(lblPath, 1);
    bar->addWidget(btnBrowse);
    bar->addWidget(btnPreview);
    bar->addWidget(btnOrganize);
    root->addLayout(bar);

    QHBoxLayout *body = new QHBoxLayout();
    listPreview = new QListWidget(this);
    txtLog = new QTextEdit(this);
    txtLog->setReadOnly(true);
    txtLog->setPlainText("Qt shell ready. Organizer logic will be added in the next commits.");
    body->addWidget(listPreview, 2);
    body->addWidget(txtLog, 1);
    root->addLayout(body, 1);

    setCentralWidget(central);
}

void MainWindow::setupStyles()
{
    setStyleSheet(
        "QMainWindow{background:#f6f7fb;}"
        "QLabel#title{font-size:26px; font-weight:800; color:#111827;}"
        "QLabel#pathLabel{background:white; border:1px solid #dfe3ea; border-radius:10px; padding:10px; color:#374151;}"
        "QPushButton{background:#111827; color:white; border:none; border-radius:10px; padding:10px 14px; font-weight:700;}"
        "QPushButton:disabled{background:#cbd5e1; color:#64748b;}"
        "QListWidget,QTextEdit{background:white; border:1px solid #dfe3ea; border-radius:12px; padding:8px;}"
    );
}
