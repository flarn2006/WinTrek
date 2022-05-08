/*********************************************
 *	  			 STAR TREK CLASSIC
 *
 *Short Range Sensor MDI Window processing
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:52  $
 * Log File:	$Logfile:   C:/src/st/vcs/srs.c_v  $
 * Log:			$Log:   C:/src/st/vcs/srs.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:52   jaworski
 * Initial Revision
 *
 *********************************************/


#define	SRS_C
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


#define	EXPLSEQ 8						 		 /* explode sequence frame count */
#define	XBM  (34+2)												/* image block sizes */
#define  YBM  (25+2)
#define  XOFF (SIZ_SRSX-(XBM*8))			 		/* offsets from window corner	*/
#define  YOFF (SIZ_SRSY-(YBM*8))						

static HBITMAP	hEntLBmp, hEntRBmp, hBaseBmp, hStar0Bmp, hStar1Bmp,  
		 			hStar2Bmp, hKlingonBmp, hRomulanBmp, hFarengBmp, hBorgBmp,
		 			hPhoBmp;

static BOOL	bAbortProc; 							  /* Movememnt abort flag 		*/
static int 	nMovement;	 							  /* impulse or warp movement */
static int 	xres, yres;								  /* used by Stars() routines */
static BOOL bInWarp;								     /* If set, no WM_PAINTs		*/	
static int nOldX = -10, nOldY = -10;			  /* photon erase flag			*/


/*************************************************************************/
LRESULT CALLBACK _export SRSDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

// This is the Short Range Sensor MDI definition procedure. 
                                                           
/**************************************************************************/
{
	static LOGFONT	lf = {-(YOFF-6), 0, 0, 0, 400, 0, 0, 0, 0, 3, 2, 1, 34, "Arial"};
	static HFONT	hFont, hOldFont;
	static FARPROC lpfnAnimProc;			  	 /* Callback for animation timer */
	RECT		rc;
	HDC		hDC;
	DWORD		dwStyle;


	hSRSWnd = hWnd;
	switch(msg)
   	{
		case WM_CREATE:
			dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CAPTION |
				WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZE | WS_MINIMIZEBOX;
			SetWindowLong(hWnd, GWL_STYLE, (LPARAM)dwStyle);

			hEntLBmp	 	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_ENTL));
			hEntRBmp	 	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_ENTR));
			hBaseBmp	 	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_BASE));
			hStar0Bmp	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_STAR0));
			hStar1Bmp	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_STAR1));
			hStar2Bmp	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_STAR2));
			hKlingonBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_KLINGON));
			hRomulanBmp = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_ROMULAN));
			hFarengBmp	= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_FARENG));
			hBorgBmp		= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_BORG));
			hPhoBmp		= LoadBitmap(hLib, MAKEINTRESOURCE(BMP_PHOTON));

			hDC = GetDC(hWnd);
			hFont = CreateFontIndirect(&lf);
			hOldFont = SelectFont(hDC, hFont);
	      SetBkColor(hDC, RGB_BLACK);


			ReleaseDC(hWnd, hDC);
			lpfnAnimProc = MakeProcInstance((FARPROC)AnimProc, hInst);
			SetTimer(hWnd, 3, 200, (TIMERPROC)lpfnAnimProc);
			break;


		case WM_ICONERASEBKGND:
			GetClientRect(hWnd, &rc);
			FillRect((HDC)wParam, &rc, GetStockObject(BLACK_BRUSH));
			break;


		case WM_PAINT:
			PaintSRS(hWnd);
			break;


		case WM_GETMINMAXINFO:
			(((LPPOINT)(lParam))+3)->x = (((LPPOINT)(lParam))+4)->x =
				SIZ_SRSX  + GetSystemMetrics(SM_CXFRAME);
         (((LPPOINT)(lParam))+3)->y = (((LPPOINT)(lParam))+4)->y =
				SIZ_SRSY + GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYFRAME);
        	return DefMDIChildProc(hWnd, msg, wParam, lParam);


		case WM_DESTROY:
			KillTimer(hWnd, 3);						/* Destroy anim timer */
			FreeProcInstance(lpfnAnimProc);

			hDC = GetDC(hWnd);
			SelectFont(hDC, hOldFont);
			ReleaseDC(hWnd, hDC);
			DeleteFont(hFont);

			DeleteBitmap(hEntLBmp);					/* free all bitmaps   */
			DeleteBitmap(hEntRBmp);
			DeleteBitmap(hBaseBmp);
			DeleteBitmap(hStar0Bmp);
			DeleteBitmap(hStar1Bmp);
			DeleteBitmap(hStar2Bmp);
			DeleteBitmap(hKlingonBmp);
			DeleteBitmap(hRomulanBmp);
			DeleteBitmap(hFarengBmp);
			DeleteBitmap(hBorgBmp);
			DeleteBitmap(hPhoBmp);
			break;


/* Process  WM_NAVEXEC messages				*/
/* wParam holds the course in degrees,		*/
/* the loword of lParam	holds the impulse,*/
/* the hiword holds the warp factor.		*/

		case WM_NAVEXEC:
			if(bSRSBusy)
				break;
			UpdateWindow(hWnd);
			bSRSBusy = TRUE;
			WaitCursorX(hFrameWnd);
			if(LOWORD(lParam))
				MoveEntImpulse(wParam, LOWORD(lParam));
			else if(!fNAVRepair)
				MoveEntWarp(wParam, HIWORD(lParam));
			bSRSBusy = FALSE;
			++bNeedSave;
			ArrowCursorX();
			break;


/* Process  WM_PHAEXEC messages				*/
/* wParam holds the Phaser Power level		*/
/* the loword of lParam	is TRUE for Single*/

		case WM_PHAEXEC:
			if(bSRSBusy)
				break;
			UpdateWindow(hWnd);
			bSRSBusy = TRUE;
			WaitCursorX(hFrameWnd);
			if(wParam)
				FirePhasers(wParam, (BOOL)LOWORD(lParam));
			bSRSBusy = FALSE;
			++bNeedSave;
			ArrowCursorX();
			break;


/* Process  WM_PHOEXEC messages				*/
/* wParam holds the course in degrees,		*/
/* the loword of lParam	holds the distance*/

		case WM_PHOEXEC:
			if(bSRSBusy)
				break;
			UpdateWindow(hWnd);
			bSRSBusy = TRUE;
			WaitCursorX(hFrameWnd);
			if(!nPhotonCnt)
				ShowMessage(MG_PHOEMPTY);
			else
				{
				PlaySoundEx(SND_PHOT, 0);
				FirePhoton(wParam, LOWORD(lParam));
				}
			bSRSBusy = FALSE;
			++bNeedSave;
			ArrowCursorX();
			break;

/* 													*/
/* Process WM_RICEXEC (ricochet) messages */
/* 													*/

		case WM_RICEXEC:
			if(bSRSBusy)
				break;
			UpdateWindow(hWnd);
			bSRSBusy = TRUE;
			WaitCursorX(hFrameWnd);
			RicochetToQuad();
			bSRSBusy = FALSE;
			++bNeedSave;
			ArrowCursorX();
			break;


		default:
         	return(DefMDIChildProc(hWnd, msg, wParam, lParam));
         	break;
     	}

	return (LRESULT)0;

} /* End SRSDefProc */




