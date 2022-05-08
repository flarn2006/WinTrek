/************************************
 *		 STAR TREK CLASSIC	 			
 *								  				
 * Photon MDI Window processing
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:48  $
 * Log File:	$Logfile:   C:/src/st/vcs/pho.c_v  $
 * Log:			$Log:   C:/src/st/vcs/pho.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:48   jaworski
 * Initial Revision
 *
 ************************************/

#define	PHO_C
#include "exclude.h"

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include "joes.h"
#include	<stdlib.h>
#include	<string.h>

#include "st.h"			 								/* Global include files		*/	
#include "resource.h"
#include "protos.h"

#define  XYDIST     16,46			 					/* bitmap x, y positions 	*/
#define  XYEXECUTE  16,166
#define  XYCOURS    150,6
#define  XCENTER    240	 					    	 /* Course needle coordinates */
#define  YCENTER    84	 				 
#define  XYDISTDIG  69,58			 				 /* digits placement   			*/
#define  XYCOURSDIG 220,166

#define  SIZ_DIST    117,61		 				/* bitmap sizes		  	  */	


#define  RC_DISTUP   41,59,56,75			 		/* button placement rects */
#define  RC_DISTDN   94,59,108,75
#define  RC_COURSUP  195,167,211,183
#define  RC_COURSDN  267,167,283,183

#define  FID_DISTUP   0						 		/* fake button IDs		  */
#define  FID_DISTDN   1
#define  FID_COURSUP  2
#define  FID_COURSDN  3
#define  FID_EXECUTE  4
#define	DELAY_WI		 300						  /* Delay values for autorepeat */
#define	DELAY_CO		 80

static RECT		rcb[5];								 /* fake button rects		  	*/
static RECT		rcCircle;							 /* Course guage mouse hits	*/
static int		nCourseNum, nDistNum; 			 /* Curent display values  	*/
static HBITMAP	hDistanceBmp;	  	    			 /* the rest are shared in st.h*/

			




/*************************************************************************/
LRESULT CALLBACK _export PHODefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Photon Torpedo MDI definition procedure. 
                                                           
/**************************************************************************/
{
	DWORD			dwStyle;
	RECT			rc;
	static int  nHilite;
	int			i, nNewCourseNum, nInc;
	HDC			hDC;
	char			szBuf[8];
	POINT			pt;

	hPHOWnd = hWnd;

	switch(msg)
   	{
		case WM_CREATE:
			dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CAPTION |
				WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZE | WS_MINIMIZEBOX;
			SetWindowLong(hWnd, GWL_STYLE, (LPARAM)dwStyle);

			hDistanceBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_DIST));
			SetRect(&rcb[FID_DISTUP], 	RC_DISTUP);   
			SetRect(&rcb[FID_DISTDN], 	RC_DISTDN);   
			SetRect(&rcb[FID_COURSUP], RC_COURSUP);  
			SetRect(&rcb[FID_COURSDN], RC_COURSDN);
			SetRect(&rcb[FID_EXECUTE], XYEXECUTE, SIZ_EXEC);
			rcb[FID_EXECUTE].right  += rcb[FID_EXECUTE].left;
			rcb[FID_EXECUTE].bottom += rcb[FID_EXECUTE].top;
			InflateRect(&rcb[FID_EXECUTE], -1, -1);

			rcCircle.left  = XCENTER - 75;		  /* set course circle detect */
			rcCircle.right = XCENTER + 75;
			rcCircle.top   = YCENTER - 75;
			rcCircle.bottom= YCENTER + 75;

			nCourseNum = nDistNum = 0;
			nHilite = -1; 										/* nothing hilited, yet */
			break;


		case WM_PAINT:
			PaintPHO(hWnd);
			break;


		case WM_GETMINMAXINFO:
			(((LPPOINT)(lParam))+3)->x = (((LPPOINT)(lParam))+4)->x =
				SIZ_CTLX  + GetSystemMetrics(SM_CXFRAME);
         (((LPPOINT)(lParam))+3)->y = (((LPPOINT)(lParam))+4)->y =
				SIZ_CTLY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);


		case WM_DESTROY:
			DeleteBitmap(hDistanceBmp);					/* free (local) bitmap  */
			break;


		case WM_KEYDOWN:					  		  /* handle keypad course changes */
			if(fPHORepair || bSRSBusy)
				break;
			switch(wParam)
				{
				case VK_NUMPAD1:
				case VK_END:
				case '1':	nNewCourseNum = 225; break;

				case VK_NUMPAD2:
				case VK_DOWN:
				case '2':	nNewCourseNum = 180; break;

				case VK_NUMPAD3:
				case VK_NEXT:
				case '3':	nNewCourseNum = 135; break;

				case VK_NUMPAD4:
				case VK_LEFT:
				case '4':	nNewCourseNum = 270; break;

				case VK_NUMPAD5:
				case '5':	nNewCourseNum = 0;   break;

				case VK_NUMPAD6:
				case VK_RIGHT:
				case '6':	nNewCourseNum = 90;  break;

				case VK_NUMPAD7:
				case VK_HOME:
				case '7':	nNewCourseNum = 315; break;

				case VK_NUMPAD8:
				case VK_UP:
				case '8':	nNewCourseNum = 0;   break;

				case VK_NUMPAD9:
				case VK_PRIOR:
				case '9': nNewCourseNum = 45;  break;

				case VK_ADD:								/* Increase Course Number */
				case 0xbb:
					nNewCourseNum = nCourseNum +1;
	 				break;

				case VK_SUBTRACT:							/* Decrease Course Number */
				case 0xbd:
					nNewCourseNum = nCourseNum -1;
	 				break;

				case VK_RETURN:
					if(nINIAutoSRS)
						Activate(hSRSWnd);			/* handle activation options */
					else
				 	Activate(hWnd);
				 	PlaySound(SND_EXEC, 0);
				 	SendMessage(hSRSWnd, WM_PHOEXEC, (WPARAM)nCourseNum,
				 		MAKELPARAM(nDistNum, 0));
					return (LRESULT)0;
					
						
					
				default:	 nNewCourseNum = -1;	 break;
				}

			if(nNewCourseNum >= 0)
				{
				hDC = GetDC(hWnd);
				DrawNumber(hDC, CourseToAscii(szBuf, nNewCourseNum),
					XYCOURSDIG, hDigitsGBmp);
				DrawCourseNeedle(hDC, XCENTER, YCENTER, nCourseNum, nNewCourseNum);
				nCourseNum = nNewCourseNum;
				}
			break;



