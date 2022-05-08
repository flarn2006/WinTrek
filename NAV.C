/***************************************
 *		 STAR TREK CLASSIC	 				
 *								  					
 *   Navigation MDI Window processing	
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:52  $
 * Log File:	$Logfile:   C:/src/st/vcs/nav.c_v  $
 * Log:			$Log:   C:/src/st/vcs/nav.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:52   jaworski
 * Initial Revision
 *								  					
 ****************************************/

#define	NAV_C
#include "exclude.h"

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include "joes.h"
#include	<stdlib.h>
#include	<string.h>

#include "st.h"			 							 /* Global include files */	
#include "resource.h"
#include "protos.h"


#define  XYWARP     16,16			 				 /* bitmap x, y positions */
#define  XYIMPULSE  16,91
#define  XYEXECUTE  16,166
#define  XYCOURS    150,6
#define  XCENTER    240	 					    	 /* Course needle coordinates */
#define  YCENTER    84	 				 
#define  XYWARPDIG  69,28			 				 /* digits placement */
#define  XYIMPDIG   69,103
#define  XYCOURSDIG 220,166

#define  SIZ_WARP    117,61		 				 /* bitmap sizes */	
#define  SIZ_IMPULSE 117,61


#define  RC_WARPUP   41,29,56,45			 		/* button placement rects */
#define  RC_WARPDN   94,29,108,45
#define  RC_IMPUP    41,104,56,120
#define  RC_IMPDN    94,104,108,120
#define  RC_COURSUP  195,167,211,183
#define  RC_COURSDN  267,167,283,183

#define  FID_WARPUP   0						 		/* fake button IDs */
#define  FID_WARPDN   1
#define  FID_IMPUP    2
#define  FID_IMPDN    3
#define  FID_COURSUP  4
#define  FID_COURSDN  5
#define  FID_EXECUTE  6
#define	DELAY_WI		 300						  /* Delay values for autorepeat */
#define	DELAY_CO		 80

static RECT		rcb[7];								 /* fake button rects		  	*/
static RECT		rcCircle;							 /* Course guage mouse hits	*/
static int		nCourseNum, nWarpNum, nImpNum; /* Curent display values  	*/
static HBITMAP	hImpulseBmp, hWarpBmp;	  	    /* the rest are shared in st.h*/

			

/*************************************************************************/
LRESULT CALLBACK _export NAVDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Navigation MDI definition procedure. 
                                                           
/**************************************************************************/
{
	DWORD			dwStyle;
	RECT			rc;
	static int  nHilite;
	int			i, nNewCourseNum, nInc;
	HDC			hDC;
	char			szBuf[8];
	POINT			pt;


	hNAVWnd = hWnd;
	switch(msg)
   	{
		case WM_CREATE:
			dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CAPTION |
				WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZE | WS_MINIMIZEBOX;
			SetWindowLong(hWnd, GWL_STYLE, (LPARAM)dwStyle);

			hImpulseBmp	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_IMPULSE));
			hWarpBmp	   = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_WARP));
			SetRect(&rcb[FID_WARPUP], 	RC_WARPUP);   
			SetRect(&rcb[FID_WARPDN], 	RC_WARPDN);   
			SetRect(&rcb[FID_IMPUP], 	RC_IMPUP);    
			SetRect(&rcb[FID_IMPDN], 	RC_IMPDN);    
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

			nCourseNum = nWarpNum = nImpNum = 0;
			nHilite = -1; 										/* nothing hilited, yet */
			break;


		case WM_ICONERASEBKGND:
			GetClientRect(hWnd, &rc);
			FillRect((HDC)wParam, &rc, GetStockObject(BLACK_BRUSH));
			return (LRESULT)0;


		case WM_PAINT:
			PaintNAV(hWnd);
			break;


		case WM_GETMINMAXINFO:
			(((LPPOINT)(lParam))+3)->x = (((LPPOINT)(lParam))+4)->x =
				SIZ_CTLX  + GetSystemMetrics(SM_CXFRAME);
         (((LPPOINT)(lParam))+3)->y = (((LPPOINT)(lParam))+4)->y =
				SIZ_CTLY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);


		case WM_DESTROY:
			DeleteBitmap(hImpulseBmp);					/* free (local) bitmaps   */
			DeleteBitmap(hWarpBmp);
			break;


