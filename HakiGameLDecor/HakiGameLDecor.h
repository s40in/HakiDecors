/*
 *
 * Authors:
 *		s40in
 */
#ifndef HAKI_GAME_L_DECOR_H
#define HAKI_GAME_L_DECOR_H

// build with makefile or Jamfile (in Haiku sorce tree)
#ifdef USEMAKEFILE
#include "/system/develop/sources/haiku/headers/os/interface/View.h"
#include "/system/develop/sources/haiku/headers/os/interface/Layout.h"
#include "/system/develop/sources/haiku/headers/os/interface/Window.h"
#else
#include "View.h"
#include "Layout.h"
#include "Window.h"
#endif

#include "DecorManager.h"
#include "SATDecorator.h"
#include "RGBColor.h"


class Desktop;
class ServerBitmap;


class HakiGameLDecorAddOn : public DecorAddOn {
public:
								HakiGameLDecorAddOn(image_id id, const char* name);

protected:
	virtual Decorator*			_AllocateDecorator(DesktopSettings& settings,
									BRect rect, Desktop* desktop);
};


class HakiGameLDecor: public SATDecorator {
public:
								HakiGameLDecor(DesktopSettings& settings,
									BRect frame, Desktop* desktop);
	virtual						~HakiGameLDecor();


protected:
	virtual	void				_DrawFrame(BRect rect);

	virtual	void				_DrawTab(Decorator::Tab* tab, BRect rect);
	virtual	void				_DrawTitle(Decorator::Tab* tab, BRect rect);
	virtual	void				_DrawClose(Decorator::Tab* tab, bool direct,
									BRect rect);
	virtual	void				_DrawZoom(Decorator::Tab* tab, bool direct,
									BRect rect);
	virtual	void				_DrawMinimize(Decorator::Tab* tab, bool direct,
									BRect rect);

	virtual	void				_GetButtonSizeAndOffset(const BRect& tabRect,
									float* offset, float* size,
									float* inset) const;
			void				_DrawBevelRect(const BRect rect, bool down,
									rgb_color light, rgb_color shadow);								

private:
			
			rgb_color			fButtonHighColor;
			rgb_color			fButtonLowColor;

			rgb_color			fFrameHighColor;
			rgb_color			fFrameMidColor;
			rgb_color			fFrameLowColor;
			rgb_color			fFrameLowerColor;
			
			rgb_color			fFrameTitleHighColor;
			rgb_color			fFrameTitleMidColor;
			rgb_color			fFrameTitleLowColor;
			
			rgb_color			fFrameHighColorInactive;
			rgb_color			fFrameMidColorInactive;
			rgb_color			fFrameLowColorInactive;
			
			rgb_color			fFrameTitleHighColorInactive;
			rgb_color			fFrameTitleMidColorInactive;
			rgb_color			fFrameTitleLowColorInactive;
		

			rgb_color			fFocusTextColor;
			rgb_color			fNonFocusTextColor;
			

};


#endif	// HAKI_GAME_M_DECOR_H
