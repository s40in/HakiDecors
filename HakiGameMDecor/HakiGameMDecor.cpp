/*
 *
 * Authors:
 *		s40in
 */





#include "HakiGameMDecor.h"

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



//     #pragma mark - HakiGameMDecorAddOn


HakiGameMDecorAddOn::HakiGameMDecorAddOn(image_id id, const char* name)
	:
	DecorAddOn(id, name)
{
}


Decorator*
HakiGameMDecorAddOn::_AllocateDecorator(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
{
	return new (std::nothrow)HakiGameMDecor(settings, rect, desktop);
}


//	#pragma mark - HakiGameMDecor


HakiGameMDecor::HakiGameMDecor(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
	:
	SATDecorator(settings, rect, desktop),
	fButtonHighColor((rgb_color) { 255, 255, 255, 255 }),
	fButtonLowColor((rgb_color) { 136, 1, 5, 255 }),
	
	fFrameHighColor((rgb_color) { 1, 89, 195, 255 }), //blue
	fFrameMidColor((rgb_color) { 0, 80, 179, 255 }),
	fFrameLowColor((rgb_color) { 0, 41, 116, 255 }),
	fFrameLowerColor((rgb_color) { 0, 0, 0, 255 }),
	
	fFrameTitleHighColor ((rgb_color) { 255, 14, 48, 255 }), //red
	fFrameTitleMidColor ((rgb_color) { 236, 0, 27, 255 }),
	fFrameTitleLowColor ((rgb_color) { 136, 1, 5, 255 }),
	
	fFrameHighColorInactive((rgb_color) { 183, 87, 45, 255 }), 
	fFrameMidColorInactive((rgb_color) { 148, 72, 38, 255 }),
	fFrameLowColorInactive((rgb_color) { 108, 51, 24, 255 }),
	
	fFrameTitleHighColorInactive ((rgb_color) { 183, 87, 45, 255 }),
	fFrameTitleMidColorInactive ((rgb_color) { 148, 72, 38, 255 }),
	fFrameTitleLowColorInactive ((rgb_color) { 108, 51, 24, 255 }),	

	
	fFocusTextColor((rgb_color) { 255, 255, 255, 255 }),
	fNonFocusTextColor((rgb_color) { 192, 192, 192, 255 })
{
	STRACE(("HakiGameMDecor:\n"));
	STRACE(("\tFrame (%.1f,%.1f,%.1f,%.1f)\n",
		rect.left, rect.top, rect.right, rect.bottom));

}


HakiGameMDecor::~HakiGameMDecor()
{
	STRACE(("HakiGameMDecor: ~HakiGameMDecor()\n"));

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
HakiGameMDecor::_DrawBevelRect(const BRect rect, bool down,
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
HakiGameMDecor::_DrawFrame(BRect invalid)
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
			_DrawBevelRect(r, false, fFrameLowColor, fFrameLowColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameHighColor, fFrameMidColor);
			
			if (fTitleBarRect.IsValid()) {
				fDrawingEngine->StrokeLine(
						BPoint(fTitleBarRect.left + 2, fTitleBarRect.bottom + 1),
						BPoint(fTitleBarRect.right - 2, fTitleBarRect.bottom + 1),
						fFrameMidColor);
				}
			
			
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColor, fFrameMidColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColor,  fFrameHighColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameLowColor, fFrameLowColor);
		  }
		  else
		  {
			// first rect
			_DrawBevelRect(r, false, fFrameLowColorInactive, fFrameLowColorInactive);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameHighColorInactive, fFrameMidColorInactive);
			
			if (fTitleBarRect.IsValid()) {
				fDrawingEngine->StrokeLine(
						BPoint(fTitleBarRect.left + 2, fTitleBarRect.bottom + 1),
						BPoint(fTitleBarRect.right - 2, fTitleBarRect.bottom + 1),
						fFrameMidColorInactive);
				}
			
			
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColorInactive, fFrameMidColorInactive);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColorInactive,  fFrameHighColorInactive);
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
				fDrawingEngine->FillRect(r, fFrameMidColorInactive);
				fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameLowColorInactive);
				fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameLowColorInactive);
				r.top += 1;
				r.left += 1;
				
				fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameHighColorInactive);
				fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameHighColorInactive);
				
						
				
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