/* Handle keyboard equivalents */

		case WM_KEYDOWN:					  		  /* handle keypad course changes */
			nNewCourseNum = NavKeyDown((WORD)wParam);
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
				if((nHilite == FID_EXECUTE) && !bSRSBusy && (nImpNum || nWarpNum)
					&&	PtInRect(&rcb[nHilite], MAKEPOINT(lParam)))
					{
					PlaySound(SND_EXEC, 0);
					if(nINIAutoSRS)
						Activate(hSRSWnd);			/* handle activation options */
					else
						Activate(hWnd);
					SendMessage(hSRSWnd, WM_NAVEXEC, (WPARAM)nCourseNum,
						MAKELPARAM(nImpNum, nWarpNum));
					}
				nHilite = -1;
				}
			break;



/* Handle fake button pressing */


	  	case WM_LBUTTONDOWN:
			if(nHilite >= 0 || bSRSBusy) 						 /* already hiliting */
				break;

			pt = MAKEPOINT(lParam);
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

			i = 0;
			while(!PtInRect(&rcb[i], pt))
				{
				if(++i > FID_EXECUTE)
					return (LPARAM)0;							 /* exit if not found */
				}

			if(fNAVRepair && (i == FID_WARPUP || i == FID_WARPDN))
				break;
				
			nHilite = i;			  	     
			hDC = GetDC(hWnd);
			InvertRect(hDC, &rcb[i]);
			switch(nHilite)
				{
				case FID_WARPUP:			 					/* Increase Warp Value */
				   if(fNAVRepair)
						break;
					if(nWarpNum > 8)
						break;
					if(nImpNum)
						{
						nImpNum = 0;
						DrawNumber(hDC, itoa(nImpNum, szBuf, 10), XYIMPDIG,
							hDigitsGBmp);
						}
					do
						{
						DrawNumber(hDC, itoa(++nWarpNum, szBuf, 10),
							XYWARPDIG, hDigitsGBmp);
						if(nWarpNum > 8)
							break;
						} while(ButtonStillDown(DELAY_WI));

	 				break;


				case FID_WARPDN:				  				/* Decrease Warp Value */
				   if(fNAVRepair)
						break;
					if(nWarpNum < 1)
						break;
					do
						{
						DrawNumber(hDC, itoa(--nWarpNum, szBuf, 10),
							XYWARPDIG, hDigitsGBmp);
						if(nWarpNum < 1)
							break;
						} while(ButtonStillDown(DELAY_WI));

					break;


				case FID_IMPUP:
					if(nImpNum > 8)			  			/* Increase Impulse Value */
						break;
					if(nWarpNum && !fNAVRepair)
						{
						nWarpNum = 0;
						DrawNumber(hDC, itoa(nWarpNum, szBuf, 10), XYWARPDIG,
							hDigitsGBmp);
						}

					do
						{
						DrawNumber(hDC, itoa(++nImpNum, szBuf, 10), XYIMPDIG,
							hDigitsGBmp);
						if(nImpNum > 8)
							break;
						} while(ButtonStillDown(DELAY_WI));
					break;


				case FID_IMPDN:					 		/* Decrease Impulse Value */
					if(nImpNum < 1)
						break;
					do
						{
						DrawNumber(hDC, itoa(--nImpNum, szBuf, 10), XYIMPDIG,
							hDigitsGBmp);
						if(nImpNum < 1)
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



		default:
         	return(DefMDIChildProc(hWnd, msg, wParam, lParam));
         	break;
     	}

	return (LRESULT)0;

} /* End NAVDefProc */







