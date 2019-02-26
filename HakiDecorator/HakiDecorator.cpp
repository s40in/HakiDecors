/*
 *
 * Authors:
 *		s40in
 */


/*! Decorator base on Haiku's BeDecorator */


#include "HakiDecorator.h"

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



//     #pragma mark - HakiDecorAddOn


HakiDecorAddOn::HakiDecorAddOn(image_id id, const char* name)
	:
	DecorAddOn(id, name)
{
}


Decorator*
HakiDecorAddOn::_AllocateDecorator(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
{
	return new (std::nothrow)HakiDecorator(settings, rect, desktop);
}


//	#pragma mark - HakiDecorator


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
HakiDecorator::HakiDecorator(DesktopSettings& settings, BRect rect,
	Desktop* desktop)
	:
	SATDecorator(settings, rect, desktop),
	fButtonHighColor((rgb_color) { 248, 252, 248, 255 }),
	fButtonLowColor((rgb_color) { 72, 80, 128, 255 }),
	
	fFrameHighColor((rgb_color) { 248, 252, 248, 255 }),
	fFrameMidColor((rgb_color) { 216, 216, 216, 255 }),
	fFrameLowColor((rgb_color) { 128, 128, 128, 255 }),
	fFrameLowerColor((rgb_color) { 0, 0, 0, 255 }),
	
	fFrameHighColorInactive((rgb_color) { 224, 228, 224, 255 }),
	fFrameMidColorInactive((rgb_color) { 216, 216, 216, 255 }),
	fFrameLowColorInactive((rgb_color) { 184, 188, 184, 255 }),
	fFrameLowerColorInactive((rgb_color) { 152, 152, 152, 255 }),
	
	fTabYHighColor((rgb_color) { 248, 248, 72, 255 }),
	fTabYMidColor((rgb_color) { 248, 224, 24, 255 }),
	fTabYLowColor((rgb_color) { 184, 132, 0, 255 }),
	
	fTabYHighColorInactive((rgb_color) { 224, 224, 168, 255 }),
	fTabYMidColorInactive((rgb_color) { 224, 216, 160, 255 }),
	fTabYLowColorInactive((rgb_color) { 208, 188, 152, 255 }),
	
	fTabBHighColor((rgb_color) { 184, 188, 240, 255 }),
	fTabBMidColor((rgb_color) { 120, 132, 192, 255 }),
	fTabBLowColor((rgb_color) { 88, 92, 136, 255 }),
	
	fTabBHighColorInactive((rgb_color) { 200, 208, 224, 255 }),
	fTabBMidColorInactive((rgb_color) { 184, 188, 208, 255 }),
	fTabBLowColorInactive((rgb_color) { 176, 180, 192, 255 }),
	
	fFocusTextColor((rgb_color) { 0, 0, 0, 255 }),
	fNonFocusTextColor((rgb_color) { 152, 156, 152, 255 })
{
	STRACE(("HakiDecorator:\n"));
	STRACE(("\tFrame (%.1f,%.1f,%.1f,%.1f)\n",
		rect.left, rect.top, rect.right, rect.bottom));

}


HakiDecorator::~HakiDecorator()
{
	STRACE(("HakiDecorator: ~HakiDecorator()\n"));

}



// #pragma mark - Protected methods
void
HakiDecorator::_DrawFrame(BRect invalid)
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
	BRect rb = BRect(fTopBorder.LeftTop(), fBottomBorder.RightBottom());

	
	switch ((int)fTopTab->look) {
		case B_TITLED_WINDOW_LOOK:
		case B_DOCUMENT_WINDOW_LOOK:
		case B_MODAL_WINDOW_LOOK:
		case B_FLOATING_WINDOW_LOOK:
		case kLeftTitledWindowLook:
		{
		  if (IsFocus(fTopTab)) {
			// first rect
			//fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameLowerColor); 		//left
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameLowerColor);		//top
			//fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameLowerColor);	//right
			//fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameLowerColor);	//bottom
			fDrawingEngine->StrokeRect(r, fFrameLowerColor);
			
			r.InsetBy(1, 1);
			
			fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameHighColor);
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameHighColor);
			fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameLowColor);
			fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameLowColor);
			
			r.InsetBy(1, 1);
			
			fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameMidColor);
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameMidColor);
			fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameMidColor);
			fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameMidColor);
			
			r.InsetBy(1, 1);
			
			fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameMidColor);
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameMidColor);
			fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameHighColor);
			fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameHighColor);
			
			r.InsetBy(1, 1);
			
			fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameLowColor);
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameLowColor);
			fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameLowColor);
			fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameLowColor);
			
			//blue
			rb.InsetBy(1, 1);
			fDrawingEngine->StrokeLine(rb.LeftTop(), rb.RightTop(), fTabBHighColor);
			rb.top += 1;
			fDrawingEngine->StrokeLine(rb.LeftTop(), rb.RightTop(), fTabBMidColor);
			rb.top += 1;
			fDrawingEngine->StrokeLine(rb.LeftTop(), rb.RightTop(), fTabBMidColor);
			rb.top += 1;
			fDrawingEngine->StrokeLine(rb.LeftTop(), rb.RightTop(), fTabBLowColor);	
		  }
		  else
		  {
		  	// first rect
			//fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameLowerColorInactive); 		//left
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameLowerColorInactive);		//top
			///fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameLowerColorInactive);	//right
			//fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameLowerColorInactive);	//bottom
			fDrawingEngine->StrokeRect(r, fFrameLowerColorInactive);
			
			r.InsetBy(1, 1);
			
			fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameHighColorInactive);
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameHighColor);
			fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameLowColorInactive);
			fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameLowColorInactive);
			
			r.InsetBy(1, 1);
			
			fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameMidColorInactive);
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameMidColor);
			fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameMidColorInactive);
			fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameMidColorInactive);
			
			r.InsetBy(1, 1);
			
			fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameMidColorInactive);
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameMidColor);
			fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameHighColorInactive);
			fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameHighColorInactive);
			
			r.InsetBy(1, 1);
			
			fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameLowColorInactive);
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameLowColor);
			fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameLowColorInactive);
			fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameLowColorInactive);
			
			//blue
			rb.InsetBy(1, 1);
			fDrawingEngine->StrokeLine(rb.LeftTop(), rb.RightTop(), fTabBHighColorInactive);
			rb.top += 1;
			fDrawingEngine->StrokeLine(rb.LeftTop(), rb.RightTop(), fTabBMidColorInactive);
			rb.top += 1;
			fDrawingEngine->StrokeLine(rb.LeftTop(), rb.RightTop(), fTabBMidColorInactive);
			rb.top += 1;
			fDrawingEngine->StrokeLine(rb.LeftTop(), rb.RightTop(), fTabBLowColorInactive);	
		  	
		  }		
			break;
		}

		/*case B_FLOATING_WINDOW_LOOK:
		case kLeftTitledWindowLook:
		{
			//fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameLowerColor); 		//left
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameLowerColor);		//top
			//fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameLowerColor);	//right
			//fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameLowerColor);	//bottom
			fDrawingEngine->StrokeRect(r, fTabBLowColor);
			
			r.InsetBy(1, 1);
			
			fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameHighColor);
			fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameHighColor);
			fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameLowColor);
			fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameLowColor);

			//fDrawingEngine->StrokeRect(r, fFrameLowColor);
			break;
		}
		*/

		case B_BORDERED_WINDOW_LOOK:
		{
			// TODO: Draw the borders individually!

			fDrawingEngine->StrokeRect(r, fFrameLowerColor);
			//r.InsetBy(1, 1);
			
			//fDrawingEngine->StrokeLine(r.LeftBottom(), r.LeftTop(), fFrameHighColor);
			//fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(), fFrameHighColor);
			//fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(), fFrameLowColor);
			//fDrawingEngine->StrokeLine(r.RightBottom(), r.LeftBottom(), fFrameLowColor);
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
					
				r.right -= 2;	
				r.bottom -= 2;
				
				if (IsFocus(fTopTab)) {
					fDrawingEngine->FillRect(r, fTabBMidColor);
				
					fDrawingEngine->StrokeLine(r.LeftTop(), r.LeftBottom(),
						fFrameLowColor);
					fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(),
						fFrameLowColor);
					
					r.InsetBy(1, 1);
					
					fDrawingEngine->StrokeLine(r.LeftTop(), r.LeftBottom(),
						fTabBHighColor);
					fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(),
						fTabBHighColor);
				}
				else {
					fDrawingEngine->FillRect(r, fTabBMidColorInactive);
				
					fDrawingEngine->StrokeLine(r.LeftTop(), r.LeftBottom(),
						fFrameLowColorInactive);
					fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(),
						fFrameLowColorInactive);
					
					r.InsetBy(1, 1);
					
					fDrawingEngine->StrokeLine(r.LeftTop(), r.LeftBottom(),
						fTabBHighColorInactive);
					fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(),
						fTabBHighColorInactive);
					
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
HakiDecorator::_DrawTab(Decorator::Tab* tab, BRect invalid)
{
	STRACE(("_DrawTab(%.1f, %.1f, %.1f, %.1f)\n",
			invalid.left, invalid.top, invalid.right, invalid.bottom));
	const BRect& tabRect = tab->tabRect;
	// If a window has a tab, this will draw it and any buttons which are
	// in it.
	if (!tabRect.IsValid() || !invalid.Intersects(tabRect))
		return;

if (IsFocus(tab)) {
	BGradientLinear gradientY;
	gradientY.SetStart(tabRect.LeftTop());
	gradientY.SetEnd(tabRect.LeftBottom());
	gradientY.AddColor((rgb_color) { 248, 236, 40, 255 }, 0);
	gradientY.AddColor((rgb_color) { 248, 224, 32, 255 }, 128);
	gradientY.AddColor((rgb_color) { 208, 172, 24, 255 }, 255);
	
	
	fDrawingEngine->FillRect(BRect(tabRect.left , tabRect.top,
								   tabRect.right, tabRect.bottom),
								   gradientY);

	// outer frame
	fDrawingEngine->StrokeLine(tabRect.LeftTop(), tabRect.LeftBottom(),
		fFrameLowerColor);
	fDrawingEngine->StrokeLine(tabRect.LeftTop(), tabRect.RightTop(),
		fFrameLowerColor);
	fDrawingEngine->StrokeLine(tabRect.RightTop(), tabRect.RightBottom(),
		fFrameLowerColor);
	fDrawingEngine->StrokeLine(tabRect.LeftBottom(), tabRect.RightBottom(),
		fFrameLowerColor);


	// bevel
	fDrawingEngine->StrokeLine(BPoint(tabRect.left + 1, tabRect.top + 1),
							   BPoint(tabRect.left + 1, tabRect.bottom-1),
							   fTabYHighColor);
		
	fDrawingEngine->StrokeLine(BPoint(tabRect.left + 1, tabRect.top + 1),
							   BPoint(tabRect.right -1, tabRect.top + 1),
							   fTabYHighColor);

	fDrawingEngine->StrokeLine(BPoint(tabRect.right - 1, tabRect.top + 2),
						       BPoint(tabRect.right - 1, tabRect.bottom-1),
							   fTabYLowColor);
	
	fDrawingEngine->StrokeLine(BPoint(tabRect.left + 1, tabRect.bottom-1),
						       BPoint(tabRect.right - 1,tabRect.bottom-1),
							   fTabYLowColor);

}
else
{
	BGradientLinear gradientYinactive;
	gradientYinactive.SetStart(tabRect.LeftTop());
	gradientYinactive.SetEnd(tabRect.LeftBottom());
	gradientYinactive.AddColor((rgb_color) { 224, 220, 160, 255 }, 0);
	gradientYinactive.AddColor((rgb_color) { 224, 212, 152, 255 }, 128);
	gradientYinactive.AddColor((rgb_color) { 208, 200, 160, 255 }, 255);
	
	
	fDrawingEngine->FillRect(BRect(tabRect.left , tabRect.top,
								   tabRect.right, tabRect.bottom),
								   gradientYinactive);

	// outer frame
	fDrawingEngine->StrokeLine(tabRect.LeftTop(), tabRect.LeftBottom(),
		fFrameLowerColorInactive);
	fDrawingEngine->StrokeLine(tabRect.LeftTop(), tabRect.RightTop(),
		fFrameLowerColorInactive);
	fDrawingEngine->StrokeLine(tabRect.RightTop(), tabRect.RightBottom(),
		fFrameLowerColorInactive);
	fDrawingEngine->StrokeLine(tabRect.LeftBottom(), tabRect.RightBottom(),
		fFrameLowerColorInactive);


	// bevel
	fDrawingEngine->StrokeLine(BPoint(tabRect.left + 1, tabRect.top + 1),
							   BPoint(tabRect.left + 1, tabRect.bottom-1),
							   fTabYHighColorInactive);
		
	fDrawingEngine->StrokeLine(BPoint(tabRect.left + 1, tabRect.top + 1),
							   BPoint(tabRect.right -1, tabRect.top + 1),
							   fTabYHighColorInactive);

	fDrawingEngine->StrokeLine(BPoint(tabRect.right - 1, tabRect.top + 2),
						       BPoint(tabRect.right - 1, tabRect.bottom-1),
							   fTabYLowColorInactive);
	
	fDrawingEngine->StrokeLine(BPoint(tabRect.left + 1, tabRect.bottom-1),
						       BPoint(tabRect.right - 1,tabRect.bottom-1),
							   fTabYLowColorInactive);

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
HakiDecorator::_DrawTitle(Decorator::Tab* _tab, BRect r)
{
	STRACE(("_DrawTitle(%f, %f, %f, %f)\n", r.left, r.top, r.right, r.bottom));

	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);

	const BRect& tabRect = tab->tabRect;
	const BRect& closeRect = tab->closeRect;
	const BRect& zoomRect = tab->zoomRect;


	fDrawingEngine->SetDrawingMode(B_OP_OVER);
	if (IsFocus(fTopTab)) {
		fDrawingEngine->SetHighColor(fFocusTextColor);
		fDrawingEngine->SetLowColor(fTabBLowColor);
	}
	else {
		fDrawingEngine->SetHighColor(fNonFocusTextColor);
		fDrawingEngine->SetLowColor(fTabBLowColorInactive);
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
HakiDecorator::_DrawClose(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawClose(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));
		
	rect.InsetBy(2, 2);
		
  if (IsFocus(fTopTab)) {		
			
	fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYLowColor);
	fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYLowColor);
	fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYHighColor);	
	fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYHighColor);	
	
	rect.InsetBy(1, 1);
		
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);
	
	if (!(tab->closePressed)) {
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYHighColor);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYHighColor);
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYLowColor);	
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYLowColor);
	}
	else {
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYLowColor);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYLowColor);
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYHighColor);	
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYHighColor);
	}
  }
  else {
	
	fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYLowColorInactive);
	fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYLowColorInactive);
	fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYHighColorInactive);	
	fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYHighColorInactive);	
	
	rect.InsetBy(1, 1);
		
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);
	
	if (!(tab->closePressed)) {
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYHighColorInactive);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYHighColorInactive);
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYLowColorInactive);	
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYLowColorInactive);
	}
	else {
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYLowColorInactive);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYLowColorInactive);
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYHighColorInactive);	
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYHighColorInactive);
	}	
  	
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
HakiDecorator::_DrawZoom(Decorator::Tab* _tab, bool direct, BRect rect)
{
	STRACE(("_DrawZoom(%f,%f,%f,%f)\n", rect.left, rect.top, rect.right,
		rect.bottom));

	if (rect.IntegerWidth() < 1)
		return;
		
	rect.InsetBy(2, 2);
	rect.bottom = rect.bottom - (rect.bottom-rect.top)/2;
	
  if (IsFocus(fTopTab)) {						
	fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYLowColor);
	fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYLowColor);
	fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYHighColor);	
	fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYHighColor);
			
	rect.InsetBy(1, 1);
		
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);
	
	if (!(tab->zoomPressed)) {
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYHighColor);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYHighColor);
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYLowColor);	
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYLowColor);
	}
	else {
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYLowColor);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYLowColor);
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYHighColor);	
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYHighColor);
	}
  }
  else {
	fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYLowColorInactive);
	fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYLowColorInactive);
	fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYHighColorInactive);	
	fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYHighColorInactive);
			
	rect.InsetBy(1, 1);
		
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);
	
	if (!(tab->zoomPressed)) {
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYHighColorInactive);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYHighColorInactive);
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYLowColorInactive);	
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYLowColorInactive);
	}
	else {
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(),
			fTabYLowColorInactive);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(),
			fTabYLowColorInactive);
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(),
			fTabYHighColorInactive);	
		fDrawingEngine->StrokeLine(rect.LeftBottom(), rect.RightBottom(),
			fTabYHighColorInactive);
	}
  }
  	
}


void
HakiDecorator::_DrawMinimize(Decorator::Tab* tab, bool direct, BRect rect)
{
	// This decorator doesn't have this button
}


void
HakiDecorator::_GetButtonSizeAndOffset(const BRect& tabRect, float* _offset,
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
	return new (std::nothrow)HakiDecorAddOn(id, name);
}
