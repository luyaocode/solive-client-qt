// PageDef.h
#ifndef PAGEDEF_H
#define PAGEDEF_H

namespace SoLive::Page
{
	enum class Page
	{
		Home,
		LiveViewer,
		Meet,
	};
	constexpr int			VIDEO_WIDTH_ORIGIN = 1600;
	constexpr double		ASPECT_RATIO = 640.0 / 480;
}

#endif // PAGEDEF_H
