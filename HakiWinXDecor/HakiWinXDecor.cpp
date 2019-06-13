/*
 *
 * Authors:
 *		s40in
 */





#include "HakiWinXDecor.h"

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



//     #pragma mark - HakiWinXDecorAddOn


HakiWinXDecorAddOn::HakiWinXDecorAddOn(image_id id, const char* name)
	:
	DecorAddOn(id, name)
{
}


Decorator*
HakiWinXDecorAddOn::_AllocateDecorator(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
{
	return new (std::nothrow)HakiWinXDecor(settings, rect, desktop);
}


//	#pragma mark - HakiWinXDecor


// TODO: get rid of DesktopSettings here, and introduce private accessor
//	methods to the Decorator base class
/*
			rgb_color			fFrameHighColor;
			rgb_color			fFrameMidColor;
			rgb_color			fFrameLowColor;
			rgb_color			fFrameLowerColor;
			
			rgb_color			fFrameHighColorInactive;
			rgb_color			fFrameMidColorInactive;
			rgb_color			fFrameLowColorInactive;
			
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
*/
HakiWinXDecor::HakiWinXDecor(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
	:
	SATDecorator(settings, rect, desktop),
	fButtonCloseFocusColor((rgb_color) { 232, 17, 35, 255 }),
	fButtonClosePressedColor((rgb_color) { 232, 17, 35, 255 }),
	fButtonClosePressedLineColor((rgb_color) { 255, 255, 255, 255 }),
	
	fButtonFocusColor((rgb_color) { 92, 172, 172, 255 }),
	fButtonPressedColor((rgb_color) { 172, 172, 172, 255 }),
	
	fFrameHighColor((rgb_color) { 0, 120, 215, 255 }), // blue
	//fFrameHighColor((rgb_color) { 255, 203, 0, 255 }), // yellow
	
	fFrameMidColor((rgb_color) { 216, 216, 216, 255 }), //Title backgraund
	fFrameLowColor((rgb_color){ 0, 120, 215, 255 }),
	fFrameLowerColor((rgb_color) { 0, 0, 0, 255 }),

	
	fFrameHighColorInactive((rgb_color) { 172, 172, 172, 255 }),
	
	fFrameMidColorInactive((rgb_color) { 216, 216, 216, 255 }),

	
	fFocusTextColor((rgb_color) { 0, 0, 0, 255 }),
	fNonFocusTextColor((rgb_color) { 172, 172, 172, 255 })
{
	STRACE(("HakiWinXDecor:\n"));
	STRACE(("\tFrame (%.1f,%.1f,%.1f,%.1f)\n",
		rect.left, rect.top, rect.right, rect.bottom));
		
	fPlainFont=*gFontManager->DefaultPlainFont();
	

}


HakiWinXDecor::~HakiWinXDecor()
{
	STRACE(("HakiWinXDecor: ~HakiWinXDecor()\n"));

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
HakiWinXDecor::_DrawBevelRect(const BRect rect, bool down,
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
HakiWinXDecor::_DrawFrame(BRect invalid)
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
			fDrawingEngine->StrokeRect(r, fFrameHighColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameMidColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameMidColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameMidColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameMidColor);
			r.InsetBy(1,1);
		  }
		  else
		  {
			fDrawingEngine->StrokeRect(r, fFrameHighColorInactive);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameMidColorInactive);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameMidColorInactive);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameMidColorInactive);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameMidColorInactive);	
			r.InsetBy(1,1);	  		
		  }		
			break;
		}

	/*	case B_FLOATING_WINDOW_LOOK:
		case kLeftTitledWindowLook:
		{
			_DrawBevelRect(r, false, fFrameHighColorInactive, fFrameLowColorInactive);
			break;
		}
		*/

		case B_BORDERED_WINDOW_LOOK:
		{
			_DrawBevelRect(r, false, fFrameHighColor, fFrameHighColor);
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
					fDrawingEngine->StrokeRect(r, fFrameHighColor);
					r.InsetBy(1,1);
					fDrawingEngine->FillRect(r, fFrameMidColor);
				}
				else {
					fDrawingEngine->StrokeRect(r, fFrameHighColorInactive);
					r.InsetBy(1,1);
					fDrawingEngine->FillRect(r, fFrameMidColor);
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
HakiWinXDecor::_DrawTab(Decorator::Tab* tab, BRect invalid)
{
	STRACE(("_DrawTab(%.1f, %.1f, %.1f, %.1f)\n",
			invalid.left, invalid.top, invalid.right, invalid.bottom));
	const BRect& tabRect = tab->tabRect;
	// If a window has a tab, this will draw it and any buttons which are
	// in it.
	if (!tabRect.IsValid() || !invalid.Intersects(tabRect))
		return;
		
	BRect rect(tab->tabRect);
	//BRect rectL(tab->tabRect);
	//BRect rectR(tab->tabRect);
	
	//const BRect& closeRect = tab->closeRect;
	//const BRect& zoomRect = tab->zoomRect;
		
	if (IsFocus(tab)) {	 // active window
		fDrawingEngine->FillRect(rect, fFrameMidColor); // fill color tab (rect - buttons wight)
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameHighColor); //left
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameHighColor); 	//top
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameHighColor);	// right
	}
	else {				// inactive window
		fDrawingEngine->FillRect(rect, fFrameMidColorInactive); // fill color tab (rect - buttons wight)
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameHighColorInactive); //left
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameHighColorInactive); 	//top
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameHighColorInactive);	// right		
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
HakiWinXDecor::_DrawTitle(Decorator::Tab* _tab, BRect r)
{
	STRACE(("_DrawTitle(%f, %f, %f, %f)\n", r.left, r.top, r.right, r.bottom));

	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);

	const BRect& tabRect = tab->tabRect;
	const BRect& closeRect = tab->closeRect;
	const BRect& zoomRect = tab->zoomRect;


	fDrawingEngine->SetDrawingMode(B_OP_OVER);
	if (IsFocus(fTopTab)) {
		fDrawingEngine->SetHighColor(fFocusTextColor);
		fDrawingEngine->SetLowColor(fFrameMidColor);
	}
	else {
		fDrawingEngine->SetHighColor(fNonFocusTextColor);
		fDrawingEngine->SetLowColor(fFrameMidColorInactive);
	}
	//fDrawingEngine->SetFont(fDrawState.Font());
	fDrawingEngine->SetFont(fPlainFont);


	// figure out position of text
	font_height fontHeight;
	fDrawState.Font().GetHeight(fontHeight);

	BPoint titlePos;
	if (fTopTab->look != kLeftTitledWindowLook) {
		titlePos.x = closeRect.IsValid() ? closeRect.right + tab->textOffset
			: tabRect.left + tab->textOffset;
		titlePos.y = floorf(((tabRect.top + 4.0) + tabRect.bottom
			+ fontHeight.ascent + fontHeight.descent) / 2.0
			- fontHeight.descent + 0.5);
	} else {
		titlePos.x = floorf(((tabRect.left + 4.0) + tabRect.right
			+ fontHeight.ascent + fontHeight.descent) / 2.0
			- fontHeight.descent + 0.5);
		titlePos.y = zoomRect.IsValid() ? zoomRect.top - tab->textOffset
			: tabRect.bottom - tab->textOffset;
	}

	//fDrawingEngine->SetFont(fDrawState.Font());
	
	fDrawingEngine->SetFont(fPlainFont);


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
HakiWinXDecor::_DrawClose(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawClose(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));
		
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);
	
