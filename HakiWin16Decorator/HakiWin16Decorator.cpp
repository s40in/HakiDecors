/*
 *
 * Authors:
 *		s40in
 */





#include "HakiWin16Decorator.h"

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



//     #pragma mark - HakiWin16DecoratorAddOn


HakiWin16DecoratorAddOn::HakiWin16DecoratorAddOn(image_id id, const char* name)
	:
	DecorAddOn(id, name)
{
}


Decorator*
HakiWin16DecoratorAddOn::_AllocateDecorator(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
{
	return new (std::nothrow)HakiWin16Decorator(settings, rect, desktop);
}


//	#pragma mark - HakiWin16Decorator


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
HakiWin16Decorator::HakiWin16Decorator(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
	:
	SATDecorator(settings, rect, desktop),
	fButtonHighColor((rgb_color) { 255, 255, 255, 255 }),
	fButtonLowColor((rgb_color) { 128, 136, 143, 255 }),
	
	fFrameHighColor((rgb_color) { 192, 199, 207, 255 }),
	fFrameMidColor((rgb_color) { 0, 0, 170, 255 }), //Title backgraund
	fFrameLowColor((rgb_color){ 128, 136, 143, 255 }),
	fFrameLowerColor((rgb_color) { 0, 0, 0, 255 }),

	
	fFrameMidColorInactive((rgb_color) { 192, 199, 207, 255 }),

	
	fFocusTextColor((rgb_color) { 255, 255, 255, 255 }),
	fNonFocusTextColor((rgb_color) { 0, 0, 0, 255 })
{
	STRACE(("HakiWin16Decorator:\n"));
	STRACE(("\tFrame (%.1f,%.1f,%.1f,%.1f)\n",
		rect.left, rect.top, rect.right, rect.bottom));

}


HakiWin16Decorator::~HakiWin16Decorator()
{
	STRACE(("HakiWin16Decorator: ~HakiWin16Decorator()\n"));

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
HakiWin16Decorator::_DrawBevelRect(const BRect rect, bool down,
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
HakiWin16Decorator::_DrawFrame(BRect invalid)
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
			fDrawingEngine->StrokeRect(r, fFrameLowerColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameHighColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameHighColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameHighColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameLowerColor);
		  }
		  else
		  {
			fDrawingEngine->StrokeRect(r, fFrameLowerColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameHighColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameHighColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameHighColor);
			r.InsetBy(1, 1);
			fDrawingEngine->StrokeRect(r, fFrameLowerColor);		  		
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
			_DrawBevelRect(r, false, fFrameLowerColor, fFrameLowerColor);
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
					
					_DrawBevelRect(r, true, fFrameLowerColor, fFrameLowerColor);
					r.InsetBy(1, 1);
					fDrawingEngine->FillRect(r, fFrameLowColor);
				}
				else {
					_DrawBevelRect(r, true, fFrameLowerColor, fFrameLowerColor);
					r.InsetBy(1, 1);
					fDrawingEngine->FillRect(r, fFrameLowColor);
					
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
HakiWin16Decorator::_DrawTab(Decorator::Tab* tab, BRect invalid)
{
	STRACE(("_DrawTab(%.1f, %.1f, %.1f, %.1f)\n",
			invalid.left, invalid.top, invalid.right, invalid.bottom));
	const BRect& tabRect = tab->tabRect;
	// If a window has a tab, this will draw it and any buttons which are
	// in it.
	if (!tabRect.IsValid() || !invalid.Intersects(tabRect))
		return;
		
	BRect rect(tab->tabRect);
	BRect rectL(tab->tabRect);
	BRect rectR(tab->tabRect);
	
	const BRect& closeRect = tab->closeRect;
	const BRect& zoomRect = tab->zoomRect;
		
	//rect.bottom = rect.bottom - (rect.bottom-rect.top)/2;

	//draw tab for all win (active & inactive)
		fDrawingEngine->StrokeRect(rect, fFrameLowerColor);
		rect.InsetBy(1,1);
		fDrawingEngine->FillRect(rect, fFrameHighColor);

		rect.top += 4;
		rect.left += 4;
		rect.right -= 4;
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameLowerColor); //left
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameLowerColor); 	//top
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameLowerColor);	// right
		
		/*
		rect.top += 1;
		rect.left += 1;
		rect.right -= 1;
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.LeftTop(), fFrameHighColor); //left
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameHighColor); 	//top
		fDrawingEngine->StrokeLine(rect.RightTop(), rect.RightBottom(), fFrameLowColor);	// right	
		*/

		rectR = rect;  // for zoom button
		rectL = rect; // for close button
		
		
		float bw = (rect.bottom-rect.top); //buttom width
		
		
		
		if (closeRect.IsValid()){
			rectL.left += bw;
			rectL.right = rectL.left+1;
			fDrawingEngine->StrokeLine(rectL.LeftBottom(), rectL.LeftTop(), fFrameLowerColor); //left
			rect.left = rectL.right;
		}
		
				
		if (zoomRect.IsValid()){
			rectR.left = rectR.right - bw-1; //(rectR.bottom-rectR.top) ;
			rectR.right = rectR.left + 1; 
			fDrawingEngine->StrokeLine(rectR.LeftBottom(), rectR.LeftTop(), fFrameLowerColor); //left
			
			rect.right = rectR.left-1;
		}
		else {
			rect.right -=1;
		}
		
		rect.top +=1;
		
		
	if (IsFocus(tab)) {	 // active window
		fDrawingEngine->FillRect(rect, fFrameMidColor); // fill color tab (rect - buttons wight)
	}
	else {				// inactive window
		fDrawingEngine->FillRect(rect, fFrameHighColor); // fill color tab (rect - buttons wight)
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
HakiWin16Decorator::_DrawTitle(Decorator::Tab* _tab, BRect r)
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
		fDrawingEngine->SetLowColor(fFrameLowColor);
	}
	fDrawingEngine->SetFont(fDrawState.Font());

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
HakiWin16Decorator::_DrawClose(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawClose(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));
		
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);
	
	rect.left += 2;
	rect.top += 3;
	
	rect.InsetBy(3, 3);
	
	rect.top = rect.top + ((rect.bottom-rect.top)/2)-2;
	rect.bottom = rect.top +4;
	
	if (!(tab->closePressed)){
		fDrawingEngine->FillRect(rect, fButtonLowColor); // shadow
	}
	else {
		fDrawingEngine->FillRect(rect, fFrameHighColor);
	}
	
	rect.left -=1;
	rect.top -= 1;
	rect.right -= 1;
	rect.bottom -= 1;
	
	fDrawingEngine->FillRect(rect, fButtonHighColor);
	fDrawingEngine->StrokeRect(rect, fFrameLowerColor);
	
	

}


