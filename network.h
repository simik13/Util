#ifndef MYHTTPCLIENT_H
#define MYHTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>

class network : public QObject
{
    Q_OBJECT
public:
    explicit network(QObject *parent = 0);
    void doUpload(const QString& url, const QByteArray &data);

signals:

public slots:
    void slotReplyFinishedHandler(QNetworkReply*);

private:
    QNetworkAccessManager* m_pqnaManager;
};

#endif // MYHTTPCLIENT_H