/***************************************************************************/

void LOCAL PaintSRS(HWND hWnd)

//	This function draws the SRS screen.	It draws the grid and ref numbers,
// then each bitmap in its location.

/***************************************************************************/
{
	RECT			rc, rcTx;
	HDC			hDC, hMemDC;
	PAINTSTRUCT	ps;
	HPEN			hOldPen, hPen;
	int			i, j;
	char			sz[4];
	HBITMAP		hBM, hOldBM;


	hDC = BeginPaint(hWnd, &ps);


	if(bInWarp && !fSRSRepair)
		{
		EndPaint(hWnd, &ps);
		return;
		}

/* Draw the grid */

	GetClientRect(hWnd, &rc);
	hPen = CreatePen(PS_SOLID, 1, RGB_RED);
	hOldPen = SelectPen(hDC, hPen);

	MoveTo(hDC, XOFF, YOFF);									/* Draw basic frame  */	
	LineTo(hDC, rc.right, YOFF);
	MoveTo(hDC, XOFF, YOFF);
	LineTo(hDC, XOFF, rc.bottom);
						   
	for(i = 0; i < 8; i++)	
		{
		MoveTo(hDC, XOFF + (XBM * i), YOFF);		     /* Draw Horiz ticks */
		LineTo(hDC, XOFF + (XBM * i), YOFF + 4);
		MoveTo(hDC, XOFF + (XBM * i), rc.bottom);
		LineTo(hDC, XOFF + (XBM * i), rc.bottom - 4);
		MoveTo(hDC, XOFF,             YOFF + (YBM*i)); /* Draw Vertical ticks */
		LineTo(hDC, XOFF + 4,         YOFF + (YBM*i));
		MoveTo(hDC, rc.right,         YOFF + (YBM*i));
		LineTo(hDC, rc.right - 4,     YOFF + (YBM*i));
		}

	SelectPen(hDC, hOldPen);
	DeletePen(hPen);


	if(fSRSRepair || bInWarp)			 	/* its broke- don't draw anything */
		{
		EndPaint(hWnd, &ps);
		return;
		}


/* Draw the grid reference numbers */

	SetBkColor(hDC, RGB_BLACK);
	SetTextColor(hDC, RGB_RED);

	*sz = '1';
	for(i = 0; i < 8; i++)
		{
		SetRect(&rcTx, XOFF + (XBM * i), 0, 0, YOFF);  /* X coord numbers */
		rcTx.right = rcTx.left + XBM;
		InflateRect(&rcTx, -2, -2);
		DrawText(hDC, sz, 1, &rcTx, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
			DT_CENTER | DT_NOCLIP);

		SetRect(&rcTx, 0, YOFF + (YBM * i), XOFF, 0); /* Y coord numbers */
		rcTx.bottom = rcTx.top + YBM;
		InflateRect(&rcTx, -2, -2);
		DrawText(hDC, sz, 1, &rcTx, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
			DT_CENTER | DT_NOCLIP);
		++*sz;
		}


/* Draw all the bitmaps */

	hMemDC = CreateCompatibleDC(hDC);
	hOldBM = SelectBitmap(hMemDC, hEntLBmp);		   /* any bitymap will do */

	SetRect(&rcTx, 0, YOFF - YBM, 0, YOFF);
	for(i = 0; i < 8; i++)
		{
		rcTx.left    = XOFF - XBM;
		rcTx.right   = XOFF;
		rcTx.top    += YBM;	  								/* move to next row */
		rcTx.bottom += YBM;

		for(j = 0; j <	8; j++)
			{
			rcTx.left  += XBM;								/* move to next column */
			rcTx.right += XBM;

			if(hBM = ObjToBitmap(GetQObj(i, j)))
				{
				SelectBitmap(hMemDC, hBM);
				StretchBlt(hDC, rcTx.left+1, rcTx.top+1, XBM-2, YBM-2,
					hMemDC, 0, 0, XBM-2, YBM-2, SRCCOPY);
				}
			}
		}
	SelectBitmap(hMemDC, hOldBM);
	DeleteDC(hMemDC);

	EndPaint(hWnd, &ps);


} /* End PaintSRS */





/***************************************************************************/

WORD CALLBACK _export AnimProc(HWND hWnd, WORD wMsg, int TimID, DWORD dwTim)

//	This is the callback function to sequence the SRS graphics on the screen.

/***************************************************************************/
{
	HDC			hDC, hMemDC;
	RECT			rc;
	HBITMAP		hOldBM;
	int			i, j, nBMO;

	if(IsIconic(hSRSWnd) || fSRSRepair || bAbortProc || bPaused)
		return 0;							 					/* iconic, broke, abort */

	if(!qd[nEntQ].Bases && !qd[nEntQ].Stars)			 	/* no stars or bases */
		return 0;

	hDC = GetDC(hWnd);

	hMemDC = CreateCompatibleDC(hDC);
	hOldBM = SelectBitmap(hMemDC, hEntLBmp);		     /* any bitmap will do */

	SetRect(&rc, 0, YOFF - YBM, 0, YOFF);
	for(i = 0; i < 8; i++)
		{
		rc.left    = XOFF - XBM;
		rc.right   = XOFF;
		rc.top    += YBM;	  										 /* move to next row */
		rc.bottom += YBM;

		for(j = 0; j <	8; j++)
			{
			rc.left  += XBM;									 /* move to next column */
			rc.right += XBM;
			nBMO = 0;
			switch(GetQObj(i,j))
				{
				case OBJ_BASE:
					if(bSRSBusy)
						break;
					SelectBitmap(hMemDC, hBaseBmp);
					nBMO = (int)GetQInc(i,j);
					StretchBlt(hDC, rc.left+1, rc.top+1, XBM-2, YBM-2,
						hMemDC, 0, nBMO*(YBM-2), XBM-2, YBM-2, SRCCOPY);
					IncQObjInc(i, j);
					break;

				case OBJ_STAR0:
					SelectBitmap(hMemDC, hStar0Bmp);
					nBMO = (int)GetQInc(i,j);
					StretchBlt(hDC, rc.left+1, rc.top+1, XBM-2, YBM-2,
						hMemDC, 0, nBMO*(YBM-2), XBM-2, YBM-2, SRCCOPY);
					IncQObjInc(i, j);
					break;

				case OBJ_STAR1:
					SelectBitmap(hMemDC, hStar1Bmp);
					nBMO = (int)GetQInc(i,j);
					StretchBlt(hDC, rc.left+1, rc.top+1, XBM-2, YBM-2,
						hMemDC, 0, nBMO*(YBM-2), XBM-2, YBM-2, SRCCOPY);
					IncQObjInc(i, j);
					break;

				case OBJ_STAR2:
					SelectBitmap(hMemDC, hStar2Bmp);
					nBMO = (int)GetQInc(i,j);
					StretchBlt(hDC, rc.left+1, rc.top+1, XBM-2, YBM-2,
						hMemDC, 0, nBMO*(YBM-2), XBM-2, YBM-2, SRCCOPY);
					IncQObjInc(i, j);
					break;

				default:
					break;
				}

			if( (char)GetQInc(i,j) > (char)0x03)
				qd[nEntQ].Obj[i][j] &= 0x00ff;
			}
		}
	SelectBitmap(hMemDC, hOldBM);
	DeleteDC(hMemDC);
	ReleaseDC(hWnd, hDC);

	return 0;

} /* End AnimProc */



