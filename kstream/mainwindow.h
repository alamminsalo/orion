#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../channelmanager.h"
#include "streamitem.h"
#include <QTimer>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QList>
#include <QSystemTrayIcon>

namespace Ui {
class MainWindow;
}

class ChannelManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    ChannelManager *cman;
    QTimer *uitimer;
    QTimer *updatetimer;
    QSystemTrayIcon *tray;
    QMenu *traymenu;
    QAction *traymenuaction;

    void remove(StreamItem*);
    void setupTray();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadList();
    void addItem(Channel*);
    void update(Channel*);

public slots:
    void updateList();
    void checkStreams();
    void showContextMenu(const QPoint&);
    void toggleShow();
    void trayGone();

private slots:
    void on_addButton_clicked();

private:
    Ui::MainWindow *ui;
    void deleteItems();
};
#endif // MAINWINDOW_H
