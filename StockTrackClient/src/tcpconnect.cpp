//INCLUDE QT

//INCLUDE STANDARD LIBRARY

//INLCUDE EXTERNAL

//INLCUDE PROJECT
#include "header/tcpconnect.h"

TCPConnect::TCPConnect(QObject* parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_serverIP("192.168.68.105")
    , m_serverPort(20580)
{
    connect(m_socket, &QTcpSocket::connected, this, &TCPConnect::connected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TCPConnect::disconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &TCPConnect::handleError);
    connect(m_socket, &QTcpSocket::readyRead, this, &TCPConnect::handleReadyRead);
}

//INVOKABLE FUNCTIONS
void TCPConnect::connectToServer()
{
    qDebug() << "Connection to server at " << m_serverIP << ":" << m_serverPort;

    if(m_socket->state() == QAbstractSocket::UnconnectedState)
    {
        m_socket->connectToHost(m_serverIP, m_serverPort);

        if(!m_socket->waitForConnected(5000))
        {
            qDebug() << "Connection timeout " << m_socket->errorString();
        }
    }
}

void TCPConnect::disconnectFromServer()
{
    qDebug() << "TCPClass trying to disconnect from server. Current State: " << m_socket->state();

    if(m_socket->state() == QAbstractSocket::ConnectedState)
    {
        //Start to disconnect
        m_socket->disconnectFromHost();

        //Only wait for disconnect if we're still connected
        if(m_socket->state() != QAbstractSocket::UnconnectedState)
        {
            qDebug() << "Disconnect timed out of failed " << m_socket->errorString();
            m_socket->abort();
        }

    }
}

void TCPConnect::sendRequest(const QString& request)
{
    qDebug() << "Attempting to connect...";

    connectToServer();
    if(m_socket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << "Connected, sending request.";
        m_socket->write(request.toUtf8());

        //Wait for the write to complete
        if(!m_socket->waitForBytesWritten(5000))
        {
            qDebug() << "Write timeout: " << m_socket->errorString();
            emit errorOccurred("Failed to write data to server.");
            disconnectFromServer();
            return;
        }
        //No need to disconnect yet, let handleReadyRead to do it
    }
    else
    {
        qDebug() << "Failed to connect to server";
        emit errorOccurred("Not connected to server");
    }
}

//SLOTS
void TCPConnect::handleConnected()
{
    qDebug() << "Connected to server successfully.";
}

void TCPConnect::handleDisconnected()
{
    qDebug() << "Disconnected from server successfully.";
}

void TCPConnect::handleError(QAbstractSocket::SocketError stocketError)
{
    qDebug() << "socket error : " <<  m_socket->errorString();
    emit errorOccurred(m_socket->errorString());
}

void TCPConnect::handleReadyRead()
{
    QByteArray data = m_socket->readAll();
    QString response = QString::fromUtf8(data);


    //Ensure we are receiving all data
    if(m_socket->bytesAvailable() == 0)
    {
        emit dataReceived(response);
        disconnectFromServer();

    }
}
