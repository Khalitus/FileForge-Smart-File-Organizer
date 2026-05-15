#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include "fileorganizer.h"

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupUI();
    void setupStyles();
    void initializeSessionStorage();
    void selectFolder();
    void previewFiles();
    void organizeFiles();
    void showLogContents();
    void updateStatsPanel(const StatsReport &stats);
    void appendLogMessage(const QString &text);

    QLabel *lblPath = nullptr;
    QLabel *lblFileCount = nullptr;
    QLabel *lblStats = nullptr;
    QListWidget *listPreview = nullptr;
    QTextEdit *txtLog = nullptr;
    QPushButton *btnBrowse = nullptr;
    QPushButton *btnPreview = nullptr;
    QPushButton *btnOrganize = nullptr;
    QPushButton *btnShowLog = nullptr;

    QString folderPath;
    Logger logger;
    Organizer organizer;
};

#endif // MAINWINDOW_H
