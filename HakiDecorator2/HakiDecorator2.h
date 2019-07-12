/*
 *
 * Authors:
 *		s40in
 */
#ifndef HAKI_DECORATOR2_H
#define HAKI_DECORATOR2_H

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


class HakiDecor2AddOn : public DecorAddOn {
public:
								HakiDecor2AddOn(image_id id, const char* name);

protected:
	virtual Decorator*			_AllocateDecorator(DesktopSettings& settings,
									BRect rect, Desktop* desktop);
};


class HakiDecorator2: public SATDecorator {
public:
								HakiDecorator2(DesktopSettings& settings,
									BRect frame, Desktop* desktop);
	virtual						~HakiDecorator2();


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

private:
			
			rgb_color			fButtonHighColor;
			rgb_color			fButtonLowColor;

			rgb_color			fFrameHighColor;
			rgb_color			fFrameMidColor;
			rgb_color			fFrameLowColor;
			rgb_color			fFrameLowerColor;
			
			rgb_color			fFrameHighColorInactive;
			rgb_color			fFrameMidColorInactive;
			rgb_color			fFrameLowColorInactive;
			rgb_color			fFrameLowerColorInactive;
			
			rgb_color			fTabYHighColor;
			rgb_color			fTabYMidColor;
			rgb_color			fTabYLowColor;
			
			rgb_color			fTabYHighColorInactive;
			rgb_color			fTabYMidColorInactive;
			rgb_color			fTabYLowColorInactive;
			
			rgb_color			fTabBHighColor;
			rgb_color			fTabBMidColor;
			rgb_color			fTabBLowColor;
			
			rgb_color			fTabBHighColorInactive;
			rgb_color			fTabBMidColorInactive;
			rgb_color			fTabBLowColorInactive;			

			rgb_color			fFocusTextColor;
			rgb_color			fNonFocusTextColor;
			

};


#endif	// HAKI_DECORATOR2_H
