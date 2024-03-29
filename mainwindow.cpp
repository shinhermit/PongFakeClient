#include "mainwindow.hpp"
#include "ui_mainwindow.h"

const qreal MainWindow::_timerInterval = 1000;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _streamer(&_socket),
    _dxRacket(0)
{
    ui->setupUi(this);

    connect( &_socket, SIGNAL(readyRead()), this, SLOT(_getDataSlot()) );
    connect( &_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(_socketError(QAbstractSocket::SocketError)) );

    ui->statusText->setPlainText(". Fake client starting\n");
}

MainWindow::~MainWindow()
{
    delete ui;

    if( _socket.isOpen() )
        _socket.close();
}

void MainWindow::connectToServer(const QString &hostname, const quint16 &port)
{
    _socket.connectToHost(hostname, port);
}

void MainWindow::connectToServer(const QHostAddress &hostAdress, const quint16 &port)
{
    _socket.connectToHost(hostAdress, port);
}

void MainWindow::connectToServer()
{
    ui->statusText->setPlainText(". Fake client trying to connect to host\n");
    _socket.connectToHost(QHostAddress::LocalHost, 6666);
}

void MainWindow::setdx()
{
    _dxRacket = (qreal)ui->dx->value();
}

void MainWindow::appendStatus(const QString &status)
{
    ui->statusText->setPlainText( ". " + status + "\n" + ui->statusText->toPlainText() );
}


void MainWindow::_socketError(QAbstractSocket::SocketError errorCode)
{
    appendStatus( "socket error, error code: " + QString::number(errorCode) );
}

void MainWindow::_getDataSlot()
{
    qint32 index, nbRackets, nbPlayers, loserIndex, gameState, downCounter;
    QVector<QLineF> racketsLines;
    qint32 id;
    QPointF p1_racket, p2_racket, ballPos;
    QString vectorString("\n   [");

    _streamer.resetStatus();

    _streamer >> ballPos >> index >> nbRackets >> nbPlayers >> loserIndex >> gameState >> downCounter;
    for(qint32 racketIndex=0; racketIndex < nbRackets; ++racketIndex)
    {
        _streamer >> id >> p1_racket >> p2_racket;
        vectorString += QString::number(id)+", ("+
                QString::number(p1_racket.x())+","+
                QString::number(p1_racket.y())+","+
                QString::number(p2_racket.x())+","+
                QString::number(p2_racket.y())+")";
        if(racketIndex != nbRackets-1)
            vectorString += "\n    ";
//        racketsLines.push_back(QLineF(p1_racket, p2_racket));
    }
    vectorString += "]";

    appendStatus("MainWindow::_getDataSlot(): data received");
    appendStatus("Received ballPos: ("+QString::number(ballPos.x())+","+QString::number(ballPos.y())+")"+
                 "index: "+QString::number(index)+
                 ",  nbRackets: "+QString::number(nbRackets)+", nbPlayers: "+QString::number(nbPlayers)+
                 ", loserIndex: "+QString::number(loserIndex)+", gameState: "+QString::number(gameState)+
                 ", downCounter: "+QString::number(downCounter)+vectorString );

    appendStatus("MainWindow::_getDataSlot(): next step = QTimer::singleShot");
    QTimer::singleShot( _timerInterval, this, SLOT(_sendDataSlot()) );
}

void MainWindow::_sendDataSlot()
{
    _streamer.resetStatus();
    _streamer << _dxRacket;

    _dxRacket = 0;

    if(_dxRacket != 0)
        appendStatus( "MainWindow::_sendDataSlot(): sent dx = "+QString::number(_dxRacket) );
}
