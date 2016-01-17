#include "game.h"


QString Game::getName() const
{
    return name;
}

void Game::setName(const QString &value)
{
    name = value;
}

QString Game::getLogo() const
{
    return logo;
}

void Game::setLogo(const QString &value)
{
    logo = value;
}

quint32 Game::getViewers() const
{
    return viewers;
}

void Game::setViewers(const quint32 &value)
{
    viewers = value;
}

QString Game::getPreview() const
{
    return preview;
}

void Game::setPreview(const QString &value)
{
    preview = value;
}
Game::Game(){
    viewers = 0;
}

//Copy constructor
Game::Game(const Game &other)
{
    setViewers(other.getViewers());
    setName(other.getName());
    setLogo(other.getLogo());
    setPreview(other.getPreview());
}
