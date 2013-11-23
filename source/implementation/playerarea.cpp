#include "ramplayer.h"

PlayerArea::PlayerArea()
{
	map = BaseBitmap::Alloc(); //You may also use AutoAlloc directly as a member
}

PlayerArea::~PlayerArea(void)
{
	if(gmap)
		BaseBitmap::Free(map);
}

Bool PlayerArea::Init()
{
	gmap = TRUE;
	if(gmap && map)
	{
		map->Init(200,200,24);
		LONG r,g,b;
		GeGetGray(&r,&g,&b);
		map->Clear(r,g,b);
	}
	return TRUE;
}

Bool PlayerArea::GetMinSize(LONG& w, LONG& h)
{	
	if(map)
	{
		w = map->GetBw();
		h = map->GetBh();
	}
	else
	{
		w = 320;
		h = 240;
	}
	return TRUE;
}

void PlayerArea::Sized(LONG w,LONG h)
{
}

void PlayerArea::DrawMsg(LONG x1,LONG y1,LONG x2,LONG y2,const BaseContainer &msg)
{
	OffScreenOn();
	
	if(map)
	{
		LONG w = map->GetBw();
		LONG h = map->GetBh();
		DrawBitmap(map,x1,y1,x2,y2,0,0,w,h,BMP_NORMAL);
	}
}