/***************************************************************************/

void LOCAL GetSectorRect(LPRECT lprc, int nX, int nY)

//	This function creates a rect in hSRSWnd local coordinates of the requested
//	X, Y coordinates. This rect can be used for drawing and erasing objects
//	directly in the SRS window.

/***************************************************************************/
{

	lprc->left  = XOFF + (nX * XBM);			/* calc absolute position */
	lprc->top   = YOFF + (nY * YBM);
	lprc->right = lprc->left + XBM;
	lprc->bottom= lprc->top  + YBM;
	InflateRect(lprc, -1, -1);						/* shrink to BM center	  */	
	OffsetRect(lprc, 1, 1);							/* offset to draw point	  */	


} /* End GetSectorRect */





/***************************************************************************/

void LOCAL MoveEntImpulse(int nDeg, int nFactor)

//	This function moves the enterprise within the current sector.

/***************************************************************************/
{
	POINT		ptEnd;
	FARPROC 	lpfnImpMovProc;
	HDC		hDC;


	ShowMessage(MG_IENGAGE); 						 /* Impulse engines ON msg */
	if(!SubtractEnergy(10))
		return;
	nMovement = nFactor;
	PlaySoundEx(SND_IMP, 1);


	ptEnd = DegToCoords(nDeg, 12);	 		 	 /* determine succesive points */
	ptEnd.x += nEntX;
	ptEnd.y += nEntY;

	if(ptEnd.x > nEntX)
		SetQObj(OBJ_ENTR, nEntY, nEntX);							/* face right dir */

	else 
		SetQObj(OBJ_ENTL, nEntY, nEntX);							/* face left dir */


	hDC = GetDC(hSRSWnd);
	bAbortProc = FALSE;
	lpfnImpMovProc = MakeProcInstance((FARPROC)ImpMovProc, hInst);
	LineDDA(nEntX, nEntY, ptEnd.x, ptEnd.y, (LINEDDAPROC)lpfnImpMovProc,
		(LPARAM)(HDC far *)&hDC);
	FreeProcInstance(lpfnImpMovProc);						/* call the callback	*/
	ReleaseDC(hSRSWnd, hDC);
	bAbortProc = FALSE;

	ShowMessage(MG_STOPPED, nEntX+1, nEntY+1);
	InvalidateRect(hPANWnd, NULL, 0);
	KillSound();
	UpdateLRS(nEntQ);				 								 	/* Mark LRS scan	*/
	SetCondition();

} /* End MoveEntImpulse */




/***************************************************************************/

void CALLBACK _export ImpMovProc(int nX, int nY, LPSTR lphDC)

//	This is the LineDDA() callback that handles impulse engine movement.
//	It redraws the enterprise at the new location and subtracts the required
// energy. If a block occurs, the callback quits and returns zero. If a
// beyond sector move occurs, the function calls the warp procedure and then
// quits. Messages are sent to the message window within this function.

/***************************************************************************/
{
	int		nQX, nQY;
	HBITMAP	hBM;
	RECT		rc;

	if(bAbortProc)
		return;

	if(nX > 7 || nX < 0 || nY > 7 || nY < 0)	 	  /* exceed sector limits? */
		{
		nQX = 0;										  /* create destination quadrant */
		nQY = 0;
		if(nX > 7)
			++nQX;
		else if(nX < 0)
			--nQX;
		if(nY > 7)
			++nQY;
		else if(nY < 0)
			--nQY;
		nMovement = 1;
		WarpToQuad(AdjQuad(nEntQ, nQX, nQY));
		fStarDNow -= 1.0;
		bAbortProc = TRUE;
		return;
		}


	switch(GetQObj(nY, nX))				 	  /* see if an object is in the way */
		{
		case OBJ_BASE:
			ShowMessage(MG_BLOCKSB, nX+1, nY+1);
			bAbortProc = TRUE;
			return;

		case OBJ_STAR0:
		case OBJ_STAR1:
		case OBJ_STAR2:
			ShowMessage(MG_BLOCKST, nX+1, nY+1);
			bAbortProc = TRUE;
			return;

		case OBJ_KLINGON:
			ShowMessage(MG_BLOCKKL, nX+1, nY+1);
			bAbortProc = TRUE;
			return;

		case OBJ_ROMULAN:
			ShowMessage(MG_BLOCKRO, nX+1, nY+1);
			bAbortProc = TRUE;
			return;

		case OBJ_FARENG:
			ShowMessage(MG_BLOCKFA, nX+1, nY+1);
			bAbortProc = TRUE;
			return;

		case OBJ_BORG:
			ShowMessage(MG_BLOCKBO, nX+1, nY+1);
			bAbortProc = TRUE;
			return;

		case OBJ_ENTL:
		case OBJ_ENTR:
			return;

		default:
			break;
		}


	if(!SubtractEnergy(5))					 /* subract move and shield energy*/
		{
		bAbortProc = TRUE;
		return;
		}
	if(nShields)
		nShields = max(0, nShields - 10);


/* Nothing stopping us- move to the new sector */

	GetSectorRect(&rc, nEntX, nEntY);					 	  /* erase old */
	if(!fSRSRepair)
		FillRect(*(HDC far *)lphDC, &rc, GetStockObject(BLACK_BRUSH));

	if(GetQObj(nEntY, nEntX) == OBJ_ENTL)
		{
		hBM = hEntLBmp;
		SetQObj(OBJ_ENTL, nY, nX);
		}
	else
		{
		hBM = hEntRBmp;
		SetQObj(OBJ_ENTR, nY, nX);
		}

	GetSectorRect(&rc, nX, nY);
	if(!fSRSRepair)
		DrawBitmap(*(HDC far *)lphDC, rc.left, rc.top, hBM, SRCCOPY);

	SetQObj(OBJ_NONE, nEntY, nEntX);
	nEntX = nX;
	nEntY = nY;
	fStarDNow +=0.1;
	if(--nMovement == 0)
		bAbortProc = TRUE;
	Delay(700L);

} /* End ImpMovProc */




/***************************************************************************/

void LOCAL MoveEntWarp(int nDeg, int nFactor)

//	This function moves the enterprise the designated warp factor.

