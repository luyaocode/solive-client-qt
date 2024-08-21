#ifndef SOCKETCLIENTIMPL_H
#define SOCKETCLIENTIMPL_H
#include "SocketClient.h"
#include <QJsonDocument>
#include <QString>
#include <string>
#include "Logger.h"

namespace SoLive::ProtocolSocketClient
{
    template<typename PTy>
    void SocketClient::emit(const QString& eName, PTy jsonObj)
    {
        static_assert(std::is_same_v<PTy, std::shared_ptr<QJsonObject>> ||
            std::is_same_v<PTy, std::unique_ptr<QJsonObject>> ||
            std::is_same_v<PTy, QJsonObject*>, "PTy must be std::shared_ptr<QJsonObject>, std::unique_ptr<QJsonObject>, or QJsonObject*");
        std::unique_lock<std::mutex> lock(_mtx);

        QString jsonString;
        if (jsonObj)
        {
            QJsonDocument doc(*jsonObj);
            jsonString = doc.toJson(QJsonDocument::Compact);
        }

        std::string strEName = eName.toUtf8().constData();
        std::string strJson = jsonString.toUtf8().constData();
        sio::message::list msg_list(strJson);
        _client.socket()->emit(strEName, msg_list);

        lock.unlock();


        LOG(Debug, "[" + strEName + "]: " + strJson)
    }
}

#endif // SOCKETCLIENTIMPL_H