//	rect.InsetBy(3, 3);
	
	if (tab->closePressed){
		fDrawingEngine->FillRect(rect, fButtonClosePressedColor);
	}
	else {
		fDrawingEngine->FillRect(rect, fFrameMidColor);
	}
	
	
	rect.InsetBy(4,4);
	if (tab->buttonFocus) {
		if (tab->closePressed){
			fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightBottom(), fButtonClosePressedLineColor); 
			fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightTop(), fButtonClosePressedLineColor);
		}
		else {
			fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightBottom(), fFocusTextColor); 
			fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightTop(), fFocusTextColor);
		}
	}
	else {
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightBottom(), fNonFocusTextColor); 
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightTop(), fNonFocusTextColor);
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
HakiWinXDecor::_DrawZoom(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawZoom(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));

	if (rect.IntegerWidth() < 1)
		return;
		
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);	
		
	//	rect.InsetBy(3, 3);
	
	if (tab->zoomPressed){
		fDrawingEngine->FillRect(rect, fButtonPressedColor);
	}
	else {
		fDrawingEngine->FillRect(rect, fFrameMidColor);
	}
	
	rect.InsetBy(4,4);
	
	if (tab->buttonFocus) {
		fDrawingEngine->StrokeRect(rect, fFocusTextColor);
	}
	else {
		fDrawingEngine->StrokeRect(rect, fNonFocusTextColor);	
	}
	
	
  	
}


void
HakiWinXDecor::_DrawMinimize(Decorator::Tab* tab, bool direct, BRect rect)
{
	// This decorator doesn't have this button
}


void
HakiWinXDecor::_GetButtonSizeAndOffset(const BRect& tabRect, float* _offset,
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
	return new (std::nothrow)HakiWinXDecorAddOn(id, name);
}
