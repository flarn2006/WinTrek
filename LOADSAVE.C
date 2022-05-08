/************************************
 *		 STAR TREK CLASSIC	 			
 *								  				
 *	Game load/save routines
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:52  $
 * Log File:	$Logfile:   C:/src/st/vcs/loadsave.c_v  $
 * Log:			$Log:   C:/src/st/vcs/loadsave.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:52   jaworski
 * Initial Revision
 *								  				
 ************************************/

#define	INIT_C
#include "exclude.h"

#include <windows.h>
#include <commdlg.h>
#include "joes.h"
#include	<stdio.h>
#include	<stdlib.h>
#include <string.h>

#include "st.h"			 								/* Global include files		*/	
#include "resource.h"
#include "protos.h"





/***************************************************************************/

int AskToSave(void)

//	This functions queries the user whether or not to save the current game.
//	The return value is either IDYES, IDNO, or IDCANCEL. If the user chooses
// to save the game, this function calls the SaveGame() function.
//
//	If	Condtion is NoAlert (indicating no game is currently in play), the
//	function returns IDNO. If a Red Alert is in effect, the user will be
// warned that the current	game will be lost, and either IDNO or IDCANCEL
// will be returned.

/***************************************************************************/
{

	char	szBuf[128];
	int	nRes;

	if(Condition == NoAlert)  									/* no game in play */
		return IDNO;

	if(!bNeedSave)  		 							 /* no save if not required */
		return IDNO;


	if(Condition == RedAlertOn || Condition == RedAlertOff)
		{
		if(ResBox(hInst, hFrameWnd, RS_REDALRTSAVE, RS_SAVEGAME, MB_OKCANCEL |
			MB_ICONEXCLAMATION) == IDOK)
			return IDYES;
		return IDCANCEL;
		}

	wsprintf(szBuf, ResToString(RS_SAVEQUERY), (LPSTR)szGFname);
	nRes = MessageBox(hFrameWnd, szBuf, ResToString(RS_SAVEGAME),
			 MB_YESNOCANCEL | MB_ICONQUESTION);
	if(nRes == IDYES)
		{
		if(*szGFname)
			SaveGame();	  				  					/* save without prompting */
		else
			{
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT |
					OFN_HIDEREADONLY;
			if(!GetSaveFileName(&ofn))
					return nRes;		
			SaveGame();
			}
		}
	return nRes;


} /* End AskToSave */





/***************************************************************************/

void NewGame(void)

//	This function creates a new game. Any existing game that may be running
// is lost.

