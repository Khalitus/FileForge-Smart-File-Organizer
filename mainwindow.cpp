#include "mainwindow.h"

#include <QDateTime>
#include <QDialog>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), organizer(&logger)
{
    setupUI();
    setupStyles();
    initializeSessionStorage();
}

void MainWindow::setupUI()
{
    setWindowTitle("FileForge");
    resize(1000, 640);
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
    btnShowLog = new QPushButton("Show Log", this);
    btnPreview->setEnabled(false);
    btnOrganize->setEnabled(false);
    btnShowLog->setEnabled(false);
    bar->addWidget(lblPath, 1);
    bar->addWidget(btnBrowse);
    bar->addWidget(btnPreview);
    bar->addWidget(btnOrganize);
    bar->addWidget(btnShowLog);
    root->addLayout(bar);

    QHBoxLayout *body = new QHBoxLayout();
    QVBoxLayout *left = new QVBoxLayout();
    lblFileCount = new QLabel("0 files", this);
    listPreview = new QListWidget(this);
    left->addWidget(lblFileCount);
    left->addWidget(listPreview, 1);

    QVBoxLayout *right = new QVBoxLayout();
    lblStats = new QLabel("Scanned: 0 | Moved: 0 | Failed: 0", this);
    lblStats->setObjectName("statsLabel");
    txtLog = new QTextEdit(this);
    txtLog->setReadOnly(true);
    right->addWidget(lblStats);
    right->addWidget(txtLog, 1);

    body->addLayout(left, 2);
    body->addLayout(right, 1);
    root->addLayout(body, 1);
    setCentralWidget(central);

    connect(btnBrowse, &QPushButton::clicked, this, [this]() { selectFolder(); });
    connect(btnPreview, &QPushButton::clicked, this, [this]() { previewFiles(); });
    connect(btnOrganize, &QPushButton::clicked, this, [this]() { organizeFiles(); });
    connect(btnShowLog, &QPushButton::clicked, this, [this]() { showLogContents(); });
}

void MainWindow::setupStyles()
{
    setStyleSheet(
        "QMainWindow{background:#f6f7fb;}"
        "QLabel#title{font-size:26px; font-weight:800; color:#111827;}"
        "QLabel#pathLabel,QLabel#statsLabel{background:white; border:1px solid #dfe3ea; border-radius:10px; padding:10px; color:#374151;}"
        "QPushButton{background:#111827; color:white; border:none; border-radius:10px; padding:10px 14px; font-weight:700;}"
        "QPushButton:disabled{background:#cbd5e1; color:#64748b;}"
        "QListWidget,QTextEdit{background:white; border:1px solid #dfe3ea; border-radius:12px; padding:8px;}"
    );
}

void MainWindow::initializeSessionStorage()
{
    QString error;
    if (logger.initializeSessionStorage(&error)) {
        btnShowLog->setEnabled(true);
        appendLogMessage("Session log created on Desktop/logs.");
    } else {
        appendLogMessage(error);
    }
}

void MainWindow::selectFolder()
{
    const QString dir = QFileDialog::getExistingDirectory(this, "Select Folder to Organize", QDir::homePath());
    if (dir.isEmpty()) return;
    folderPath = dir;
    lblPath->setText(folderPath);
    btnPreview->setEnabled(true);
    btnOrganize->setEnabled(logger.isReady());
    appendLogMessage("Folder selected: " + folderPath);
    previewFiles();
}

void MainWindow::previewFiles()
{
    listPreview->clear();
    QString error;
    const QList<PreviewItem> items = organizer.preview(folderPath, &error);
    if (!error.isEmpty()) { appendLogMessage(error); return; }
    if (items.isEmpty()) { lblFileCount->setText("0 files"); listPreview->addItem("No files found."); return; }
    for (const PreviewItem &item : items) {
        QString line = item.fileName + "  ->  " + item.folder;
        if (item.renamed) line += "  (as " + item.finalName + ")";
        listPreview->addItem(line);
    }
    lblFileCount->setText(QString::number(items.size()) + " preview items");
}

void MainWindow::organizeFiles()
{
    if (folderPath.isEmpty()) return;
    const QList<PreviewItem> items = organizer.preview(folderPath);
    if (items.isEmpty()) { appendLogMessage("No files to organize."); return; }
    const auto reply = QMessageBox::question(this, "Confirm Organization", QString("Organize %1 file(s)?").arg(items.size()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply != QMessageBox::Yes) { appendLogMessage("Organization cancelled."); return; }

    appendLogMessage("Organization started...");
    const QStringList messages = organizer.organize(folderPath);
    for (const QString &msg : messages) appendLogMessage(msg);
    updateStatsPanel(organizer.getStats());
    previewFiles();
}

void MainWindow::showLogContents()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Session Log");
    dialog.resize(720, 460);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QTextEdit *viewer = new QTextEdit(&dialog);
    viewer->setReadOnly(true);
    viewer->setPlainText(logger.readLogText());
    layout->addWidget(viewer);
    dialog.exec();
}

void MainWindow::updateStatsPanel(const StatsReport &s)
{
    lblStats->setText(QString("Scanned: %1 | Moved: %2 | Failed: %3 | Images: %4 | Docs: %5 | Code: %6 | Other: %7")
        .arg(s.totalScanned).arg(s.totalMoved).arg(s.totalFailed).arg(s.imageCount).arg(s.documentCount).arg(s.codeCount).arg(s.otherCount));
}

void MainWindow::appendLogMessage(const QString &text)
{
    txtLog->append(QDateTime::currentDateTime().toString("hh:mm:ss") + "  " + text);
}
