#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <QSystemTrayIcon>
#include <QMenu>


class SysTray: public QSystemTrayIcon
{
    Q_OBJECT

public:
    SysTray();
    ~SysTray();

signals:
    void showTriggered();
    void closeEventTriggered();

public slots:
    void showSlot();
    void quitSlot();
    void clickSlot(QSystemTrayIcon::ActivationReason);

private:
    QMenu *menu;
};

#endif // SYSTRAY_H