/***************************************************************************/
{
	POINT		ptEnd;
	FARPROC 	lpfnWarpMovProc;
	int		nNewQ;


	ShowMessage(MG_WENGAGE); 						 	/* warp engines ON msg */
	if(!SubtractEnergy(10))
		return;
	nMovement = nFactor+1;

	ptEnd = DegToCoords(nDeg, 12);	 		 	 /* compute succesive pts */
	ptEnd.x += XQUAD(nEntQ);
	ptEnd.y += YQUAD(nEntQ);

	bAbortProc = FALSE;
	lpfnWarpMovProc = MakeProcInstance((FARPROC)WarpMovProc, hInst);
	LineDDA(XQUAD(nEntQ), YQUAD(nEntQ), ptEnd.x, ptEnd.y,
		(LINEDDAPROC)lpfnWarpMovProc, (LPARAM)(LPPOINT)&ptEnd);
	FreeProcInstance(lpfnWarpMovProc);						/* call the callback	*/

	nMovement = nFactor;		 							  /* refresh for next call */
	nNewQ = AdjQuad(nEntQ, ptEnd.x - XQUAD(nEntQ), ptEnd.y - YQUAD(nEntQ));
	WarpToQuad(nNewQ);

	ShowMessage(MG_STOPPED, nEntX+1, nEntY+1);
	InvalidateRect(hPANWnd, NULL, 0);
	UpdateLRS(nEntQ);				 								 	/* Mark LRS scan	*/
	SetCondition();

} /* End MoveEntWarp */




/***************************************************************************/

void WarpToQuad(int nNewQuad)

//	This function performs the drawing and updating of the enterprise warping
//	to a new quadrant. It draws the star pattern in the SRS window, subtracts
//	the required energy, and updates all necessary global values. 

/***************************************************************************/
{
	POINT 	pt;
	HDC		hDC;
	RECT		rc;


	bAbortProc = TRUE;

	SetQObj(OBJ_NONE, nEntY, nEntX);						/* fix up array values*/
	nEntQ = nNewQuad;
	pt 	= RandomXY(nEntQ, 0xffff);		 			
	if(pt.x > 3)
		qd[nEntQ].Obj[pt.y][pt.x] = MAKEWORD(OBJ_ENTL, 0);
	else
		qd[nEntQ].Obj[pt.y][pt.x] = MAKEWORD(OBJ_ENTR, 0);
	nEntX = pt.x;
	nEntY = pt.y;


/* Draw the warping graphics */

	hDC = GetDC(hSRSWnd);
	GetClientRect(hSRSWnd, &rc);
	Stars(hDC, nMovement, &rc);
	ReleaseDC(hSRSWnd, hDC);

/* Clean up and exit */

	bAbortProc = FALSE;
	UpdateWindow(hSRSWnd);
	SubtractTime(nMovement);
	SubtractEnergy(nMovement * 20);
	if(nShields)
		nShields = max(0, nShields - (nMovement * 20));

	InvalidateRect(hSRSWnd, NULL, 1);

} /* End WarpToQuad */





/***************************************************************************/

void CALLBACK _export WarpMovProc(int nX, int nY, LPSTR lppt)

//	This is the LineDDA() callback that handles warp engine movement.
//	It returns the new point value until the nMovement factor is reached.
// Note that unlike ImpMovProc, this function does not draw any graphics.

/***************************************************************************/
{
	LPPOINT lpt;

	if(bAbortProc)
		return;


	lpt = (LPPOINT)lppt;
	lpt->x = nX;
	lpt->y = nY;
	if(--nMovement == 0)
		bAbortProc = TRUE;


} /* End WarpMovProc */




/***************************************************************************/

void LOCAL Stars(HDC hDC, int nDistance, LPRECT lprc)

//	This function draws the stars animation while we are traveling at warp
//	factor nDistance. If nDistance is zero, a ricochet effect is enabled.

/***************************************************************************/
{

	#define MAXSTARS 55
	#define VELOCITY 0.4

	int 		i, j = 0;
	double	velocity = VELOCITY;
	STARS 	s[MAXSTARS];
	BOOL		bRic = nDistance;
	DWORD		dwTick;

	bInWarp = TRUE;
	xres = lprc->right - lprc->left;		 		 /* make window dimensions */
	yres = lprc->bottom - lprc->top;
	if(nDistance)
		nDistance *= 45;
	else
		nDistance = 90;

	PlaySound(SND_IMP, 1);
	if(fSRSRepair)
		{														/* don't do it if broke */
		Delay(nDistance * 70L);
		KillSound();
		bInWarp = FALSE;
		return;
		}


	for (i = 0; i < MAXSTARS; i++)
		InitStar(&s[i]);

	FillRect(hDC, lprc, GetStockObject(BLACK_BRUSH));
	dwTick = GetTickCountEx();

	while(--nDistance)
		{
		for (i = 0; i < MAXSTARS; i++)
			MoveStar(hDC, &s[i], velocity, -1);
		do
			{
			MyYield();
			} while(GetTickCountEx() < dwTick + 60L);
		dwTick = GetTickCountEx();
		}

	if(!bRic)			  						 /* Add only on ricochet effect */
		{
		while(j == 0)
			{
			j = 1;
			for (i = 0; i < MAXSTARS; i++)
				{
				if(s[i].z > 0.0)
					{
					MoveStar(hDC, &s[i], velocity, 0);
					j = 0;
					}
				}
			Delay(40L);
			}
		}

	bInWarp = FALSE;
	KillSound();


} /* End Stars */




/***************************************************************************/

void LOCAL InitStar(STARS *s)

//	This function initializes the star positions.

/***************************************************************************/
{
	s->x  = ((double)GetRand(201)-100.0)/10.0;
	s->y  = ((double)GetRand(201)-100.0)/10.0;
	s->z  = (double)GetRand(15)+10.0;
	s->ox = 0;
	s->oy = 0;
	s->color = RGB_GRAY;

} /* End InitStar */




/***************************************************************************/

void LOCAL MoveStar(HDC hDC, STARS *s, double v, int erase )

//	This function moves a star during the warping process.

/***************************************************************************/
{
	int x, y, x2, y2;

	x2 = xres >> 1;
	y2 = yres >> 1;
	x = (int)((s->x / s->z) * x2);
	y = (int)((s->y / s->z) * y2);


	if (s->z < 3.0)
		{
		SetPixel(hDC, x+x2+1, y+y2, RGB_WHITE);
		if(erase)
			SetPixel(hDC, (s->ox)+1, s->oy, RGB_BLACK);
		}
	else if (s->z < 6.0)
		s->color = RGB_WHITE;

	if(erase)
		SetPixel(hDC, s->ox, s->oy, RGB_BLACK);
					  
	SetPixel(hDC, x+x2, y+y2, s->color);
	s->ox = x+x2; s->oy = y+y2;
	s->z -= v;

	if(erase)
		if (s->z <= 0.0)
			InitStar(s);

}	/* End MoveStar */





/***************************************************************************/

void LOCAL RicochetToQuad(void)

//	This function performs the ricochet effect. The stars are drawn, then
//	a new random Quadrant and sector is selected.

