#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    cman = new ChannelManager();
    cman->checkResources();

    this->setWindowTitle(APP_NAME);
    this->setWindowIcon(QIcon("resources/icon.svg"));

    this->setStyleSheet("QToolTip {max-width:336px;}");

    this->setupTray();

    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidget->setSortingEnabled(true);
    connect(ui->listWidget,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));

    connect(cman,SIGNAL(channelExists(Channel*)),this,SLOT(showAlreadyAdded(Channel*)));
    connect(cman,SIGNAL(channelNotFound(Channel*)),this,SLOT(showNotFound(Channel*)));
    connect(cman,SIGNAL(channelStateChanged(Channel*)),this,SLOT(notify(Channel*)));
    connect(cman,SIGNAL(newChannel(Channel*)),this,SLOT(addItem(Channel*)));

    loadList();

    updatetimer = new QTimer(this);
    connect(updatetimer, SIGNAL(timeout()), this, SLOT(checkStreams()));
    updatetimer->start(60000);
    checkStreams();
}

MainWindow::~MainWindow()
{
    qDebug() << "Destroyer: MainWindow";
    saveSettings();

    qDebug() << "Deleting ui..";
    delete ui;

    qDebug() << "Clearing channel list..";
    ui->listWidget->clear();

    qDebug() << "Stopping timers..";
    updatetimer->stop();

    qDebug() << "Deleting timers..";
    if (updatetimer)
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

void MainWindow::showNotFound(Channel *channel)
{
    QMessageBox::information(this,"404", "Channel not found",QMessageBox::Ok);
    StreamItem *item = find(channel);
    if (item)
        remove(item);
}

StreamItem* MainWindow::find(Channel *channel){
    for (int i=0; i<ui->listWidget->count(); i++){
        StreamItem *item = dynamic_cast<StreamItem*>(ui->listWidget->item(i));
        if (item->getChannel() == channel){
            return item;
        }
    }
    return NULL;
}

int MainWindow::findAt(Channel *channel){
    return ui->listWidget->row(find(channel));
}

void MainWindow::showAlreadyAdded(Channel *channel)
{
    ui->listWidget->setCurrentItem(find(channel));
    QMessageBox::information(this,"Error", "Channel already on list",QMessageBox::Ok);
}

void MainWindow::addItem(Channel* channel){
    QString logopath = "logos/";
    logopath.append(channel->getServiceName());

    StreamItem* item = new StreamItem(channel);

    connect(channel,SIGNAL(updated()),item,SLOT(update()));
    connect(channel,SIGNAL(iconUpdated()),item,SLOT(updateIcon()));
    ui->listWidget->addItem(item);
}

void MainWindow::updateList(){
    for (int i=0; i<ui->listWidget->count(); i++){
        dynamic_cast<StreamItem*>(ui->listWidget->item(i))->update();
    }
    ui->listWidget->sortItems();
}

void MainWindow::checkStreams(){
    cman->checkStreams();
}


void MainWindow::on_addButton_clicked()
{
    if (cman->getChannels().size() >= 99){ //Sorry my laziness, twitch api supports maximum limit of 100..
        QMessageBox::information(this,"Error", "Maximum number of channels reached",QMessageBox::Ok);
        return;
    }
    QInputDialog input(this);
    input.resize(300,120);
    input.setWindowTitle("Add channel");
    input.setLabelText("Channel name");

    if (input.exec()){
        QString val = input.textValue().trimmed();
        if (!val.isEmpty()){
            cman->addToFavourites(val);
        }
        else QMessageBox::information(this,"Error", "Bad input",QMessageBox::Ok);
    }
}

void MainWindow::showContextMenu(const QPoint& pos){
    if (ui->listWidget->currentItem()){
        StreamItem *item = dynamic_cast<StreamItem*>(ui->listWidget->currentItem());

        QMenu menu;
        menu.addAction(QIcon::fromTheme("mpv"),"Watch");
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
}

void MainWindow::remove(StreamItem* item){
    cman->removeFromFavourites(item->getChannel());
    qDebug() << "Removed from channelmanager";
    delete item;
    qDebug() << "Removed from list";
}

void MainWindow::setupTray(){
    tray = new QSystemTrayIcon();
    traymenu = new QMenu();

    traymenuaction = new QAction("Show list",tray);
    traymenu->addAction(traymenuaction);
    connect(traymenuaction,SIGNAL(triggered()),this,SLOT(toggleShow()));

    traymenuquit = new QAction("Exit",tray);
    traymenu->addAction(traymenuquit);
    connect(traymenuquit,SIGNAL(triggered()),QApplication::instance(),SLOT(quit()));

    tray->setContextMenu(traymenu);
    tray->setIcon(QIcon("resources/icon.svg"));
    tray->show();
}

void MainWindow::loadList()
{
    cman->load();
    for (unsigned int i=0; i < cman->getChannels().size(); i++){
        addItem(cman->getChannels().at(i));
    }
}

void MainWindow::trayGone(){
    qDebug() << "Tray destroyed";
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    saveSettings();
}

void MainWindow::toggleShow(){
    if (!isVisible()){
        this->show();
        raise();
    }
    else hide();
}

void MainWindow::saveSettings(){
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
}

void MainWindow::loadSettings(){
    QSettings settings;
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    restoreState(settings.value("mainWindowState").toByteArray());
}

void MainWindow::show(){
    loadSettings();
    QMainWindow::show();
}

void MainWindow::hide(){
    saveSettings();
    QMainWindow::hide();
}


void MainWindow::notify(Channel *channel){
#ifdef Q_OS_LINUX
    QString title = channel->getName() + (channel->isOnline() ? " is streaming" : " has gone offline");
    QString cmd = "resources/scripts/dialog.sh \"" + title + "\" \"" + channel->getInfo() + "\" \"/" + channel->getLogoPath() + "\"";
    //QProcess process;
    QProcess::startDetached(cmd);
#endif
}


