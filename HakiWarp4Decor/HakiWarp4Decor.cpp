/*
 *
 * Authors:
 *		s40in
 */





#include "HakiWarp4Decor.h"

#include <algorithm>
#include <cmath>
#include <new>
#include <stdio.h>

#include <WindowPrivate.h>

#include <Autolock.h>
#include <Debug.h>
#include <GradientLinear.h>
#include <Rect.h>
#include <Region.h>
#include <View.h>

#include "BitmapDrawingEngine.h"
#include "Desktop.h"
#include "DesktopSettings.h"
#include "DrawingEngine.h"
#include "DrawState.h"
#include "FontManager.h"
#include "PatternHandler.h"
#include "RGBColor.h"
#include "ServerBitmap.h"


//#define DEBUG_DECORATOR
#ifdef DEBUG_DECORATOR
#	define STRACE(x) printf x
#else
#	define STRACE(x) ;
#endif


static const float kBorderResizeLength = 22.0;
static const float kResizeKnobSize = 18.0;


static const unsigned char f = 0xff; // way to write 0xff shorter



//     #pragma mark - HakiWarp4DecorAddOn


HakiWarp4DecorAddOn::HakiWarp4DecorAddOn(image_id id, const char* name)
	:
	DecorAddOn(id, name)
{
}


Decorator*
HakiWarp4DecorAddOn::_AllocateDecorator(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
{
	return new (std::nothrow)HakiWarp4Decor(settings, rect, desktop);
}


//	#pragma mark - HakiWarp4Decor


HakiWarp4Decor::HakiWarp4Decor(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
	:
	SATDecorator(settings, rect, desktop),
	fButtonHighColor((rgb_color) { 255, 255, 255, 255 }),
	fButtonLowColor((rgb_color) { 132, 132, 132, 255 }),
	
	fFrameHighColor((rgb_color) { 255, 255, 255, 255 }), // ala gray
	fFrameMidColor((rgb_color) { 216, 216, 216, 255 }),
	fFrameLowColor((rgb_color) { 132, 132, 132, 255 }),
	fFrameLowerColor((rgb_color) { 8, 8, 8, 255 }),
	fFrameTitleColor ((rgb_color) { 41, 0, 173, 255 }),
	
	fFrameHighColorInactive((rgb_color) { 255, 255, 255, 255 }), //gray
	fFrameMidColorInactive((rgb_color) { 216, 216, 216, 255 }),
	fFrameLowColorInactive((rgb_color) { 132, 132, 132, 255 }),
	fFrameTitleColorInactive ((rgb_color) { 128, 128, 128, 255 }),

	
	fFocusTextColor((rgb_color) { 255, 255, 255, 255 }),
	fNonFocusTextColor((rgb_color) { 192, 192, 192, 255 })
{
	STRACE(("HakiWarp4Decor:\n"));
	STRACE(("\tFrame (%.1f,%.1f,%.1f,%.1f)\n",
		rect.left, rect.top, rect.right, rect.bottom));

}


HakiWarp4Decor::~HakiWarp4Decor()
{
	STRACE(("HakiWarp4Decor: ~HakiWarp4Decor()\n"));

}

// #pragma mark - Private methods
/*!
	\brief Draws a bevel around a rectangle.
	\param rect The rectangular area to draw in.
	\param down Whether or not the button is pressed down.
	\param light The light color to use.
	\param shadow The shadow color to use.
*/
void
HakiWarp4Decor::_DrawBevelRect(const BRect rect, bool down,
	rgb_color light, rgb_color shadow)
{
	if (down) {
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.LeftBottom(), light);	//bottom
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), shadow); //left
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), shadow); 	//top
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), light);	// right

	} else {
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.LeftBottom(), shadow);	//bottom		
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), light); //left
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), light); 	//top
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), shadow);	// right

	}
}


