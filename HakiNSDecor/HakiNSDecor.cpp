/*
 *
 * Authors:
 *		s40in
 */





#include "HakiNSDecor.h"

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



//     #pragma mark - HakiNSDecorAddOn


HakiNSDecorAddOn::HakiNSDecorAddOn(image_id id, const char* name)
	:
	DecorAddOn(id, name)
{
}


Decorator*
HakiNSDecorAddOn::_AllocateDecorator(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
{
	return new (std::nothrow)HakiNSDecor(settings, rect, desktop);
}


//	#pragma mark - HakiNSDecor


HakiNSDecor::HakiNSDecor(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
	:
	SATDecorator(settings, rect, desktop),
	fButtonHighColor((rgb_color) { 255, 255, 255, 255 }),
	fButtonLowColor((rgb_color) { 0, 0, 0, 255 }),
	
	fFrameHighColor((rgb_color) { 182, 182, 182, 255 }), // ala gray
	fFrameMidColor((rgb_color) { 216, 216, 216, 255 }),
	fFrameLowColor((rgb_color) { 97, 97, 97, 255 }),
	fFrameLowerColor((rgb_color) { 0, 0, 0, 255 }),
	
	fFrameHighColorInactive((rgb_color) { 255, 255, 255, 255 }), //gray
	fFrameMidColorInactive((rgb_color) { 170, 170, 170, 255 }),
	fFrameLowColorInactive((rgb_color) { 85, 85, 85, 255 }),

	
	fFocusTextColor((rgb_color) { 255, 255, 255, 255 }),
	fNonFocusTextColor((rgb_color) { 0, 0, 0, 255 })
{
	STRACE(("HakiNSDecor:\n"));
	STRACE(("\tFrame (%.1f,%.1f,%.1f,%.1f)\n",
		rect.left, rect.top, rect.right, rect.bottom));

}


HakiNSDecor::~HakiNSDecor()
{
	STRACE(("HakiNSDecor: ~HakiNSDecor()\n"));

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
HakiNSDecor::_DrawBevelRect(const BRect rect, bool down,
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
HakiNSDecor::_DrawFrame(BRect invalid)
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
			_DrawBevelRect(r, false, fFrameLowerColor, fFrameLowerColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColor, fFrameMidColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColor, fFrameMidColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColor,  fFrameHighColorInactive);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, true, fFrameHighColor, fFrameHighColor);
		  }
		  else
		  {
			// first rect
			_DrawBevelRect(r, false, fFrameLowerColor, fFrameLowerColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColor, fFrameMidColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColor, fFrameMidColor);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, false, fFrameMidColor,  fFrameHighColorInactive);
			r.InsetBy(1, 1);
			_DrawBevelRect(r, true, fFrameHighColor, fFrameHighColor); 		
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
			_DrawBevelRect(r, false, fFrameHighColorInactive, fFrameLowColorInactive);
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
			{
				if (!invalid.Intersects(r))
					break;
					
				r.right -= 4;	
				r.bottom -= 4;
				
				if (IsFocus(fTopTab)) {
					fDrawingEngine->FillRect(r, fFrameLowerColor);
					_DrawBevelRect(r, true, fFrameHighColor, fFrameLowColor);
					r.InsetBy(1, 1);
					_DrawBevelRect(r, false, fFrameHighColor, fFrameLowColor);
		
				}
				else {
					fDrawingEngine->FillRect(r, fFrameMidColor);
					_DrawBevelRect(r, true, fFrameHighColor, fFrameLowColor);
					r.InsetBy(1, 1);
					_DrawBevelRect(r, false, fFrameHighColor, fFrameLowColor);
					
				}


				/*
				float x = r.right - 2;
				float y = r.bottom - 2;

				BRect bg(x - 13, y - 13, x, y);


				fDrawingEngine->FillRect(bg, fTabBMidColor);
				*/


				if (fTopTab && !IsFocus(fTopTab))
					break;
				break;
			}

			case B_TITLED_WINDOW_LOOK:
			case B_FLOATING_WINDOW_LOOK:
			case B_MODAL_WINDOW_LOOK:
			case kLeftTitledWindowLook:
			{
				/*
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
				*/	
				break;
			}

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
HakiNSDecor::_DrawTab(Decorator::Tab* tab, BRect invalid)
{
	STRACE(("_DrawTab(%.1f, %.1f, %.1f, %.1f)\n",
			invalid.left, invalid.top, invalid.right, invalid.bottom));
	const BRect& tabRect = tab->tabRect;
	// If a window has a tab, this will draw it and any buttons which are
	// in it.
	if (!tabRect.IsValid() || !invalid.Intersects(tabRect))
		return;
		
	BRect rect(tab->tabRect);
	BRect rectMyClose(tab->tabRect);
	BRect rectMyZoom(tab->tabRect);
	
	const BRect& closeRect = tab->closeRect;
	const BRect& zoomRect = tab->zoomRect;
		
	//rect.bottom = rect.bottom - (rect.bottom-rect.top)/2;

	if (IsFocus(tab)) {
		fDrawingEngine->FillRect(rect, fFrameLowerColor);
		
		rect.InsetBy(1, 1);
				
				
		if (closeRect.IsValid()){
			rectMyClose.left += 1;
			rectMyClose.top += 1;
			rectMyClose.bottom -= 1;
			rectMyClose.right = rectMyClose.left + ( rectMyClose.bottom - rectMyClose.top );
			
			_DrawBevelRect(rectMyClose, false, fFrameHighColor, fFrameLowColor);
			
			rect.left = rectMyClose.right +2;
		}
		
		if (zoomRect.IsValid()){
			rectMyZoom.right -= 1;
			rectMyZoom.top += 1;
			rectMyZoom.bottom -= 1;
			rectMyZoom.left = rectMyZoom.right - (rectMyZoom.bottom - rectMyZoom.top);
			
		
			_DrawBevelRect(rectMyZoom, false, fFrameHighColor, fFrameLowColor);
		
			rect.right = rectMyZoom.left - 2;		
		}
		
		_DrawBevelRect(rect, false, fFrameHighColor, fFrameLowColor);
		
		
		
	}
	else {
		fDrawingEngine->StrokeRect(rect, fFrameLowerColor);
		
		rect.InsetBy(1, 1);
		fDrawingEngine->FillRect(rect, fFrameMidColorInactive);
				
				
		if (closeRect.IsValid()){
			rectMyClose.left += 1;
			rectMyClose.top += 1;
			rectMyClose.bottom -= 1;
			rectMyClose.right = rectMyClose.left + ( rectMyClose.bottom - rectMyClose.top );
			
			_DrawBevelRect(rectMyClose, false, fFrameHighColorInactive, fFrameLowColorInactive);
			
			rect.left = rectMyClose.right +2;
		}
		
		if (zoomRect.IsValid()){
			rectMyZoom.right -= 1;
			rectMyZoom.top += 1;
			rectMyZoom.bottom -= 1;
			rectMyZoom.left = rectMyZoom.right - (rectMyZoom.bottom - rectMyZoom.top);
			
			_DrawBevelRect(rectMyZoom, false, fFrameHighColorInactive, fFrameLowColorInactive);
			
			rect.right = rectMyZoom.left - 2;		
		}
		
		_DrawBevelRect(rect, false, fFrameHighColorInactive, fFrameLowColorInactive);

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
HakiNSDecor::_DrawTitle(Decorator::Tab* _tab, BRect r)
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
HakiNSDecor::_DrawClose(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawClose(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));
		
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);
	
	rect.bottom -= 3;
	rect.right -= 3;
	
	rect.InsetBy(2, 2);
	
  if (IsFocus(fTopTab)) {		
	
	if (tab->closePressed) {
		fDrawingEngine->SetHighColor(fFrameLowColor);
	}
	else {
		fDrawingEngine->SetHighColor(fFocusTextColor);
	}
  }
  else {
  	fDrawingEngine->SetHighColor(fNonFocusTextColor);

  }
  
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


/*!	\brief Actually draws the zoom button

	Unless a subclass has a particularly large button, it is probably
	unnecessary to check the update rectangle.

	\param _tab The \a tab to update.
	\param direct Draw without double buffering.
	\param rect The area of the button to update.
*/
void
HakiNSDecor::_DrawZoom(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawZoom(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));

	if (rect.IntegerWidth() < 1)
		return;
		
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);	
		
	rect.left += 3;
	rect.bottom -= 3;
	
	rect.InsetBy(2, 2);
	
  if (IsFocus(fTopTab)) {		
	
	if (tab->zoomPressed) {
		fDrawingEngine->SetHighColor(fFrameLowColor);
	}
	else {
		fDrawingEngine->SetHighColor(fFocusTextColor);
	}
  }
  else {
  	fDrawingEngine->SetHighColor(fNonFocusTextColor);

  }
  
		fDrawingEngine->StrokeRect(rect);
		rect.InsetBy(1, 1);
		rect.bottom = rect.top + (rect.bottom-rect.top)/4;
		fDrawingEngine->FillRect(rect);

 	
}


void
HakiNSDecor::_DrawMinimize(Decorator::Tab* tab, bool direct, BRect rect)
{
	// This decorator doesn't have this button
}


void
HakiNSDecor::_GetButtonSizeAndOffset(const BRect& tabRect, float* _offset,
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
	return new (std::nothrow)HakiNSDecorAddOn(id, name);
}
