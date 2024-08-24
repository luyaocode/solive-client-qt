#include "stdafx.h"
#include "SioAdapter.h"
using namespace SoLive::ProtocolSocketClient;

void SioAdapter::connect(const std::string& url)
{
	_client.connect(url);
}
void SioAdapter::emit(const std::string& eName, const std::string& msg)
{
    sio::message::list msg_list(msg);
	_client.socket()->emit(eName, msg_list);
}
void SioAdapter::listen(const std::string& eName, const std::function<void(const EventVariant& event)>& callback)
{
	_client.socket()->on(eName,callback);
}
void SioAdapter::close()
{

}

std::string SioAdapter::id()
{
	return "";
}
