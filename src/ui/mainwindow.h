#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../model/channelmanager.h"
#include "streamitem.h"
#include <QTimer>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QList>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QProcess>
#include <QDebug>

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
    void loadList();
    StreamItem* find(Channel*);
    int findAt(Channel*);
    void loadSettings();
    void saveSettings();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void show();
    virtual void hide();

    void update(Channel*);

public slots:
    void updateList();
    void checkStreams();
    void showContextMenu(const QPoint&);
    void toggleShow();
    void trayGone();
    void closeEvent(QCloseEvent*);


private slots:
    void on_addButton_clicked();
    void showNotFound(Channel*);
    void showAlreadyAdded(Channel*);
    void notify(Channel*);
    void addItem(Channel*);

private:
    Ui::MainWindow *ui;
    void deleteItems();
};
#endif // MAINWINDOW_H