/***************************************************************************/
{
	POINT 	pt;
	HDC		hDC;
	RECT		rc;



	bAbortProc = TRUE;	  							/* disable WM_PAINT responses */
	SetQObj(OBJ_NONE, nEntY, nEntX);		 				  /* fix up array values*/
	nEntQ = GetRand(64);
	pt 	= RandomXY(nEntQ, 0xffff);		 			
	if(pt.x > 3)
		qd[nEntQ].Obj[pt.y][pt.x] = MAKEWORD(OBJ_ENTL, 0);
	else
		qd[nEntQ].Obj[pt.y][pt.x] = MAKEWORD(OBJ_ENTR, 0);
	nEntX = pt.x;
	nEntY = pt.y;


/* Draw the warping graphics with ricochet */

	hDC = GetDC(hSRSWnd);
	GetClientRect(hSRSWnd, &rc);
	Stars(hDC, 0, &rc);				 				/* zero warp means a ricochet */
	ReleaseDC(hSRSWnd, hDC);


/* Clean up and exit */

	bAbortProc = FALSE;
	UpdateWindow(hSRSWnd);

	InvalidateRect(hSRSWnd, NULL, 1);
	UpdateLRS(nEntQ);				 								 	/* Mark LRS scan	*/
	nShields = 0;
	InvalidateRect(hPANWnd, NULL, 0);
	SetCondition();

} /* End RicochetToQuad */






/***************************************************************************/

void LOCAL FirePhoton(int nDeg, int nFactor)

//	This function fires a photon torpedo. It sequences the graphics, then
// updates all global parms.

/***************************************************************************/
{
	POINT		ptEnd;
	FARPROC 	lpfnPhoProc;
	HDC		hDC;

	ShowMessage(MG_PHOFIRE); 						 	/* display firing message */
	nMovement = nFactor;
	--nPhotonCnt;

	ptEnd = DegToCoords(nDeg, 12);	 		 	/* determine succesive points */
	ptEnd.x += nEntX;
	ptEnd.y += nEntY;

	hDC = GetDC(hSRSWnd);
	bAbortProc = FALSE;
	nOldX = -10; nOldY = -10;
	lpfnPhoProc = MakeProcInstance((FARPROC)PhoProc, hInst);
	LineDDA(nEntX, nEntY, ptEnd.x, ptEnd.y, (LINEDDAPROC)lpfnPhoProc,
		(LPARAM)(HDC far *)&hDC);
	FreeProcInstance(lpfnPhoProc);							/* call the callback	*/
	ReleaseDC(hSRSWnd, hDC);
	bAbortProc = FALSE;

	InvalidateRect(hPANWnd, NULL, 0);
	SetCondition();

} /* End FirePhoton */






/***************************************************************************/

void CALLBACK _export PhoProc(int nX, int nY, LPSTR lphDC)

//	This is the LineDDA() callback that handles photon movement.
//	It redraws the photon at subsequent locations. If an object is hit,
// the callback quits and returns zero. Appropriate messages are sent to
// the message window within this function.

/***************************************************************************/
{
	int		nEnergy, nA;
	RECT		rc;



	if(nOldX != -10)
		{
		GetSectorRect(&rc, nOldX, nOldY);				 	 /* erase old photon */
	   if(!fSRSRepair)
			FillRect(*(HDC far *)lphDC, &rc, GetStockObject(BLACK_BRUSH));
		}

	if(bAbortProc)
		return;

	if(nX > 7 || nX < 0 || nY > 7 || nY < 0) /* photon exceed sector limits? */
		{
		ShowMessage(MG_PHOLOST);
		bAbortProc = TRUE;
		return;
		}


	switch(GetQObj(nY, nX))				 	  		 /* see if an object gets hit */
		{
		case OBJ_BASE:
			ShowMessage(MG_PHOSHIP, nX+1, nY+1, 250);
			ExplodeSequence(*(HDC far *)lphDC, nX, nY, TRUE);
			--qd[nEntQ].Bases;
			ShowMessage(MG_PHOBASE);
			qd[nEntQ].Obj[nY][nX] = 0;
			bAbortProc = TRUE;
			return;


		case OBJ_STAR0:
		case OBJ_STAR1:
		case OBJ_STAR2:
			ShowMessage(MG_PHOSTAR, nX+1, nY+1);
			bAbortProc = TRUE;
			return;

		case OBJ_KLINGON:
		case OBJ_ROMULAN:
		case OBJ_FARENG:
		case OBJ_BORG:
			switch(nSkill)
				{								 /* Set Photon strength based on skill */
				case 0:
				case 1: nEnergy = GetRand(80) + 200; break;
				case 2:
				case 3: nEnergy = GetRand(80) + 150; break;
				case 4:
				default:nEnergy = GetRand(80) + 100; break;	
				}

			if(nMovement == 1)		  				 /* add exact distance factor */
				nEnergy +=70;

			if((nA = FindAlien(nEntQ, nX, nY)) == -1)
				{																/* fatal error */
				FATAL();
			   bAbortProc = TRUE;
				return;
				}

			pAL[nA].Energy -= nEnergy;
			if(pAL[nA].Energy <= 0)						  	/* alien was destroyed */
				{								  				  
				ShowMessage(MG_PHOSHIP, nX+1, nY+1, nEnergy);
				ExplodeSequence(*(HDC far *)lphDC, nX, nY, TRUE);
				--qd[nEntQ].Aliens;
				pAL[nA].Quad = -1;
				qd[nEntQ].Obj[nY][nX] = 0;
				ShowMessage(MG_PHODEST);
				--nAlienCnt;
				}
			else												 /* alien survived the hit */
				{
				ShowMessage(MG_PHOSHIP, nX+1, nY+1, nEnergy);
				ExplodeSequence(*(HDC far *)lphDC, nX, nY, FALSE);
				}

			bAbortProc = TRUE;
			return;


		case OBJ_ENTL:
		case OBJ_ENTR:
			return;


		default:
			break;
		}



/* Nothing stopping us- move the photon to the new sector */


	GetSectorRect(&rc, nX, nY);
	nOldX = nX;
	nOldY = nY;
	   if(!fSRSRepair)
			DrawBitmap(*(HDC far *)lphDC, rc.left+15, rc.top+9, hPhoBmp, SRCCOPY);
	--nMovement;
	Delay(500L);

} /* End PhoProc */





/***************************************************************************/

void LOCAL ExplodeSequence(HDC hDC, int nX, int nY, BOOL bDestroy)

//	This function displays the explosion graphics at the designated nX, nY
//	coordinates. If bDestroy is true, the object is destroyed, leaving an
//	empty sector. This routine does not update any global vars.

