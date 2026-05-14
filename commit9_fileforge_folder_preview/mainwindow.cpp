#include "mainwindow.h"

#include <QDateTime>
#include <QFileDialog>
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
    btnOrganize->setEnabled(false);
    btnPreview->setEnabled(false);
    bar->addWidget(lblPath, 1);
    bar->addWidget(btnBrowse);
    bar->addWidget(btnPreview);
    bar->addWidget(btnOrganize);
    root->addLayout(bar);

    QHBoxLayout *body = new QHBoxLayout();
    QVBoxLayout *left = new QVBoxLayout();
    lblFileCount = new QLabel("0 files", this);
    listPreview = new QListWidget(this);
    left->addWidget(lblFileCount);
    left->addWidget(listPreview, 1);
    txtLog = new QTextEdit(this);
    txtLog->setReadOnly(true);
    body->addLayout(left, 2);
    body->addWidget(txtLog, 1);
    root->addLayout(body, 1);

    setCentralWidget(central);

    connect(btnBrowse, &QPushButton::clicked, this, [this]() { selectFolder(); });
    connect(btnPreview, &QPushButton::clicked, this, [this]() { previewFiles(); });
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

void MainWindow::selectFolder()
{
    const QString dir = QFileDialog::getExistingDirectory(this, "Select Folder to Organize", QDir::homePath());
    if (dir.isEmpty()) return;

    folderPath = dir;
    lblPath->setText(folderPath);
    btnPreview->setEnabled(true);
    appendLogMessage("Folder selected: " + folderPath);
    previewFiles();
}

void MainWindow::previewFiles()
{
    listPreview->clear();
    if (folderPath.isEmpty()) return;

    QString error;
    const QList<PreviewItem> items = organizer.preview(folderPath, &error);
    if (!error.isEmpty()) {
        appendLogMessage(error);
        return;
    }

    if (items.isEmpty()) {
        lblFileCount->setText("0 files");
        listPreview->addItem("No files found in this folder.");
        appendLogMessage("No files to preview.");
        return;
    }

    for (const PreviewItem &item : items) {
        QString line = item.fileName + "  ->  " + item.folder;
        if (item.renamed) line += "  (as " + item.finalName + ")";
        listPreview->addItem(line);
    }

    lblFileCount->setText(QString::number(items.size()) + " preview items");
    appendLogMessage(QString("Preview ready for %1 file(s).").arg(items.size()));
}

void MainWindow::appendLogMessage(const QString &text)
{
    txtLog->append(QDateTime::currentDateTime().toString("hh:mm:ss") + "  " + text);
}
