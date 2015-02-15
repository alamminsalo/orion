#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setupTray();

    cman = new ChannelManager();

    ui->listWidget->setSortingEnabled(true);
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));

    loadList();
    updateList();
    uitimer = new QTimer(this);
    connect(uitimer, SIGNAL(timeout()), this, SLOT(updateList()));
    uitimer->start(500);

    checkStreams();
    updatetimer = new QTimer(this);
    connect(updatetimer, SIGNAL(timeout()), this, SLOT(checkStreams()));
    updatetimer->start(20000);

    this->setStyleSheet("QToolTip {max-width:336px;}");
}

MainWindow::~MainWindow()
{
    qDebug() << "Destroyer: MainWindow";
    if (cman) delete cman;
    if (uitimer) delete uitimer;
    if (updatetimer) delete updatetimer;
    if (tray) delete tray;
    if (ui) delete ui;
    qDebug() << "MainWindow successfully destroyed!";
}

void MainWindow::loadList(){
    ui->listWidget->clear();
    cman->readJSON(DATAURI);
    unsigned int nsize = cman->getChannels()->size();
    for (unsigned int i=0; i < nsize; i++){
        Channel* channel = &cman->getChannels()->at(i);
        addItem(channel);
    }
}

void MainWindow::addItem(Channel* channel){
    QString logopath = "logos/";
    logopath.append(channel->getUriName().c_str());

    if (channel->isEmpty())
        cman->checkStream(channel,true);

    StreamItem* item = new StreamItem(channel);

    if (util::fileExists(logopath.toStdString().c_str()))
        item->setIcon(QIcon(logopath));

    ui->listWidget->addItem(item);
}

void MainWindow::updateList(){
    for (int i=0; i<ui->listWidget->count(); i++){
        dynamic_cast<StreamItem*>(ui->listWidget->item(i))->update();
    }
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
            loadList();
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
            std::string cmd = "./watch.sh "+item->getUriName().toStdString();
            std::thread t(system,cmd.c_str());
            t.detach();
        }
    }
}

void MainWindow::remove(StreamItem* item){
    cman->remove(item->getChannel());
    cman->writeJSON(DATAURI);
    ui->listWidget->removeItemWidget(item);

    loadList();
}

void MainWindow::setupTray(){
    tray = new QSystemTrayIcon();

    QMenu *traymenu = new QMenu();
    QAction *showAction = new QAction("Show list",tray);
    traymenu->addAction(showAction);
    connect(showAction,SIGNAL(triggered()),this,SLOT(toggleShow()));

    tray->setContextMenu(traymenu);
    tray->show();
}

void MainWindow::toggleShow(){
    if (!isVisible()){
        show();
        raise();
    }
    else hide();
}