/***************************************************************************/
{
	RECT		rc;
	HBITMAP	hBM, hOldBM, hOrigBM;
	HDC		hMemDC;
	int		i;


	switch(GetQObj(nY, nX))
		{
		case OBJ_BASE:
			hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXPLBASD));
			hOrigBM = hBaseBmp;
			break;


		case OBJ_KLINGON:
			if(bDestroy)
				hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXPLKLGD));
			else
				hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXPLKLG));
			hOrigBM = hKlingonBmp;
			break;


		case OBJ_ROMULAN:
			if(bDestroy)
				hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXPLROMD));
			else
				hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXPLROM));
			hOrigBM = hRomulanBmp;
			break;


		case OBJ_FARENG:
			if(bDestroy)
				hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXPLFARD));
			else
				hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXPLFAR));
			hOrigBM = hFarengBmp;
			break;


		case OBJ_BORG:
			if(bDestroy)
				hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXPLBRGD));
			else
				hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXPLBRG));
			hOrigBM = hBorgBmp;
			break;

		case OBJ_ENTL:
			hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXPLENTL));
			hOrigBM = hEntLBmp;
			break;

		case OBJ_ENTR:
			hBM = LoadBitmap(hLib, MAKEINTRESOURCE(BMP_EXPLENTR));
			hOrigBM = hEntRBmp;
			break;

		default:
			FATAL();
			return;
		}

	GetSectorRect(&rc, nX, nY);
	hMemDC = CreateCompatibleDC(hDC);
	hOldBM = SelectBitmap(hMemDC, hBM);		 /* Select frame sequence bitmap */

	if(bDestroy)
		PlaySound(SND_EXPLO, 0);

	if(fSRSRepair && bDestroy)
		Delay(1000L);									 /* delay instead of graphics */


	for(i = 0; i < EXPLSEQ; i++)
		{
		if(!fSRSRepair)
			StretchBlt(hDC, rc.left-1, rc.top-1, XBM-2, YBM-2,
				hMemDC, 0, i*(YBM-2), XBM-2, YBM-2, SRCCOPY);
		Delay(120L);
		}

	if(bDestroy && !fSRSRepair)
		FillRect(hDC, &rc, GetStockObject(BLACK_BRUSH));
	else
		{
		SelectBitmap(hMemDC, hOrigBM);				 /* redraw original object */
		if(!fSRSRepair)
			StretchBlt(hDC, rc.left-1, rc.top-1, XBM-2, YBM-2,
				hMemDC, 0, 0, XBM-2, YBM-2, SRCCOPY);
		}

	SelectBitmap(hMemDC, hOldBM);
	DeleteDC(hMemDC);
	DeleteBitmap(hBM);


} /* End ExplodeSequence */




/***************************************************************************/

void LOCAL FirePhasers(int nPower, BOOL bSingle)

//	This function does the graphics for firing the phasers. The power level
//	specified is fired upon any aliens. Global vars are updated.

/***************************************************************************/
{
	int			nIndex[8];						 /* max is 8 aliens per quadrant */
	int			nX, nY, nXa, nYa,
					nDist, nAlCount, nOldROP, i;
	HDC			hDC;
	RECT			rc;
	DWORD			dwTime;
	BOOL			bBlocked;
	int			nXEnd, nYEnd, nPowerD, nPower2;



	if(qd[nEntQ].Aliens == 1)
		bSingle = TRUE;								/* force single on one alien */

	ShowMessage(MG_PHAFIRE); 						 	/* display firing message */


/* Make a list of all aliens in the quadrant */

	nAlCount = 0;
	for(nY = 0; nY < 8; nY++)
		{
		for(nX = 0; nX <	8; nX++)
			{
			if((nIndex[nAlCount] = FindAlien(nEntQ, nX, nY)) == -1)
				continue;
			++nAlCount;
			}
		}

	GetSectorRect(&rc, nEntX, nEntY);
	nX = (RCWIDTH(rc) >> 1) + rc.left;	 	  /* set enterprise X,Y for line */
	nY = (RCHEIGHT(rc)>> 1) + rc.top;
	hDC = GetDC(hSRSWnd);
	nOldROP = SetROP2(hDC, R2_XORPEN);

	if(nAlCount)		 										  /* one or more aliens */
		{
		if(bSingle)
			nAlCount = 1;						 /* force back to one on single sel */
		nPowerD = nPower/nAlCount;			 /* get divided max power level		*/

		for(i = 0; i < nAlCount; i++)
			{
			nXEnd = pAL[nIndex[i]].SecX;
			nYEnd = pAL[nIndex[i]].SecY;
			bBlocked = BlockedByStar(nEntX, nEntY, &nXEnd, &nYEnd);

			nDist = (abs(nEntX - nXEnd)) + abs(nEntY - nYEnd) >> 1;
			nDist *=(GetRand(5) + 5);				 /* subtract distance penalty */
			nPower2 = max(0, (nPowerD - nDist));
			dwTime = (DWORD)nPower2 * 10L;	  /* set phaser line display time*/

			GetSectorRect(&rc, nXEnd, nYEnd);
			nXa = (RCWIDTH(rc) >> 1) + rc.left;	 	 /* set alien X,Y for line */
			nYa = (RCHEIGHT(rc)>> 1) + rc.top;
			AnimatePhasers(hDC, nX, nY, nXa, nYa, dwTime, 0);

			if(bBlocked)
				{
				ShowMessage(MG_STARHIT, nXEnd+1, nYEnd+1, nPower2);
				}
			else
				{
				pAL[nIndex[i]].Energy -= nPower2;
				if(pAL[nIndex[i]].Energy <= 0) 			 /* alien was destroyed */
					{								  				  
					ShowMessage(MG_PHAHIT, nXEnd+1, nYEnd+1, nPower2);
					ExplodeSequence(hDC, nXEnd, nYEnd, TRUE);
					--qd[nEntQ].Aliens;
					pAL[nIndex[i]].Quad = -1;	 	/* effective remove from list */
					qd[nEntQ].Obj[nYEnd][nXEnd] = 0;
					ShowMessage(MG_PHODEST);
					--nAlienCnt;
					}
				else											 /* alien survived the hit */
					{
					ShowMessage(MG_PHAHIT, nXEnd+1, nYEnd+1, nPower2);
					ExplodeSequence(hDC, nXEnd, nYEnd, FALSE);
					}
				}
			}

		}
	else
		{	  												/* no alients in the quadrant */
		GetClientRect(hSRSWnd, &rc);
		nYa = GetRand(RCHEIGHT(rc)) + YOFF;
		if(GetQObj(nEntY, nEntX) == OBJ_ENTL)
			nXa = XOFF;									  				 /* random firing */
		else
			nXa = rc.right + 1;
		AnimatePhasers(hDC, nX, nY, nXa, nYa, (DWORD)nPower * 10L, 0);
		ShowMessage(MG_PHANONE);

		}
	SetROP2(hDC, nOldROP);
	ReleaseDC(hSRSWnd, hDC);
	InvalidateRect(hPANWnd, NULL, 0);
	SetCondition();


} /* End FirePhasers */




/***************************************************************************/

void LOCAL AnimatePhasers(HDC hDC, int nXS, int nYS, int nXE, int nYE, DWORD dwTime, BOOL bAlien)

//	This function draws the toggling lines on the SRS screen to show the
//	phaser beams. The line(s) are drawn from the starting (nXS, nYS) to the
//	ending (nXE, nYE) positions.