// #pragma mark - Protected methods
void
HakiWarp4Decor::_DrawFrame(BRect invalid)
{
	STRACE(("_DrawFrame(%f,%f,%f,%f)\n", invalid.left, invalid.top,
		invalid.right, invalid.bottom));

	// NOTE: the DrawingEngine needs to be locked for the entire
	// time for the clipping to stay valid for this decorator

	if (fTopTab->look == B_NO_BORDER_WINDOW_LOOK)
		return;

	if (fBorderWidth <= 0)
		return;

	// Draw the border frame
	BRect r = BRect(fTopBorder.LeftTop(), fBottomBorder.RightBottom());
	
	
	switch ((int)fTopTab->look) {
		case B_TITLED_WINDOW_LOOK:
		case B_DOCUMENT_WINDOW_LOOK:
		case B_MODAL_WINDOW_LOOK:
		case B_FLOATING_WINDOW_LOOK:
		case kLeftTitledWindowLook:
		{
		  if (IsFocus(fTopTab)) {
			// first rect
			_DrawBevelRect(r, false, fFrameLowColor, fFrameLowerColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameHighColor, fFrameLowColor);
			
			if (fTitleBarRect.IsValid()) {
				fDrawingEngine->StrokeLine(
						BPoint(fTitleBarRect.left + 2, fTitleBarRect.bottom + 1),
						BPoint(fTitleBarRect.right - 2, fTitleBarRect.bottom + 1),
						fFrameMidColor);
				}
			
			
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColor, fFrameMidColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColor,  fFrameMidColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameLowColor, fFrameLowColor);
		  }
		  else
		  {
			// first rect
			_DrawBevelRect(r, false, fFrameLowColorInactive, fFrameLowerColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameHighColorInactive, fFrameLowColorInactive);
			
			if (fTitleBarRect.IsValid()) {
				fDrawingEngine->StrokeLine(
						BPoint(fTitleBarRect.left + 2, fTitleBarRect.bottom + 1),
						BPoint(fTitleBarRect.right - 2, fTitleBarRect.bottom + 1),
						fFrameMidColorInactive);
				}
			
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColorInactive, fFrameMidColorInactive);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColorInactive,  fFrameMidColorInactive);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameLowColorInactive, fFrameLowColorInactive);
		  }		
			break;
		}

		/*
		case B_FLOATING_WINDOW_LOOK:
		case kLeftTitledWindowLook:
		{
			_DrawBevelRect(r, false, fFrameHighColorInactive, fFrameLowColorInactive);
			break;
		}
		*/

		case B_BORDERED_WINDOW_LOOK:
		{
			_DrawBevelRect(r, false, fFrameHighColor, fFrameLowColor);
			break;
		}

		default:
			// don't draw a border frame
			break;
	}

	// Draw the resize knob if we're supposed to
	if (!(fTopTab->flags & B_NOT_RESIZABLE)) {
		r = fResizeRect;


		switch ((int)fTopTab->look) {
			case B_DOCUMENT_WINDOW_LOOK:
			case B_TITLED_WINDOW_LOOK:
			case B_FLOATING_WINDOW_LOOK:
			case B_MODAL_WINDOW_LOOK:
			case kLeftTitledWindowLook:
			{
				if (!invalid.Intersects(r))
					break;
					
				r.right -= 4;	
				r.bottom -= 4;
				fDrawingEngine->FillRect(r, fFrameMidColor);
				fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameLowColor);
				fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameLowColor);
				r.top += 1;
				r.left += 1;
				
				fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameHighColor);
				fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameHighColor);
				
				/*
				if (IsFocus(fTopTab)) {
					fDrawingEngine->FillRect(r, fFrameMidColor);
					_DrawBevelRect(r, true, fFrameHighColor, fFrameLowColor);
					r.InsetBy(1, 1);
					_DrawBevelRect(r, false, fFrameHighColor, fFrameLowColor);
		
				}
				else {
					fDrawingEngine->FillRect(r, fFrameMidColorInactive);
					_DrawBevelRect(r, true, fFrameHighColorInactive, fFrameLowColorInactive);
					r.InsetBy(1, 1);
					_DrawBevelRect(r, false, fFrameHighColorInactive, fFrameLowColorInactive);
					
				}
				*/

				if (fTopTab && !IsFocus(fTopTab))
					break;
				break;
			}

			/*
			case B_TITLED_WINDOW_LOOK:
			case B_FLOATING_WINDOW_LOOK:
			case B_MODAL_WINDOW_LOOK:
			case kLeftTitledWindowLook:
			{
				
				if (!invalid.Intersects(BRect(fRightBorder.right
						- kBorderResizeLength,
					fBottomBorder.bottom - kBorderResizeLength,
					fRightBorder.right - 1, fBottomBorder.bottom - 1))) {
					break;
				}

				fDrawingEngine->StrokeLine(BPoint(fRightBorder.left,
					fBottomBorder.bottom - kBorderResizeLength),
					BPoint(fRightBorder.right - 1,
						fBottomBorder.bottom - kBorderResizeLength),
					fTabBHighColor);
				fDrawingEngine->StrokeLine(
					BPoint(fRightBorder.right - kBorderResizeLength,
						fBottomBorder.top),
					BPoint(fRightBorder.right - kBorderResizeLength,
						fBottomBorder.bottom - 1),
					fTabBHighColor);
				
				break;
			}
			*/

			default:
				// don't draw resize corner
				break;
		}
	}
}