/*!	\brief Actually draws the tab

	This function is called when the tab itself needs drawn. Other items,
	like the window title or buttons, should not be drawn here.

	\param tab The \a tab to update.
	\param invalid The area of the \a tab to update.
*/
void
HakiGameMDecor::_DrawTab(Decorator::Tab* tab, BRect invalid)
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
	
	//const BRect& closeRect = tab->closeRect;
	//const BRect& zoomRect = tab->zoomRect;
		
	//rect.bottom = rect.bottom - (rect.bottom-rect.top)/2;

	if (IsFocus(tab)) {

		//fDrawingEngine->FillRect(rect, fFrameMidColor);

		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameTitleLowColor);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameTitleLowColor);
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameTitleLowColor);
		rect.left += 1;
		rect.top += 1;
		rect.right -= 1;
		
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameTitleHighColor);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameTitleHighColor);
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameTitleLowColor);
		
		rect.left += 1;
		rect.top += 1;
		rect.right -= 1;

		fDrawingEngine->FillRect(rect, fFrameTitleMidColor);
		
	}
	else {
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameTitleLowColorInactive);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameTitleLowColorInactive);
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameTitleLowColorInactive);
		rect.left += 1;
		rect.top += 1;
		rect.right -= 1;
		
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameTitleHighColorInactive);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameTitleHighColorInactive);
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameTitleLowColorInactive);
		
		rect.left += 1;
		rect.top += 1;
		rect.right -= 1;

		fDrawingEngine->FillRect(rect, fFrameTitleMidColorInactive);
	}

	_DrawTitle(tab, tabRect);

	_DrawButtons(tab, invalid);
}


/*!	\brief Actually draws the title

	The main tasks for this function are to ensure that the decorator draws
	the title only in its own area and drawing the title itself.
	Using B_OP_COPY for drawing the title is recommended because of the marked
	performance hit of the other drawing modes, but it is not a requirement.

	\param _tab The \a tab to update.
	\param r area of the title to update.
*/
void
HakiGameMDecor::_DrawTitle(Decorator::Tab* _tab, BRect r)
{
	STRACE(("_DrawTitle(%f, %f, %f, %f)\n", r.left, r.top, r.right, r.bottom));

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


/*!	\brief Actually draws the close button

	Unless a subclass has a particularly large button, it is probably
	unnecessary to check the update rectangle.

	\param _tab The \a tab to update.
	\param direct Draw without double buffering.
	\param rect The area of the button to update.
*/
void
HakiGameMDecor::_DrawClose(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawClose(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));
		
  Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);
  
  if (IsFocus(fTopTab)) {		
  	fDrawingEngine->FillRect(rect, fFrameTitleMidColor);
  }
  else {
  	fDrawingEngine->FillRect(rect, fFrameTitleMidColorInactive);
  }
  
  rect.InsetBy(4,4);
 	
  BRect rectShadow(rect);
	
  rectShadow.left += 1;
  rectShadow.top += 1;
  rectShadow.right +=1;
  rectShadow.bottom +=1;
	
	
  if (IsFocus(fTopTab)) {		
	
	// --- draw shadow
	if (!tab->closePressed) {
		fDrawingEngine->SetHighColor(fFrameTitleLowColor);
		fDrawingEngine->StrokeLine(BPoint(rectShadow.left, rectShadow.top),
								BPoint(rectShadow.right, rectShadow.bottom));
		fDrawingEngine->StrokeLine(BPoint(rectShadow.left, rectShadow.top+1),
								BPoint(rectShadow.right-1, rectShadow.bottom));		
		fDrawingEngine->StrokeLine(BPoint(rectShadow.left+1, rectShadow.top),
								BPoint(rectShadow.right, rectShadow.bottom-1));										
					  
		fDrawingEngine->StrokeLine(BPoint(rectShadow.left, rectShadow.bottom),
								BPoint(rectShadow.right, rectShadow.top));
		fDrawingEngine->StrokeLine(BPoint(rectShadow.left+1, rectShadow.bottom),
								BPoint(rectShadow.right, rectShadow.top+1));
		fDrawingEngine->StrokeLine(BPoint(rectShadow.left, rectShadow.bottom-1),
								BPoint(rectShadow.right-1, rectShadow.top));
	}

 
		fDrawingEngine->SetHighColor(fButtonHighColor);
		fDrawingEngine->StrokeLine(BPoint(rect.left, rect.top),
								BPoint(rect.right, rect.bottom));
		fDrawingEngine->StrokeLine(BPoint(rect.left, rect.top+1),
								BPoint(rect.right-1, rect.bottom));		
		fDrawingEngine->StrokeLine(BPoint(rect.left+1, rect.top),
								BPoint(rect.right, rect.bottom-1));										
					  
		fDrawingEngine->StrokeLine(BPoint(rect.left, rect.bottom),
								BPoint(rect.right, rect.top));
		fDrawingEngine->StrokeLine(BPoint(rect.left+1, rect.bottom),
								BPoint(rect.right, rect.top+1));
		fDrawingEngine->StrokeLine(BPoint(rect.left, rect.bottom-1),
								BPoint(rect.right-1, rect.top));
								
  }
		
}