/***************************************************************************/
{

	HPEN			hPen1, hPen2, hOldPen;
	DWORD			dwEndTime;


	if(bAlien)						  							/* select phaser colors */
		{
		PlaySound(SND_PHX, 1);
		hPen1 = CreatePen(PS_SOLID, 1, RGB_BLUE);
		hPen2 = CreatePen(PS_SOLID, 1, RGB_AQUA);
		}
	else
		{
		PlaySound(SND_PHAS, 1);
		hPen1 = CreatePen(PS_SOLID, 1, RGB_RED);
		hPen2 = CreatePen(PS_SOLID, 1, RGB_PURPLE);
		}


	hOldPen = SelectPen(hDC, hPen1);

	dwEndTime = GetTickCountEx() + dwTime;

	while(GetTickCountEx() < dwEndTime)
		{
		if(fSRSRepair)
			{
			Delay(100L);
			continue;
			}
		SelectPen(hDC, hPen1);
		MoveTo(hDC, nXS, nYS);			 			/* draw the line  */
		LineTo(hDC, nXE, nYE);
		Delay(60L);					 					/* wait a bit	   */
		MoveTo(hDC, nXS, nYS);
		LineTo(hDC, nXE, nYE);		 				/* erase the line */

		SelectPen(hDC, hPen2);
		MoveTo(hDC, nXS, nYS);
		LineTo(hDC, nXE, nYE);
		Delay(40L);
		MoveTo(hDC, nXS, nYS);
		LineTo(hDC, nXE, nYE);
		MyYield();
		}

	if(bAlien)
		PlaySound(SND_PHX2, 0);
	else
		KillSound();

	SelectPen(hDC, hOldPen);
	DeletePen(hPen1);
	DeletePen(hPen2);


} /* End AnimatePhasers */




/***************************************************************************/

void ProcessRed(void)

//	This function handles alien movements and firing during a condition
//	red. It is called continuously from our main message loop.


/***************************************************************************/
{
	DWORD		    dwTick;
	static DWORD dwLTick;
	int		    nX, nY, nA;
	static BOOL	 bFirst;


	dwTick = GetTickCountEx();


/* If just entered Red Alert, set all	 */
/* alienmove/fire times only and return */


	if(bResetTick)
		{	  	
		for(nY = 0; nY < 8; nY++)		 
			{
			for(nX = 0; nX < 8; nX++)
				{
				if(qd[nEntQ].Obj[nY][nX] & (WORD)OBJ_ALIENMASK)
					{
					nA = FindAlien(nEntQ, nX, nY);
					pAL[nA].MStartTick = dwTick;  		/* set starting times */
					pAL[nA].FStartTick = dwTick;
					}
				}
			}
		bResetTick = FALSE;
		bFirst = TRUE;
		dwLTick = dwTick;
		return;
		}


	if(dwTick + 1000L < dwLTick)	  		/* limit updates to every 2 seconds */
		return;

	dwLTick = dwTick;


/* See if any aliens need to move or fire */

	for(nY = 0; nY < 8; nY++)		 
		{
		for(nX = 0; nX < 8; nX++)
			{
			if(qd[nEntQ].Obj[nY][nX] & (WORD)OBJ_ALIENMASK)
				{
				nA = FindAlien(nEntQ, nX, nY);
				if((dwTick > (pAL[nA].MStartTick + pAL[nA].MTick)) ||
					(bFirst && (GetRand(100) > 94)))
					{
					if(!bSRSBusy)
						{
						bSRSBusy = TRUE;
						WaitCursorX(hFrameWnd);

						RandomAlienMove(nA);
						pAL[nA].MStartTick = dwTick;  		/* reset start time */
						bSRSBusy = FALSE;
						ArrowCursorX();
						MyYield();
						}
					}
				if((dwTick > (pAL[nA].FStartTick + pAL[nA].FTick)) ||
					(bFirst && (GetRand(100) > 94)))
					{									  
					if(!bSRSBusy)
						{
						if(dwTick > ((pAL[nA].FStartTick + pAL[nA].FTick)+15000L))
							{
							pAL[nA].FStartTick = dwTick; /* if >15 sec, just reset */
							}
						else
							{
							bSRSBusy = TRUE;
							WaitCursorX(hFrameWnd);
							AlienFires(nA);
							pAL[nA].FStartTick = dwTick; 		/* reset start time */
							bSRSBusy = FALSE;
							ArrowCursorX();
							}

						}
					}
				}
			}
		}
	bFirst = FALSE;


} /* End ProcessRed */





/***************************************************************************/

void LOCAL RandomAlienMove(int nA)

//	This function randomly moves an alien to a new surrounding sector.
// nA is the index into the pAL structure	for the alien.

/***************************************************************************/
{
	int		nX, nY, nPos, nDest;
	POINT		pt[8];
	HDC		hDC;
	RECT		rc;
	HBITMAP	hBM;


/* Make a list of all qualifying positions */

	nPos = 0;
	for(nX = pAL[nA].SecX-1; nX < pAL[nA].SecX + 2; nX++)
		{
		if(nX < 0 || nX > 7)									 /* don't exceed bounds */
			continue;
		for(nY = pAL[nA].SecY-1; nY < pAL[nA].SecY + 2; nY++)
			{
			if(nY > 7 || nY < 0)								 /* don't exceed bounds */
				continue;
			if(nX == pAL[nA].SecX && nY == pAL[nA].SecY)	/* skip original loc */
				continue;
			if(qd[nEntQ].Obj[nY][nX] == OBJ_NONE)
				{
				pt[nPos].x = nX;
				pt[nPos].y = nY;
				++nPos;
				}
			}
		}

	if(!nPos)	 												 /* none qualified */
		return;


/* Redraw in new position */

	nDest = GetRand(nPos);

	if(!fSRSRepair)
		{
		hBM = ObjToBitmap(pAL[nA].Obj);
		hDC = GetDC(hSRSWnd);
		GetSectorRect(&rc, pAL[nA].SecX, pAL[nA].SecY);
		FillRect(hDC, &rc, GetStockObject(BLACK_BRUSH));  		/* erase old */
		GetSectorRect(&rc, pt[nDest].x, pt[nDest].y);
		DrawBitmap(hDC, rc.left, rc.top, hBM, SRCCOPY);		 	/* draw new  */
		ReleaseDC(hSRSWnd, hDC);
		}

	SetQObj(OBJ_NONE, pAL[nA].SecY, pAL[nA].SecX);
	SetQObj(pAL[nA].Obj, pt[nDest].y, pt[nDest].x);
	pAL[nA].SecX = pt[nDest].x;
	pAL[nA].SecY = pt[nDest].y;


} /* End RandomAlienMove */




/***************************************************************************/

void LOCAL AlienFires(int nA)

//	This function makes an alien shoot at the Enterprise. nA is the index
// into the pAL structure for the alien.

