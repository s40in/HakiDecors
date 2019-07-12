/*
 *
 * Authors:
 *		s40in
 */
#ifndef HAKI_ASCIIDECORATOR_H
#define HAKI_ASCIIDECORATOR_H

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


class HakiAsciiDecoratorAddOn : public DecorAddOn {
public:
								HakiAsciiDecoratorAddOn(image_id id, const char* name);

protected:
	virtual Decorator*			_AllocateDecorator(DesktopSettings& settings,
									BRect rect, Desktop* desktop);
};


class HakiAsciiDecorator: public SATDecorator {
public:
								HakiAsciiDecorator(DesktopSettings& settings,
									BRect frame, Desktop* desktop);
	virtual						~HakiAsciiDecorator();


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
			
			rgb_color			fButtonColor;
			rgb_color			fButtonColorInactive;
			rgb_color			fButtonColorPressed;

			rgb_color			fTabColor;
			rgb_color			fTabColorInactive;
			
			rgb_color			fFrameColor;
			rgb_color			fFrameColorInactive;
			
			rgb_color			fFocusTextColor;
			rgb_color			fNonFocusTextColor;
			

};


#endif	// HAKI_ASCIIDECORATOR_H