/*!	\brief Actually draws the zoom button

	Unless a subclass has a particularly large button, it is probably
	unnecessary to check the update rectangle.

	\param _tab The \a tab to update.
	\param direct Draw without double buffering.
	\param rect The area of the button to update.
*/
void
HakiWin16Decorator::_DrawZoom(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawZoom(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));

	if (rect.IntegerWidth() < 1)
		return;
		
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);	
		
	rect.right -= 1;
	rect.top += 1;
	rect.bottom +=1;
	rect.left -=1;
		
	
	_DrawBevelRect(rect, tab->zoomPressed, fButtonHighColor, fButtonLowColor);
	rect.InsetBy(1,1);
	_DrawBevelRect(rect, tab->zoomPressed, fFrameHighColor, fButtonLowColor);
	rect.InsetBy(1,1);
	fDrawingEngine->StrokeRect(rect, fFrameHighColor);
	
	rect.InsetBy(3,3);
	
	BRect triRect;
	triRect = rect;
	
	triRect.bottom = triRect.top + (triRect.bottom-triRect.top)/2-1;
	
	for (int i=0; i<4; i++){
		fDrawingEngine->StrokeLine(triRect.LeftBottom(), triRect.RightBottom(), fFrameLowerColor); 
		triRect.bottom -=1;
		triRect.left +=1;
		triRect.right -=1;
	}
	
	triRect = rect;
	triRect.bottom = triRect.top + (triRect.bottom-triRect.top)/2+1;
	
	for (int i=0; i<4; i++){
		fDrawingEngine->StrokeLine(triRect.LeftBottom(), triRect.RightBottom(), fFrameLowerColor); 
		triRect.bottom +=1;
		triRect.left +=1;
		triRect.right -=1;
	}
  	
}


void
HakiWin16Decorator::_DrawMinimize(Decorator::Tab* tab, bool direct, BRect rect)
{
	// This decorator doesn't have this button
}


void
HakiWin16Decorator::_GetButtonSizeAndOffset(const BRect& tabRect, float* _offset,
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
	return new (std::nothrow)HakiWin16DecoratorAddOn(id, name);
}