/*!	\brief Actually draws the zoom button

	Unless a subclass has a particularly large button, it is probably
	unnecessary to check the update rectangle.

	\param _tab The \a tab to update.
	\param direct Draw without double buffering.
	\param rect The area of the button to update.
*/
void
HakiGameMDecor::_DrawZoom(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawZoom(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));

	if (rect.IntegerWidth() < 1)
		return;
		
	
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);	
	
  if (IsFocus(fTopTab)) {		
  	fDrawingEngine->FillRect(rect, fFrameTitleMidColor);
  }
  else {
  	fDrawingEngine->FillRect(rect, fFrameTitleMidColorInactive);
  }
  
  rect.InsetBy(2,2);
 	
 
 float buttonH, buttonW;
 buttonH = (int)(rect.right - rect.left)/2;
 buttonW = (int)(rect.bottom - rect.top)/2;
 //buttonW = buttonH;
 
 BRect LeftSq( rect.left+1, rect.top+1, rect.left+1+buttonW, rect.top+1+buttonH);
 BRect RightSq( rect.right-1-buttonW, rect.bottom-1-buttonH, rect.right-1, rect.bottom-1);
 RightSq.left -=2;
 RightSq.top -=2;
 
	
		
  if (IsFocus(fTopTab)) {	
		fDrawingEngine->FillRect(LeftSq, fButtonHighColor);
		fDrawingEngine->FillRect(RightSq, fButtonHighColor);

	if (!tab->zoomPressed) {
		fDrawingEngine->SetHighColor(fFrameTitleLowColor);
		fDrawingEngine->SetLowColor(fFrameTitleMidColor);				
	}
	else {
		fDrawingEngine->SetHighColor(fFrameTitleMidColor);
		fDrawingEngine->SetLowColor(fFrameTitleMidColor);
	}
	
		fDrawingEngine->StrokeLine(BPoint(LeftSq.left, LeftSq.bottom+1),
								BPoint(LeftSq.right+1, LeftSq.bottom+1));
		fDrawingEngine->StrokeLine(BPoint(LeftSq.right+1, LeftSq.bottom),
								BPoint(LeftSq.right+1, LeftSq.top+1));			
								
		fDrawingEngine->StrokeLine(BPoint(RightSq.left+1, RightSq.bottom+1),
								BPoint(RightSq.right+1, RightSq.bottom+1));
		fDrawingEngine->StrokeLine(BPoint(RightSq.right+1, RightSq.bottom),
								BPoint(RightSq.right+1, RightSq.top+1));	

  }
		
		
 	
}


void
HakiGameMDecor::_DrawMinimize(Decorator::Tab* tab, bool direct, BRect rect)
{
	// This decorator doesn't have this button
}


void
HakiGameMDecor::_GetButtonSizeAndOffset(const BRect& tabRect, float* _offset,
	float* _size, float* _inset) const
{
	float tabSize = fTopTab->look == kLeftTitledWindowLook ?
		tabRect.Width() : tabRect.Height();

	*_offset = 5.0f;
	*_inset = 0.0f;

	*_size = std::max(0.0f, tabSize - 7.0f);
}


// #pragma mark - Private methods



extern "C" DecorAddOn* (instantiate_decor_addon)(image_id id, const char* name)
{
	return new (std::nothrow)HakiGameMDecorAddOn(id, name);
}