/* Handle fake button releasing */

	  	case WM_LBUTTONUP:
			if(nHilite >= 0)
				{
				hDC = GetDC(hWnd);
				InvertRect(hDC, &rcb[nHilite]);
				ReleaseDC(hWnd, hDC);
				if((nHilite == FID_EXECUTE) && !bSRSBusy &&
					PtInRect(&rcb[nHilite], MAKEPOINT(lParam))) /* Execute Rect? */
					{
					if(nINIAutoSRS)
						Activate(hSRSWnd);			/* handle activation options */
					else
						Activate(hWnd);
					if(!fPHORepair)
						{
						PlaySound(SND_EXEC, 0);
						PostMessage(hSRSWnd, WM_PHOEXEC, (WPARAM)nCourseNum,
							MAKELPARAM(nDistNum, 0));
						}
					}
				nHilite = -1;
				}
			break;



/* Handle fake button pressing */


	  	case WM_LBUTTONDOWN:
			if(nHilite >= 0 || bSRSBusy) 						 /* already hiliting */
				break;

			pt = MAKEPOINT(lParam);
			if(!fPHORepair)
				{
				if(PtInRect(&rcCircle, pt))						   /* Circle hit? */
					{
					pt.x -= XCENTER;
					pt.y -= YCENTER;
					hDC = GetDC(hWnd);
					nNewCourseNum = CoordsToDeg(pt);
					DrawNumber(hDC, CourseToAscii(szBuf, nNewCourseNum),
							XYCOURSDIG, hDigitsGBmp);
					DrawCourseNeedle(hDC, XCENTER, YCENTER, nCourseNum, nNewCourseNum);
					nCourseNum = nNewCourseNum;
					ReleaseDC(hWnd, hDC);
					break;
					}
				}

			i = 0;
			while(!PtInRect(&rcb[i], pt))
				{
				if(++i > FID_EXECUTE)
					return (LPARAM)0;							 /* exit if not found */
				}

			if(fPHORepair)
				{								/* if broke, support only Execute btn */
				if(i == FID_EXECUTE)
					{
					nHilite = i;			  	     
					hDC = GetDC(hWnd);
					InvertRect(hDC, &rcb[i]);
					ReleaseDC(hWnd, hDC);
					}
				else
					break;
				}

			nHilite = i;			  	     
			hDC = GetDC(hWnd);
			InvertRect(hDC, &rcb[i]);
			switch(nHilite)
				{
				case FID_DISTUP:			 					/* Increase Dist Value */
					if(nDistNum > 8)
						break;
					do
						{
						DrawNumber(hDC, itoa(++nDistNum, szBuf, 10),
							XYDISTDIG, hDigitsGBmp);
						if(nDistNum > 8)
							break;
						} while(ButtonStillDown(DELAY_WI));

	 				break;


				case FID_DISTDN:				  				/* Decrease Dist Value */
					if(nDistNum < 1)
						break;
					do
						{
						DrawNumber(hDC, itoa(--nDistNum, szBuf, 10),
							XYDISTDIG, hDigitsGBmp);
						if(nDistNum < 1)
							break;
						} while(ButtonStillDown(DELAY_WI));

					break;


				case FID_COURSUP:							/* Increase Course Number */
					if(GetKeyState(VK_SHIFT) & 0x8000)
						nInc = 1;				 			/* set for SHIFT key down */
					else
						nInc = 5;
					 					
					if(nCourseNum >= 360-nInc)
						nNewCourseNum = 0;
					else
						nNewCourseNum = nCourseNum + nInc;

					do
						{
						DrawNumber(hDC, CourseToAscii(szBuf, nNewCourseNum),
							XYCOURSDIG, hDigitsGBmp);
						DrawCourseNeedle(hDC, XCENTER, YCENTER, nCourseNum, nNewCourseNum);
						nCourseNum = nNewCourseNum;
						if(nCourseNum >= 360-nInc)
							nNewCourseNum = 0;
						else
							nNewCourseNum = nCourseNum + nInc;
						} while(ButtonStillDown(DELAY_CO));
	 				break;


				case FID_COURSDN:							/* Decrease Course Number */
					if(GetKeyState(VK_SHIFT) & 0x8000)
						nInc = 1;				 			/* set for SHIFT key down */
					else
						nInc = 5;

					if(nCourseNum == 0)
						nNewCourseNum = 360 - nInc;
					else if(nCourseNum <= nInc)
						nNewCourseNum = 0;
					else
						nNewCourseNum = nCourseNum - nInc;

					do
						{
						DrawNumber(hDC, CourseToAscii(szBuf, nNewCourseNum),
							XYCOURSDIG, hDigitsGBmp);
						DrawCourseNeedle(hDC, XCENTER,YCENTER, nCourseNum, nNewCourseNum);
						nCourseNum = nNewCourseNum;
					if(nCourseNum == 0)
						nNewCourseNum = 360 - nInc;
					else if(nCourseNum <= nInc)
						nNewCourseNum = 0;
					else
						nNewCourseNum = nCourseNum - nInc;
						} while(ButtonStillDown(DELAY_CO));
	 				break;


				default :
					break;

				} /* End switch(nHilite) */

			ReleaseDC(hWnd, hDC);
			break;



		case WM_ICONERASEBKGND:
			GetClientRect(hWnd, &rc);
			FillRect((HDC)wParam, &rc, GetStockObject(BLACK_BRUSH));
			break;


     	default:
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);
        	break;
     	}

	return (LRESULT)0;

} /* End PHODefProc */