/***************************************************************************/

void LOCAL PaintNAV(HWND hWnd)

//	This function draws the navigation display. Numeric updates are done
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


	SetRect(&rc, XYWARP, SIZ_WARP);				 		  	  /* Draw Warp panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		{
		if(!fNAVRepair)
			{
			DrawBitmap(hDC, XYWARP, hWarpBmp, SRCCOPY);
			DrawNumber(hDC, itoa(nWarpNum, szBuf, 10), XYWARPDIG, hDigitsGBmp);
			}
		else
			FillRect(hDC, &rc, GetStockObject(BLACK_BRUSH));
		}
		

	SetRect(&rc, XYIMPULSE, SIZ_IMPULSE);		 		  /* Draw Impulse panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		{
		DrawBitmap(hDC, XYIMPULSE, hImpulseBmp, SRCCOPY);
		DrawNumber(hDC, itoa(nImpNum, szBuf, 10), XYIMPDIG, hDigitsGBmp);
		}

	SetRect(&rc, XYCOURS, SIZ_COURS);		 		    /* Draw Course panel */
	rc.right  += rc.left;
	rc.bottom += rc.top;
	if(IntersectRect(&rcDummy, &ps.rcPaint, &rc))
		{
		DrawBitmap(hDC, XYCOURS, hCourseBmp, SRCCOPY);
		DrawNumber(hDC, CourseToAscii(szBuf, nCourseNum), XYCOURSDIG,
			hDigitsGBmp);
		DrawCourseNeedle(hDC, XCENTER, YCENTER, -1, nCourseNum);
		}

	EndPaint(hWnd, &ps);


} /* End PaintNAV */





/***************************************************************************/

int LOCAL NavKeyDown(WORD wKey)

//	This function decodes keypresses in the NAV window. The return value is
// the new course number. If -1,	a non-course command was processed.

/***************************************************************************/
{
	int	nDeg;

	if(bSRSBusy)												  /* ignore all if busy */
		return -1;

	switch(wKey)
		{
		case VK_NUMPAD1:
		case VK_END:
		case '1':	nDeg = 225; break;

		case VK_NUMPAD2:
		case VK_DOWN:
		case '2':	nDeg = 180; break;

		case VK_NUMPAD3:
		case VK_NEXT:
		case '3':	nDeg = 135; break;

		case VK_NUMPAD4:
		case VK_LEFT:
		case '4':	nDeg = 270; break;

		case VK_NUMPAD5:
		case '5':	nDeg = 0;   break;

		case VK_NUMPAD6:
		case VK_RIGHT:
		case '6':	nDeg = 90;  break;

		case VK_NUMPAD7:
		case VK_HOME:
		case '7':	nDeg = 315; break;

		case VK_NUMPAD8:
		case VK_UP:
		case '8':	nDeg = 0;   break;

		case VK_NUMPAD9:
		case VK_PRIOR:
		case '9':   nDeg = 45;  break;

		case VK_ADD:								/* Increase Course Number */
		case 0xbb:	nDeg = nCourseNum + 1; break;

		case VK_SUBTRACT:							/* Decrease Course Number */
		case 0xbd:	nDeg = nCourseNum - 1; break;


		case VK_RETURN:
			if(nImpNum || nWarpNum)
				{
				PlaySound(SND_EXEC, 0);
				if(nINIAutoSRS)
					Activate(hSRSWnd);		 /* handle activation options */
				else
					Activate(hNAVWnd);
				SendMessage(hSRSWnd, WM_NAVEXEC, (WPARAM)nCourseNum,
					MAKELPARAM(nImpNum, nWarpNum));
				}
			nDeg = -1;
			break;


		default:
			nDeg = -1;
			break;
		}

	return nDeg;

} /* End NavKeyDown */