void
HakiWarp4Decor::_DrawTab(Decorator::Tab* tab, BRect invalid)
{
	STRACE(("_DrawTab(%.1f, %.1f, %.1f, %.1f)\n",
			invalid.left, invalid.top, invalid.right, invalid.bottom));
	const BRect& tabRect = tab->tabRect;
	// If a window has a tab, this will draw it and any buttons which are
	// in it.
	if (!tabRect.IsValid() || !invalid.Intersects(tabRect))
		return;
		
	BRect rect(tab->tabRect);
	//BRect rectMyClose(tab->tabRect);
	//BRect rectMyZoom(tab->tabRect);
	
	const BRect& closeRect = tab->closeRect;
	const BRect& zoomRect = tab->zoomRect;
		
	//rect.bottom = rect.bottom - (rect.bottom-rect.top)/2;

	if (IsFocus(tab)) {

		fDrawingEngine->FillRect(rect, fFrameMidColor);

		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameLowColor);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameLowColor);
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameLowerColor);
		rect.left += 1;
		rect.top += 1;
		rect.right -= 1;
		
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameHighColor);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameHighColor);
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameLowColor);
		
		rect.top += 4;
		rect.bottom -= 2;
		rect.left += 3;
		rect.right -= 3;
		
		if (closeRect.IsValid()){
			rect.left = closeRect.right +1;
		}
		else {
			rect.left +=  2;
		}
		
		if (zoomRect.IsValid()){
			rect.right = zoomRect.left -1;
		
		}
		else {
			rect.right -= 2;
		}
		
		
		_DrawBevelRect(rect, false, fFrameLowColor, fFrameHighColor);
		rect.InsetBy(1, 1);
		fDrawingEngine->FillRect(rect, fFrameTitleColor);
		
	}
	else {
		fDrawingEngine->FillRect(rect, fFrameMidColorInactive);

		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameLowColorInactive);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameLowColorInactive);
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameLowerColor);
		rect.left += 1;
		rect.top += 1;
		rect.right -= 1;
		
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameHighColorInactive);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameHighColorInactive);
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameLowColorInactive);
		
		rect.top += 4;
		rect.bottom -= 2;
		rect.left += 3;
		rect.right -= 3;
		
		if (closeRect.IsValid()){
			rect.left = closeRect.right +1;
		}
		
		if (zoomRect.IsValid()){
			rect.right = zoomRect.left -1;
		
		}
		else {
			rect.right -= 2;
		}
		
		
		_DrawBevelRect(rect, false, fFrameLowColorInactive, fFrameHighColorInactive);
		rect.InsetBy(1, 1);
		fDrawingEngine->FillRect(rect, fFrameTitleColorInactive);
	}

	_DrawTitle(tab, tabRect);

	_DrawButtons(tab, invalid);
}


