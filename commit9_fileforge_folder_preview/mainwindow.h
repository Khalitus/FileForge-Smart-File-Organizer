#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTextEdit>
#include "fileorganizer.h"

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupUI();
    void setupStyles();
    void selectFolder();
    void previewFiles();
    void appendLogMessage(const QString &text);

    QLabel *lblPath = nullptr;
    QLabel *lblFileCount = nullptr;
    QListWidget *listPreview = nullptr;
    QTextEdit *txtLog = nullptr;
    QPushButton *btnBrowse = nullptr;
    QPushButton *btnPreview = nullptr;
    QPushButton *btnOrganize = nullptr;

    QString folderPath;
    Organizer organizer;
};

#endif // MAINWINDOW_H