/***************************************************************************/
{
	int	nPower, nEX, nEY, nAX, nAY;
	int	nXEnd, nYEnd;
	RECT	rc;
	DWORD dwTime;
	HDC	hDC;
	int	nOldROP;


	nPower = GetRand(pAL[nA].Energy / 2) + (pAL[nA].Energy / 2);
	nXEnd = nEntX;
	nYEnd = nEntY;
	if(BlockedByStar(pAL[nA].SecX, pAL[nA].SecY, &nXEnd, &nYEnd))
		return;								  		 /* don't do anything if blocked */

	GetSectorRect(&rc, nXEnd, nYEnd);
	nEX = (RCWIDTH(rc) >> 1) + rc.left;	 	  /* set enterprise X,Y for line */
	nEY = (RCHEIGHT(rc)>> 1) + rc.top;
	GetSectorRect(&rc, pAL[nA].SecX, pAL[nA].SecY);
	nAX = (RCWIDTH(rc) >> 1) + rc.left;	 	  /* set alien X,Y for line */
	nAY = (RCHEIGHT(rc)>> 1) + rc.top;
	dwTime = (DWORD)nPower * 10L;				  /* set phaser line display time*/

	hDC = GetDC(hSRSWnd);
	nOldROP = SetROP2(hDC, R2_XORPEN);
	AnimatePhasers(hDC, nAX, nAY, nEX, nEY, dwTime, 1);

	ExplodeSequence(hDC, nEntX, nEntY, FALSE);
	SetROP2(hDC, nOldROP);
	ReleaseDC(hSRSWnd, hDC);
	ShowMessage(MG_ENTHIT, nPower);


/* Subract hit energy from shields */

	if(nShields > nPower)
		{													/* Shields will hold */
		ShowMessage(MG_SHEHOLD, (unsigned)(((float)nPower/(float)nShields) * 100.0));
		ShowMessage(MG_SHEHLD2);
		nShields -= nPower;
		InvalidateRect(hPANWnd, NULL, 0);
		return;
		}

	if(nShields && (nShields < nPower))
		{										 			/* Shields are going down */
		nShields -= nPower;
		RandomDamage(abs(nShields));				/* Go do some damage */
		nShields = 0;
		ShowMessage(MG_SHEDOWN);
		InvalidateRect(hPANWnd, NULL, 0);
		SetCondition();
		return;
		}

	RandomDamage(nPower);			  			/* Shields are already down */	 
	InvalidateRect(hPANWnd, NULL, 0);
	SetCondition();


} /* End AlienFires */




/***************************************************************************/

void RandomDamage(int nHit)

//	This function checks to see if a system is damaged based on the passed
//	amount of hit energy. An appropriate message is displayed and the repair
//	status of the system is changed.

/***************************************************************************/
{
	int		nP, nRes;
	BOOL		bKil;
	HWND		hWnd;
	double   *pfRepair, *pfRep, fDays;


/* Set probability based on skill level, Energy, and random factor */

	nP = (nSkill * GetRand(15)) + nHit;

	if(nP < 75 || (GetRand(20) < 2))
		{					  										/* Minor Damage Only */
		ShowMessage(DM_MINOR+GetRand(DM_MINORMAX));
		return;
		}

	if(nP < 150)
		bKil = (BOOL)(GetRand(22) < 2);	  	/* Set permanent damage odds */
	else if(nP < 250)
		bKil = (BOOL)(GetRand(20) < 3);
	else if(nP < 350)
		bKil = (BOOL)(GetRand(20) < 4);
	else if(nP < 450)
		bKil = (BOOL)(GetRand(15) < 5);
	else
		bKil = TRUE;								/* >450 is always	destroyed  */

	fDays	= (double)(nHit / 12);				/* set repair time in days   */
	fDays /= 10.0;
	nRes = GetRand(10);

	if(nRes == 7)	 						 /* make it harder to damage the DAM */
		{
		if(GetRand(10) > 4)				 /* 60% chance	*/
			nRes = GetRand(10);
		}

	switch(nRes)
		{
		case 0: pfRepair = &fNAVRepair; pfRep = &fNAVRep; hWnd = hNAVWnd; break;
		case 1: pfRepair = &fSRSRepair; pfRep = &fSRSRep; hWnd = hSRSWnd; break;
		case 2: pfRepair = &fLRSRepair; pfRep = &fLRSRep; hWnd = hLRSWnd; break;
		case 3: pfRepair = &fSHERepair; pfRep = &fSHERep; hWnd = hSHEWnd; break;
		case 4: pfRepair = &fPHARepair; pfRep = &fPHARep; hWnd = hPHAWnd; break;
		case 5: pfRepair = &fPHORepair; pfRep = &fPHORep; hWnd = hPHOWnd; break;
		case 6: pfRepair = &fCOMRepair; pfRep = &fCOMRep; hWnd = hCOMWnd; break;
		case 7: pfRepair = &fDAMRepair; pfRep = &fDAMRep; hWnd = hDAMWnd; break;
		case 8: pfRepair = &fPANRepair; pfRep = &fPANRep; hWnd = hPANWnd; break;
		case 9: pfRepair = &fMSGRepair; pfRep = &fMSGRep; hWnd = hMSGWnd; break;
		}

	
	if(bKil && (*pfRepair != -1.0))
		{										 					/* system is destroyed */
		if(hWnd == GetFocus())
			SetFocus(hBTNWnd);
		ShowMessage(nRes+DM_KILNAV);
		*pfRepair = -1.0;
		*pfRep = 0.0;
		InvalidateRect(hWnd, NULL, (BOOL)(hWnd != hPANWnd));
		UpdateWindow(hWnd);
		if(!fDAMRepair)
			InvalidateRect(hDAMWnd, NULL, 0);
		}

	else if(*pfRepair > 0.0)
		{										 		/* system gets additional damage */
		if(hWnd == GetFocus())
			SetFocus(hBTNWnd);
		ShowMessage(nRes+DM_ADDNAV);
		*pfRepair += fDays;
		*pfRep += fDays;
		if(!fDAMRepair)
			InvalidateRect(hDAMWnd, NULL, 0);
		}

	else if(*pfRepair == 0.0)
		{												/* system gets 1st major damage */
		if(hWnd == GetFocus())
			SetFocus(hBTNWnd);
		ShowMessage(nRes+DM_MAJNAV);
		*pfRepair = fDays;
		*pfRep = fDays + fStarDNow;
		InvalidateRect(hWnd, NULL, (BOOL)(hWnd != hPANWnd));
		UpdateWindow(hWnd);
		if(!fDAMRepair)
			InvalidateRect(hDAMWnd, NULL, 0);
		}

	

} /* End RandomDamage */





/***************************************************************************/

HBITMAP LOCAL ObjToBitmap(int nObj)

//	This function converts an SRS OBJECT type to a bitmap. If nObj is
// is illegal, a NULL is returned. Otherwise, the preloaded SRS bitmap 
// handle is returned.

/***************************************************************************/
{
	HBITMAP	hBM;

	switch(nObj)
		{
		case OBJ_ENTL:		hBM = hEntLBmp;	break;
		case OBJ_ENTR:		hBM = hEntRBmp;	break;
		case OBJ_BASE:		hBM = hBaseBmp;	break;
		case OBJ_STAR0:	hBM = hStar0Bmp;	break;
		case OBJ_STAR1:	hBM = hStar1Bmp;	break;
		case OBJ_STAR2:	hBM = hStar2Bmp;	break;
		case OBJ_KLINGON:	hBM = hKlingonBmp;break;
		case OBJ_ROMULAN:	hBM = hRomulanBmp;break;
		case OBJ_FARENG:	hBM = hFarengBmp;	break;
		case OBJ_BORG:		hBM = hBorgBmp;	break;
		default:				hBM = NULL;			break;
		}
	return hBM;

} /* End ObjToBitmap */





