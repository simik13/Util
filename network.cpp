#include "network.h"

#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QDateTime>
#include <QHttpPart>

network::network(QObject *parent) : QObject(parent)
{
    m_pqnaManager = new QNetworkAccessManager(this);
    connect(m_pqnaManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotReplyFinishedHandler(QNetworkReply*)));
}
//----------------------------------------------------------------------

void network::slotReplyFinishedHandler(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << " [!]ERROR: " << reply->errorString();
    }
    else
    {
        qDebug() << " [-]Reply:";
        qDebug() << " [|] " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << " [|] " << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        qDebug() << " [|] " << reply->readAll();
        qDebug() << " [-]END";
    }
    reply->deleteLater();
}
//----------------------------------------------------------------------

void network::doUpload(const QString &url, const QByteArray& data)
{
       qDebug()<<"DATA "<<data;
       QNetworkAccessManager *manager = new QNetworkAccessManager(this);
       QEventLoop eLoop;
       QNetworkRequest request;
       request.setUrl(url);
       request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
       request.setHeader(QNetworkRequest::ContentLengthHeader, data.count());
       QNetworkReply *reply = manager->post(request, data);
       QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &eLoop, SLOT(quit()));
       try {
           eLoop.exec();
           if(reply->error() != QNetworkReply::NoError) {
               throw reply;
           } else {
               if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString()=="200"){
                   qDebug() << "[]:\tSent request"<<reply->errorString();
               }
           }
       } catch(QNetworkReply *) {
           qDebug() << "[!]ERROR:\t" << reply->errorString();
       }
       delete manager;

}
//----------------------------------------------------------------------
