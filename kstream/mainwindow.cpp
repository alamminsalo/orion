#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("icon"));

    this->setStyleSheet("QToolTip {max-width:336px;}");

    this->setupTray();

    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));

    cman = new ChannelManager(this);
    cman->load();

    uitimer = new QTimer(this);
    connect(uitimer, SIGNAL(timeout()), this, SLOT(updateList()));
    uitimer->start(1000);
    updateList();

    updatetimer = new QTimer(this);
    connect(updatetimer, SIGNAL(timeout()), this, SLOT(checkStreams()));
    updatetimer->start(10000);
    checkStreams();
}

MainWindow::~MainWindow()
{
    qDebug() << "Destroyer: MainWindow";

    qDebug() << "Deleting ui..";
    delete ui;

    qDebug() << "Clearing channel list..";
    ui->listWidget->clear();

    qDebug() << "Stopping timers..";
    uitimer->stop();
    updatetimer->stop();

    disconnect(uitimer, SIGNAL(timeout()), this, SLOT(updateList()));
    disconnect(updatetimer, SIGNAL(timeout()), this, SLOT(checkStreams()));

    qDebug() << "Deleting timers..";
    delete uitimer;
    delete updatetimer;

    qDebug() << "Deleting channelmanager..";
    delete cman;

    qDebug() << "Deleting tray..";
    delete traymenuaction;
    qDebug() << "..";
    delete traymenu;
    qDebug() << "..";
    tray->deleteLater();

    qDebug() << "All done!";
}

void MainWindow::loadList(){
    ui->listWidget->clear();
    cman->readJSON(DATAURI);
    unsigned int nsize = cman->getChannels()->size();
    for (unsigned int i=0; i < nsize; i++){
        //Channel* channel = &cman->getChannels()
        //addItem(channel);
    }
}

void MainWindow::update(Channel *channel){
    qDebug() << channel;
    for (unsigned int i=0; i < cman->getChannels()->size(); i++){
        StreamItem *item = dynamic_cast<StreamItem*>(ui->listWidget->item(i));
        if (item->getChannel() == channel){
            item->update();
            break;
        }
    }
    ui->listWidget->sortItems();
}

void MainWindow::addItem(Channel* channel){
    QString logopath = "logos/";
    logopath.append(channel->getUriName().c_str());

    if (channel->isEmpty())
        cman->checkStream(channel,true);

    StreamItem* item = new StreamItem(channel);

    item->update();

    if (util::fileExists(logopath.toStdString().c_str()))
        item->setIcon(QIcon(logopath));

    ui->listWidget->addItem(item);
}

void MainWindow::updateList(){
    for (int i=0; i<ui->listWidget->count(); i++){
        dynamic_cast<StreamItem*>(ui->listWidget->item(i))->update();
    }
    ui->listWidget->sortItems();
}

void MainWindow::checkStreams(){
    cman->checkStreams(false);
}

void MainWindow::deleteItems(){

}


void MainWindow::on_addButton_clicked()
{
    QInputDialog input;
    input.resize(300,120);
    input.setWindowTitle("Add channel");
    input.setLabelText("Channel name");

    if (input.exec()){
        std::string val = input.textValue().toStdString();
        if (!val.empty()){
            cman->add(val.c_str());
            cman->writeJSON(DATAURI);
            //loadList();
        }
    }
}

void MainWindow::showContextMenu(const QPoint& pos){
    StreamItem *item = dynamic_cast<StreamItem*>(ui->listWidget->currentItem());

    QMenu menu;
    menu.addAction(QIcon::fromTheme("vlc"),"Watch");
    if (!item->online())
        menu.actions().first()->setEnabled(false);
    menu.addAction(QIcon::fromTheme("list-remove"),"Remove");

    QAction* action = menu.exec(mapToGlobal(pos));
    if (action){
        QString act = action->text();

        if (act == "Remove"){
            if (QMessageBox::question(this,"Remove channel","Remove this channel?",QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
                remove(item);
        }
        else if (act == "Watch"){
            cman->play(item->getChannel());
        }
    }
}

void MainWindow::remove(StreamItem* item){
    cman->remove(item->getChannel());
    delete item;
}

void MainWindow::setupTray(){
    tray = new QSystemTrayIcon();
    traymenu = new QMenu();
    traymenuaction = new QAction("Show list",tray);
    traymenu->addAction(traymenuaction);
    connect(traymenuaction,SIGNAL(triggered()),this,SLOT(toggleShow()));

    tray->setContextMenu(traymenu);
    tray->setIcon(QIcon("icon"));
    tray->show();
}

void MainWindow::trayGone(){
    qDebug() << "Tray destroyed";
}

void MainWindow::toggleShow(){
    if (!isVisible()){
        show();
        raise();
    }
    else hide();
}