/***************************************************************************/
{

	int		i, nQ;
	int		nOdds, nOddsK, nOddsR, nOddsF;
	BYTE		yObj;
	DWORD		dwFT, dwMT;  						 	  /* temp move and fire ticks */
	int		nEnergy;
	POINT		pt;
	HRSRC		hSrcRes;
	HGLOBAL	hGRes;
	char		szBuf1[512], szBuf2[512];


	if(Condition != NoAlert)
		{
		KillSound();
		DestroyGame();	 			  				 /* game was in play so destroy */
		}

	PlaySound(SND_CODE1, 0);
	fStarDStart		= fStarDNow = (float)(GetRand(7000))+1000.0;
	fMissionL		= 34.0 + (double)GetRand(10);

	switch(nSkill)
		{
		case 0:										  	  /* Expendable Crewman level */
			nStarBaseCnt	= GetRand(4) + 2;
			nTotStars		= GetRand(100)+50;
			nAlienCnt = nAlienTotal	= 20 + GetRand(10);
			nOddsK = 55; nOddsR = 80; nOddsF = 98;
			break;

		case 1:											  				/* Yeoman level */
			nStarBaseCnt	= GetRand(3) + 2;
			nTotStars		= GetRand(100)+65;
			nAlienCnt = nAlienTotal = 25 + GetRand(10);
			nOddsK = 50; nOddsR = 80; nOddsF =97;
			break;

		case 2:											  				/* Officer Level */
			nStarBaseCnt	= GetRand(3) + 1;
			nTotStars		= GetRand(100)+80;
			nAlienCnt = nAlienTotal = 30 + GetRand(10);
			nOddsK = 50; nOddsR = 80; nOddsF =96;
			break;

		case 3:												  	  /* 1st Officer level */
			nStarBaseCnt	= GetRand(3) + 1;
			nTotStars		= GetRand(100)+95;
			nAlienCnt = nAlienTotal= 35 + GetRand(10);
			nOddsK = 45; nOddsR = 80; nOddsF =95;
			break;

		case 4:													  		/* Captain Level */
			nStarBaseCnt	= (rand() & 0x0001) + 1;
			nTotStars		= GetRand(100)+110;
			nAlienCnt = nAlienTotal = 35 + GetRand(10);
			nOddsK = 40; nOddsR = 80; nOddsF =94;
			break;

		default:											 /* default is Admiral level */
			nStarBaseCnt	= (rand() & 0x0001) + 1;
			nTotStars		= GetRand(100)+125;
			nAlienCnt = nAlienTotal = 40 + GetRand(10);
			nOddsK = 40; nOddsR = 80; nOddsF =94;
			break;

		}

			
/* Fill Quadrants with stars */

	for(i = 0; i < nTotStars; i++)
		{
		while(qd[nQ = GetRand(64)].Stars	> 8); /* limit to 9 stars per Quad */
		++(qd[nQ].Stars);
		switch(GetRand(3))
			{
			case 0: yObj = OBJ_STAR0; break;
			case 1: yObj = OBJ_STAR1; break;	 		    /* one of 3 varieties */
			case 2:
				yObj = OBJ_STAR2;
				if(rand() & 0x0001)
					qd[nQ].Flags |= QFLAG_RIC;			  /* Add ricochet enable? */
				break;
			}
		RandomXY(nQ, MAKEWORD(yObj, (BYTE)rand()&0x0003));
		}

/* Fill Quadrants with starbases */

	for(i = 0; i < nStarBaseCnt; i++)
		{
		while(qd[nQ = GetRand(64)].Bases	> 2);
		++(qd[nQ].Bases); 					    	/* limit to 2 bases per Quad */
		RandomXY(nQ, MAKEWORD(OBJ_BASE, (BYTE)rand()&0x0003));
		}


/* Fill Quadrants with aliens */


	pAL = (PALIENINFO)(PSTR)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
			sizeof(ALIENINFO) * nAlienTotal);

	for(i = 0; i < nAlienTotal; i++)
		{
		while(qd[nQ = GetRand(64)].Aliens > 7);   /* limit to 8 per sector */
		++(qd[nQ].Aliens);
		nOdds = GetRand(100);

		if(nOdds < nOddsK)		  					/* determine random factors */
			{
			yObj =  OBJ_KLINGON;				  		     /* Set alien type 		 */
			dwFT =  (DWORD)(GetRand(25000))+25000L;  /* Set fire ticks		 */
			dwMT =  (DWORD)(GetRand(25000))+15000L;  /* Set move Ticks 	 	 */
			nEnergy = 150 + GetRand(150);	  		     /* Set total energy 	 */
			}
		else if(nOdds < nOddsR)
			{
			yObj =  OBJ_ROMULAN;
			dwFT =  (DWORD)(GetRand(25000))+30000L;
			dwMT =  (DWORD)(GetRand(25000))+20000L;
			nEnergy = 200 + GetRand(150);
			}
		else if(nOdds < nOddsF)
			{
			yObj =  OBJ_FARENG;
			dwFT =  (DWORD)(GetRand(20000))+20000L;
			dwMT =  (DWORD)(GetRand(20000))+15000L;
			nEnergy = 50 + GetRand(100);
			}
		else
			{
			yObj =  OBJ_BORG;
			dwFT =  (DWORD)(GetRand(30000))+35000L;
			dwMT =  (DWORD)(GetRand(25000))+35000L;
			nEnergy = 500 + GetRand(500);
			}

		if(nSkill > 2)
			{
			if(rand()&0x0001)
				nEnergy +=75;					/* and more energy as well */
			if(!(rand()&0x0003))
			dwFT >>= 1;				  			/* half as much for hi levels */
			}


		
		pt = RandomXY(nQ, MAKEWORD(yObj, 0));
		pAL[i].Quad	= nQ;
		pAL[i].SecX	= pt.x;
		pAL[i].SecY	= pt.y;
		pAL[i].Obj		= yObj;				   /* build alien table- this entry */
		pAL[i].Energy = nEnergy;
		pAL[i].MTick  = dwMT;
		pAL[i].FTick  = dwFT;
		}


/* Finally, place the Enterprise in a random quadrant */

	nEntQ = GetRand(64);
	pt 	= RandomXY(nQ, 0xffff);		 					/* get coords only */
	if(pt.x > 3)
		qd[nEntQ].Obj[pt.y][pt.x] = MAKEWORD(OBJ_ENTL, 0);
	else
		qd[nEntQ].Obj[pt.y][pt.x] = MAKEWORD(OBJ_ENTR, 0);
	nEntX = pt.x;
	nEntY = pt.y;


