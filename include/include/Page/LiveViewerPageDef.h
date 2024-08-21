// LiveViewerPageDef.h
#ifndef LIVEVIEWERPAGEDEF_H
#define LIVEVIEWERPAGEDEF_H

namespace SoLive::Page
{
	constexpr auto ROOM_ID_LEN = 8;

	constexpr auto WARNING_INPUT_LESS_NUM	= "输入必须为8位";
	constexpr auto WARNING_INPUT_INVALID	= "输入必须为字母和数字";
	constexpr auto BTN_RECORD				= "录制";
	constexpr auto BTN_STOP_RECORD			= "结束";

	// client
	constexpr auto EVENT_CREATE_ROOM			= "createMeetRoom";
	constexpr auto EVENT_ENTER_ROOM				= "enterMeetRoom";
	constexpr auto EVENT_LEAVE_ROOM				= "leaveMeetRoom";
	constexpr auto EVENT_GET_RTP_CAPA			= "getRouterRtpCapabilities";
	constexpr auto EVENT_CREATE_PROD_TRP		= "createProducerTransport";
	constexpr auto EVENT_CONN_PROD_TRP			= "connectProducerTransport";
	constexpr auto EVENT_RRODUCE				= "produce";
	constexpr auto EVENT_CREATE_CONS_TRP		= "createConsumerTransport";
	constexpr auto EVENT_CONNECT_CONS_TRP		= "connectConsumerTransport";
	constexpr auto EVENT_RESUME					= "resume";
	constexpr auto EVENT_CONSUME				= "consume";
	constexpr auto EVENT_CONSUME_NEW_PROD		= "consumeNewProducer";
	constexpr auto EVENT_PUSH_MSG				= "pushSfuLiveMsgs";

	// server
	constexpr auto EVENT_ROOM_NOT_EXIST			= "liveRoomNotExist";
	constexpr auto EVENT_ROOM_ENTERED			= "meetRoomEntered";
	constexpr auto EVENT_RTP_CAPA_GOT			= "routerRtpCapabilities";
	constexpr auto EVENT_CONS_TRP_CREATED		= "consumerTransportCreated";
	constexpr auto EVENT_SUBSCRIBED				= "subscribed";
	constexpr auto EVENT_CONS_TRP_CONNECTED		= "consumerConnected";
	constexpr auto EVENT_RESUMED				= "resumed";

}

#endif // LIVEVIEWERPAGEDEF_H