void
HakiWarp4Decor::_DrawTitle(Decorator::Tab* _tab, BRect r)
{


	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);

	const BRect& tabRect = tab->tabRect;
	const BRect& closeRect = tab->closeRect;
	const BRect& zoomRect = tab->zoomRect;


	fDrawingEngine->SetDrawingMode(B_OP_OVER);
	if (IsFocus(fTopTab)) {
		fDrawingEngine->SetHighColor(fFocusTextColor);
		fDrawingEngine->SetLowColor(fFrameLowColor);
	}
	else {
		fDrawingEngine->SetHighColor(fNonFocusTextColor);
		fDrawingEngine->SetLowColor(fFrameLowColorInactive);
	}
	fDrawingEngine->SetFont(fDrawState.Font());

	// figure out position of text
	font_height fontHeight;
	fDrawState.Font().GetHeight(fontHeight);

	BPoint titlePos;
	if (fTopTab->look != kLeftTitledWindowLook) {
		titlePos.x = closeRect.IsValid() ? closeRect.right + tab->textOffset
			: tabRect.left + tab->textOffset;
		titlePos.y = floorf(((tabRect.top + 2.0) + tabRect.bottom
			+ fontHeight.ascent + fontHeight.descent) / 2.0
			- fontHeight.descent + 0.5);
	} else {
		titlePos.x = floorf(((tabRect.left + 2.0) + tabRect.right
			+ fontHeight.ascent + fontHeight.descent) / 2.0
			- fontHeight.descent + 0.5);
		titlePos.y = zoomRect.IsValid() ? zoomRect.top - tab->textOffset
			: tabRect.bottom - tab->textOffset;
	}

	fDrawingEngine->SetFont(fDrawState.Font());

	fDrawingEngine->DrawString(tab->truncatedTitle.String(),
		tab->truncatedTitleLength, titlePos);

	fDrawingEngine->SetDrawingMode(B_OP_COPY);
}


void
HakiWarp4Decor::_DrawClose(Decorator::Tab* _tab, bool direct, BRect rect)
{
		
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);
	
	rect.top += 2;
	rect.left += 1;
	rect.bottom -= 2;
	rect.right -= 3;
	
	
	_DrawBevelRect(rect, tab->closePressed, fButtonHighColor, fButtonLowColor);
	rect.InsetBy(2,2);
	
	
	fDrawingEngine->SetHighColor(fButtonLowColor);
	
	fDrawingEngine->StrokeLine(BPoint(rect.left, rect.bottom-4),
								BPoint(rect.left, rect.top));
	fDrawingEngine->StrokeLine(BPoint(rect.left, rect.top),
								BPoint(rect.right-4, rect.top));
								
	fDrawingEngine->StrokeLine(BPoint(rect.left+2, rect.bottom-1),
								BPoint(rect.right-1, rect.top+2));							
				
								
	fDrawingEngine->SetHighColor(fButtonHighColor);	
	fDrawingEngine->StrokeLine(BPoint(rect.right-3, rect.top+1),
								BPoint(rect.left+1, rect.bottom-2));	
								
	fDrawingEngine->StrokeLine(BPoint(rect.left+3, rect.bottom),
								BPoint(rect.right, rect.bottom));
	
	fDrawingEngine->StrokeLine(BPoint(rect.right, rect.bottom),
								BPoint(rect.right, rect.top+3));														

		
}


void
HakiWarp4Decor::_DrawZoom(Decorator::Tab* _tab, bool direct, BRect rect)
{

	if (rect.IntegerWidth() < 1)
		return;
		
	rect.top += 2;
	rect.left += 3;
	rect.bottom -= 2;
	rect.right -= 1;
	
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);	
	
	_DrawBevelRect(rect, tab->zoomPressed, fButtonHighColor, fButtonLowColor);
	rect.InsetBy(2,2);
	_DrawBevelRect(rect, tab->zoomPressed, fButtonLowColor, fButtonHighColor);
		
 	
}


void
HakiWarp4Decor::_DrawMinimize(Decorator::Tab* tab, bool direct, BRect rect)
{
	// This decorator doesn't have this button
}





extern "C" DecorAddOn* (instantiate_decor_addon)(image_id id, const char* name)
{
	return new (std::nothrow)HakiWarp4DecorAddOn(id, name);
}