/* Display opening starfleet memo */

	hSrcRes = FindResource(hLib, MAKEINTRESOURCE(RD_OPEN), RT_RCDATA);
	hGRes = LoadResource(hLib,	hSrcRes);
	_fstrcpy(szBuf1, (LPSTR)LockResource(hGRes));
	sprintf(szBuf2, szBuf1, rand(), nAlienCnt, fStarDStart +
		fMissionL, (int)fMissionL, nStarBaseCnt);
	StarFleetMemo((LPSTR)szBuf2);
	UnlockResource(hGRes);
	FreeResource(hGRes);


/* Initialize the remaining variables */

	nDilithium = 3000;
	nShields	  = 0;	
	nPhotonCnt = 10;
	fSRSRepair = fLRSRepair = fNAVRepair = fPHORepair = /* everything works */
	fPHARepair = fSHERepair = fCOMRepair =
	fDAMRepair = fPANRepair=  fMSGRepair = 0.0;
	bResetTick = TRUE;

	UpdateLRS(nEntQ);				 								 /* Mark LRS scan	*/
	CreateGame();
	DisplayComMessage(MAKEINTRESOURCE(COM_INIT), rand());
	ShowMessage(MG_INIMSG1);
	ShowMessage(MG_INIMSG2);

	} /* End NewGame */





/***************************************************************************/

int SaveGame(void)

//	This function saves the current game to the location specified in the
//	global 'ofn' OPENFILENAME struct szGFull[].
//
// The return value is non-zero if successful. Otherwise, the return value
// is zero and the user is alerted as to the error condition.

/***************************************************************************/
{
	HFILE		hDOS;


	WaitCursorX(hFrameWnd);

/* copy all values to the GAMESTRUCT */

	strcpy(gs.sig, "STW");
	gs.nSkill		=	nSkill;
	memcpy((PSTR)&gs.qd, (PSTR)&qd, sizeof(QUADRANT) * 64);
	gs.nStarBaseCnt=	nStarBaseCnt; 		 
	gs.nTotStars	=	nTotStars;	  		 
	gs.fStarDStart	=	fStarDStart;  		 
	gs.fStarDNow	=	fStarDNow;	  		 
	gs.nAlienTotal	=	nAlienTotal;  		 
	gs.nAlienCnt	=	nAlienCnt;	  		 
	gs.nDilithium	=	nDilithium;	  		 
	gs.nShields		=  nShields;	  
	gs.nPhotonCnt	=  nPhotonCnt;	  
	gs.fSRSRepair	=  fSRSRepair;	  
	gs.fLRSRepair	=  fLRSRepair;	  
	gs.fNAVRepair	=  fNAVRepair;	  
	gs.fPHORepair	=  fPHORepair;	  
	gs.fPHARepair	=  fPHARepair;	  
	gs.fSHERepair	=  fSHERepair;	  
	gs.fCOMRepair	=  fCOMRepair;	  
	gs.fDAMRepair	=  fDAMRepair;	  
	gs.fPANRepair	=  fPANRepair;	  
	gs.fMSGRepair	=  fMSGRepair;	  
	gs.fSRSRep		=	fSRSRep;
	gs.fLRSRep		=	fLRSRep;
	gs.fNAVRep		=	fNAVRep;
	gs.fPHORep		=	fPHORep;
	gs.fPHARep		=	fPHARep;
	gs.fSHERep		=	fSHERep;
	gs.fCOMRep		=	fCOMRep;
	gs.fDAMRep		=	fDAMRep;
	gs.fPANRep		=	fPANRep;
	gs.fMSGRep		=	fMSGRep;
	gs.fMissionL	=	fMissionL;
	gs.nEntQ			=	nEntQ;
	gs.nEntX			=	nEntX;
	gs.nEntY			=	nEntY;
	gs.bReRoute		=	bReRoute;

	if((hDOS = _lcreat(szGFull, 0)) == -1)
		{
		ArrowCursorX();
		ResBoxEx(hInst, hFrameWnd, RS_WRITEFAIL, RS_SAVEGAME, MB_OK |
			MB_ICONEXCLAMATION, (LPSTR)szGFull);
		return 0;
		}

	_lwrite(hDOS, (LPSTR)&gs, sizeof(GAMESTRUCT));
	_lwrite(hDOS, (LPSTR)pAL, sizeof(ALIENINFO) * gs.nAlienTotal);
	_lclose(hDOS);
	ShowMessage(MG_SAVED);
	bNeedSave = FALSE;
	ArrowCursorX();
	return 1;

} /* End SaveGame */





