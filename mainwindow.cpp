#include "mainwindow.hpp"
#include "ui_mainwindow.h"

const int MainWindow::_dxRacket = 20;
const int MainWindow::_timerInterval = 5000;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _socket_stream(&_socket)
{
    ui->setupUi(this);
    //_socket_stream.setDevice(&_socket);

    connect( &_socket, SIGNAL(readyRead()), this, SLOT(_getDataSlot()) );
    connect( this, SIGNAL(sendDataSignal()), this, SLOT(_sendDataSlot()) );
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

void MainWindow::show()
{
    connectToServer();

    QMainWindow::show();
}

void MainWindow::appendStatus(const QString &status)
{
    ui->statusText->setPlainText(ui->statusText->toPlainText()+". "+status+"\n");
}


void MainWindow::_socketError(QAbstractSocket::SocketError socketError)
{
    appendStatus("socket error" + socketError);
}

void MainWindow::_getDataSlot()
{
    qint32 index, nbRackets, nbPlayers, loserIndex, gameState, downCounter=-1;
    QVector<QLineF> racketsLines;
    int dummy;
    QPointF p1_racket, p2_racket;

    _socket_stream >> index >> nbRackets >> nbPlayers >> loserIndex >> gameState >> downCounter;
    for(int playerIndex=0; playerIndex < nbPlayers; ++playerIndex)
    {
        _socket_stream >> dummy >> p1_racket >> p2_racket;
        racketsLines.push_back(QLineF(p1_racket, p2_racket));
    }

    appendStatus("Received index: "+QString::number(index)+",  nbRackets: "+QString::number(nbRackets)+" ...");

    QTimer::singleShot( _timerInterval, this, SLOT(_sendDataSlot()) );
}

void MainWindow::_sendDataSlot()
{
    _socket_stream << _dxRacket % (_dxRacket+1);
}