/***************************************************************************/

void LOCAL PaintPHO(HWND hWnd)

//	This function draws the photon display. Numeric updates are done
// in IDN_xxxx message procs.

/***************************************************************************/
{
	HDC			hDC;
	PAINTSTRUCT	ps;
	char			szBuf[8];
	RECT			rc, rcDummy;

	hDC = BeginPaint(hWnd, &ps);


	SetRect(&rc, XYEXECUTE, SIZ_EXEC);				 	  /* Draw Execute panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		DrawBitmap(hDC, XYEXECUTE, hExecuteBmp, SRCCOPY);

	SetRect(&rc, XYDIST, SIZ_DIST);				 		 /* Draw Distance panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		{
		DrawBitmap(hDC, XYDIST, hDistanceBmp, SRCCOPY);
		if(!fPHORepair)
			DrawNumber(hDC, itoa(nDistNum, szBuf, 10), XYDISTDIG, hDigitsGBmp);
		}

	SetRect(&rc, XYCOURS, SIZ_COURS);		 		    /* Draw Course panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		{
		DrawBitmap(hDC, XYCOURS, hCourseBmp, SRCCOPY);
		if(!fPHORepair)
			{
			DrawNumber(hDC, CourseToAscii(szBuf, nCourseNum), XYCOURSDIG,
				hDigitsGBmp);
			DrawCourseNeedle(hDC, XCENTER, YCENTER, -1, nCourseNum);
			}

		}

	EndPaint(hWnd, &ps);


} /* End PaintPHO */
