// UtilDef.h
#ifndef UTILDEF_H
#define UTILDEF_H

namespace SoLive::Util
{
	enum class RecordMode
	{
		VideoAndAudio,
		OnlyVideo,
		OnlyAudio
	};
	constexpr auto VIDEO_AND_AUDIO	= "视频和音频";
	constexpr auto ONLY_VIDEO		= "仅视频";
	constexpr auto ONLY_AUDIO		= "仅音频";
}

#endif // UTILDEF_H
