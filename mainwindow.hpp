#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QTimer>
#include <QLineF>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void connectToServer(const QString & hostname,
                         const quint16 & port=6666);

    void connectToServer(const QHostAddress & hostAdress=QHostAddress::LocalHost,
                         const quint16 & port=6666);

    void show();

    void appendStatus(const QString & status);

private slots:
    void _socketError(QAbstractSocket::SocketError socketError);
    void _getDataSlot();
    void _sendDataSlot();

private:
    Ui::MainWindow *ui;
    QTcpSocket _socket;
    QDataStream _socket_stream;

    static const int _dxRacket;
    static const int _timerInterval;
};

#endif // MAINWINDOW_HPP