/***************************************************************************/

int LoadGame(void)

//	This function loads a new game from the location specified in the
//	global 'ofn' OPENFILENAME struct.
//
// The return value is non-zero if successful. Otherwise, the return value
// is zero and the user is alerted as to the error condition.

/***************************************************************************/
{
	int	i;
	HFILE	hDOS;


	WaitCursorX(hFrameWnd);

	if((hDOS = _lopen(szGFull, OF_READ)) == -1)
		{
		ArrowCursorX();
		ResBoxEx(hInst, hFrameWnd, RS_OPENFAIL, RS_LOADGAME, MB_OK |
			MB_ICONEXCLAMATION, (LPSTR)szGFull);
		return 0;
		}

	gs.sig[0] =	0;
	i = _lread(hDOS, (LPSTR)&gs, sizeof(GAMESTRUCT));
	if(i == -1 || !i)
		{
		ArrowCursorX();
		ResBoxEx(hInst, hFrameWnd, RS_OPENFAIL, RS_LOADGAME, MB_OK |
			MB_ICONEXCLAMATION, (LPSTR)szGFull);
		_lclose(hDOS);
		return 0;
		}

	if(strcmp(gs.sig, "STW"))
		{
		ArrowCursorX();
		ResBoxEx(hInst, hFrameWnd, RS_OPENFAIL2, RS_LOADGAME, MB_OK |
			MB_ICONEXCLAMATION, (LPSTR)szGFull);
		_lclose(hDOS);
		return 0;
		}


	if(Condition != NoAlert)
		{
		KillSound();
		DestroyGame();	 			  				 /* game was in play so destroy */
		}


/* Create new pAL list */


	if(pAL)
		LocalFree((HLOCAL)(PSTR)pAL); 						/* free alien list */

	pAL = (PALIENINFO)(PSTR)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
		sizeof(ALIENINFO) * gs.nAlienTotal);
	_lread(hDOS, (LPSTR)pAL, sizeof(ALIENINFO) * gs.nAlienTotal);

	_lclose(hDOS);


/* update all values from the GAMESTRUCT */

	nSkill			=	gs.nSkill;		  		 
	memcpy((PSTR)&qd, (PSTR)&gs.qd, sizeof(QUADRANT) * 64);
	nStarBaseCnt	=	gs.nStarBaseCnt; 		 
	nTotStars		=	gs.nTotStars;	  		 
	fStarDStart		=	gs.fStarDStart;  		 
	fStarDNow		=	gs.fStarDNow;	  		 
	nAlienTotal		=	gs.nAlienTotal;  		 
	nAlienCnt		=	gs.nAlienCnt;	  		 
	nDilithium		=	gs.nDilithium;	  		 
	nShields			=  gs.nShields;	  
	nPhotonCnt		=  gs.nPhotonCnt;	  
	fSRSRepair		=  gs.fSRSRepair;	  
	fLRSRepair		=  gs.fLRSRepair;	  
	fNAVRepair		=  gs.fNAVRepair;	  
	fPHORepair		=  gs.fPHORepair;	  
	fPHARepair		=  gs.fPHARepair;	  
	fSHERepair		=  gs.fSHERepair;	  
	fCOMRepair		=  gs.fCOMRepair;	  
	fDAMRepair		=  gs.fDAMRepair;	  
	fPANRepair		=  gs.fPANRepair;	  
	fMSGRepair		=  gs.fMSGRepair;	  
	fSRSRep			=	gs.fSRSRep;
	fLRSRep			=	gs.fLRSRep;
	fNAVRep			=	gs.fNAVRep;
	fPHORep			=	gs.fPHORep;
	fPHARep			=	gs.fPHARep;
	fSHERep			=	gs.fSHERep;
	fCOMRep			=	gs.fCOMRep;
	fDAMRep			=	gs.fDAMRep;
	fPANRep			=	gs.fPANRep;
	fMSGRep			=	gs.fMSGRep;
	fMissionL		=	gs.fMissionL;
	nEntQ				=	gs.nEntQ;
	nEntX				=	gs.nEntX;
	nEntY				=	gs.nEntY;
	bReRoute			=	gs.bReRoute;


	UpdateLRS(nEntQ);				 								 /* Mark LRS scan	*/
	CreateGame();
	ShowMessage(MG_RESUME);
	bNeedSave = FALSE;
	DisplayComMessage("\n>");
	ArrowCursorX();
	return 1;

} /* End LoadGame */







