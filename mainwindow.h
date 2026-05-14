#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTextEdit>

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupUI();
    void setupStyles();

    QLabel *lblPath = nullptr;
    QListWidget *listPreview = nullptr;
    QTextEdit *txtLog = nullptr;
    QPushButton *btnBrowse = nullptr;
    QPushButton *btnPreview = nullptr;
    QPushButton *btnOrganize = nullptr;
};

#endif // MAINWINDOW_H
