#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setupMenu();

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

    this->setStyleSheet("QToolTip {max-width:320px;}");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete uitimer;
    delete updatetimer;
}

void MainWindow::loadList(){
    items.clear();
    cman.readJSON(DATAURI);
    unsigned int nsize = cman.getChannels()->size();
    for (unsigned int i=0; i < nsize; i++){
        Channel* channel = &cman.getChannels()->at(i);
        addItem(channel);
    }
    //sortItems();
}

void MainWindow::addItem(Channel* channel){
    QString logopath = "logos/";
    logopath.append(channel->getUriName().c_str());

    if (channel->isEmpty())
        cman.checkStream(channel,true);

    StreamItem item(channel);

    if (util::fileExists(logopath.toStdString().c_str()))
        item.setIcon(QIcon(logopath));

    items.push_back(item);
    /*for (unsigned int i=0; i < items.size(); i++){
        if (items.at(i).getChannel() == channel){
            ui->listWidget->addItem(&items.at(i));
            break;
        }
    }*/
}

void MainWindow::sortItems(){
    std::sort(items.begin(),items.end());
    //ui->listWidget->sortItems();
    /*qDebug() << "SORTED ORDER:";
    for (size_t i=0; i < items.size(); i++)
        qDebug() << items.at(i).text();
    qDebug() << "ORDER:";
    for (unsigned int i=0; i < items.size(); i++){
        if (ui->listWidget->item(i))
            qDebug() << items.at(i).getName() << ", " << ui->listWidget->item(i)->text();
    }
    qDebug() << "END:";*/
}

void MainWindow::updateList(){
    sortItems();

    for (unsigned int i=0; i < items.size(); i++){
        StreamItem &item = items.at(i);
        item.checkContent();

        if (item.online())
            item.setTextColor(QColor(0, 0, 0));
        else
            item.setTextColor(QColor(200, 200, 200));

        if (ui->listWidget->row(&item) == -1)
            ui->listWidget->addItem(&item);

    }
}

bool MainWindow::hasElement(Channel *channel){
    for (unsigned int i=0; i<items.size(); i++){
        if (items.at(i).text().toStdString() == channel->getName())
            return true;
    }
    return false;
}



void MainWindow::checkStreams(){
    cman.checkStreams(false);
}

void MainWindow::deleteItems(){

}


void MainWindow::on_addButton_clicked()
{
    QInputDialog input;
    input.resize(300,120);
    input.setWindowTitle("Add channel");
    input.setLabelText("Channel name");

    //qDebug() << input.textValue();
    if (input.exec()){
        std::string val = input.textValue().toStdString();
        if (!val.empty()){
            cman.add(val.c_str());
            cman.writeJSON(DATAURI);
            loadList();
        }
    }
    else qDebug() << "Canceled";
    //checkStreams();
}

void MainWindow::showContextMenu(const QPoint& pos){
    StreamItem *item = dynamic_cast<StreamItem*>(ui->listWidget->selectedItems().first());

    QMenu menu;
    menu.addAction("Watch");
    if (!item->online())
        menu.actions().first()->setEnabled(false);
    menu.addAction(QIcon::fromTheme("dialog-close"),"Remove");


    QAction* action = menu.exec(mapToGlobal(pos));
    if (action){
        QString act = action->text();


        if (act == "Remove"){
            if (QMessageBox::question(this,"Remove channel","Remove this channel?",QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
                remove(item);
        }
        else if (act == "Watch"){
            //QProcess process;
            std::string cmd = "./watch.sh "+dynamic_cast<StreamItem*>(item)->getUriName().toStdString();
            //system(cmd.c_str());
            //process.execute(cmd.c_str());
            std::thread t(system,cmd.c_str());
            t.detach();
        }
    }
}

void MainWindow::remove(StreamItem* item){
    cman.remove(item->getChannel());
    cman.writeJSON(DATAURI);

    for (unsigned int i=0; i < items.size(); i++){
        if (&items.at(i) == item){
            items.erase(items.begin() + i);
            break;
        }
    }
    updateList();

}
