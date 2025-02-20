#ifndef STACKTRACK_TCPCONNECT_H
#define STACKTRACK_TCPCONNECT_H

//INCLUDE QT
#include <QObject>
#include <QTcpSocket>

//INCLUDE STANDARD LIBRARY

//INCLUDE EXTERNAL

//INCLUDE PROJECT

class TCPConnect : public QObject
{
    Q_OBJECT
public:

    explicit TCPConnect(QObject* parent = nullptr);

    ~TCPConnect() = default;

    Q_INVOKABLE void connectToServer();
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void sendRequest(const QString& request);

signals:

    void connected();
    void disconnected();
    void dataReceived(const QString& data);
    void errorOccurred(const QString& errorMessage);

    void stockListReceived(const QStringList& stockList);
    void priceDataReceived(const QString& stockName, int hour, double price);

private slots:
    void handleConnected();
    void handleDisconnected();
    void handleError(QAbstractSocket::SocketError stocketError);
    void handleReadyRead();

private:

    //This is inherited by QObject, so there is no need to delete it, Qt will handle it
    QTcpSocket* m_socket;
    const QString m_serverIP;
    const quint16 m_serverPort;

};

#endif //STACKTRACK_TCPCONNECT_H
