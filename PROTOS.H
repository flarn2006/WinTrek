/**************************************************************************
 *
 *  (c) 1992 Joe Jaworski
 *	 18405 Tamarind Street
 *  Fountain Valley, CA 92708
 *	 ALL RIGHTS RESERVED.
 *
 * Created and Written by Joe Jaworski
 * Microsoft C 6.00A, SDK 3.1
 *
 *
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:48  $
 * Log File:	$Logfile:   C:/src/st/vcs/protos.h_v  $
 * Log:			$Log:   C:/src/st/vcs/protos.h_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:48   jaworski
 * Initial Revision
 *
 **************************************************************************/




/*------------*/
/*   ST.C     */
/*------------*/

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow);
LRESULT CALLBACK _export FrameDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#ifdef ST_C
LRESULT CALLBACK _export ClientDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void LOCAL PaintMain(HWND hWnd);
void LOCAL SetDefPos(void);
void LOCAL SavePos(void);
void LOCAL RestorePos(void);
#endif



/*------------*/
/*   INIT.C   */
/*------------*/

void LoadINI(void);
void SaveINI(void);
void LoadUpStrings(void);
void SetupClasses(void);
void DestroyGame(void);
void CreateGame(void);
void StarFleetMemo(LPSTR lpszMemo);
void GameIsOver(void);
void LOCAL ShowScores(void);
void About(void);
LRESULT CALLBACK _export ScoresDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#ifdef INIT_C
LRESULT CALLBACK _export MemoDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int CALLBACK _export HiScoreProc(HWND hDlg, WORD message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK _export AboutDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif


/*------------*/
/*  UTILS.C   */
/*------------*/

LPSTR ResToString(int nResID);
POINT RandomXY(int nQuad, WORD wObj);
void  GetWindowParms(HWND hWnd, LPRECT lprc, LPWORD lpwStat);
void  SetWindowParms(HWND hWnd, LPRECT lprc, WORD wStat);
int   AdjQuad(int nQuad, int nXOffset, int nYOffset);
void  DrawNumber(HDC hDC, PSTR szNum, int nXPos, int nYPos, HBITMAP hDigBmp);
void  DrawCourseNeedle(HDC hDC, int nX, int nY, int nCourse, int nNewCourse);
POINT DegToCoords(int nDeg, int nRadius);
int   CoordsToDeg(POINT pt);
BOOL  ButtonStillDown(int nDelay);
void  MyYield(void);
void  Activate(HWND hWnd);
PSTR  CourseToAscii(PSTR szBuf, int nCourse);
void  Delay(DWORD dwDelay);
DWORD GetTickCountEx(void);
int   GetWarpFromPoints(int nXStart, int nYStart, int nXEnd, int nYEnd);
void  CALLBACK _export WFPProc(int nX, int nY, LPSTR lpInt);
int   FindAlien(int nQuad, int nX, int nY);
void  RepairShip(void);
void  PauseGame(void);
LRESULT CALLBACK _export PauseDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
PSTR SkillToAscii(int nSK);
BOOL BlockedByStar(int nXStart, int nYStart, LPINT lpnXEnd, LPINT lpnYEnd);
void CALLBACK _export BlockProc(int nX, int nY, LPSTR lp);



/*------------*/
/* LOADSAVE.C */
/*------------*/

int  AskToSave(void);
void NewGame(void);
int  SaveGame(void);
int  LoadGame(void);



/*------------*/
/*  PANEL.C   */
/*------------*/

LRESULT CALLBACK _export PanelDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
enum Alert SetCondition(void);
#ifdef PANEL_C
WORD CALLBACK _export FlashProc(HWND hWnd, WORD wMsg, int TimID, DWORD dwTim);
void LOCAL PaintPanel(HWND hWnd);
void LOCAL DrawCondition(HDC hDC);
#endif


/*------------*/
/*   MSG.C    */
/*------------*/

LRESULT CALLBACK _export MsgDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void LOCAL PaintMSG(HWND hWnd);
void ShowMessage(int nResID, ...);



/*------------*/
/*   BTN.C    */
/*------------*/

LRESULT CALLBACK _export BtnDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#ifdef BTN_C
void LOCAL PaintBtn(HWND hWnd);
void LOCAL DrawScrew(HDC hDC, int nX, int nY);
void LOCAL MoveButtons(HWND hWnd, int nWidth, int nHeight);
#endif


/*------------*/
/*   SRS.C    */
/*------------*/

LRESULT CALLBACK _export SRSDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ProcessRed(void);
void RandomDamage(int nHit);
#ifdef SRS_C
void LOCAL PaintSRS(HWND hWnd);
WORD CALLBACK _export AnimProc(HWND hWnd, WORD wMsg, int TimID, DWORD dwTim);
void LOCAL GetSectorRect(LPRECT lprc, int nX, int nY);
void LOCAL MoveEntImpulse(int nDeg, int nFactor);
void CALLBACK _export ImpMovProc(int nX, int nY, LPSTR lphDC);
void LOCAL MoveEntWarp(int nDeg, int nFactor);
void WarpToQuad(int nNewQuad);
void CALLBACK _export WarpMovProc(int nX, int nY, LPSTR lppt);
void LOCAL Stars(HDC hDC, int nDistance, LPRECT lprc);
void LOCAL InitStar(STARS *s);
void LOCAL MoveStar(HDC hDC, STARS *s, double v, int erase);
void LOCAL RicochetToQuad(void);
void LOCAL FirePhoton(int nDeg, int nFactor);
void CALLBACK _export PhoProc(int nX, int nY, LPSTR lphDC);
void LOCAL ExplodeSequence(HDC hDC, int nX, int nY, BOOL bDestroy);
void LOCAL FirePhasers(int nPower, BOOL bSingle);
void LOCAL AnimatePhasers(HDC hDC, int nXS, int nYS, int nXE, int nYE, DWORD dwTime, BOOL bAlien);
void LOCAL RandomAlienMove(int nA);
void LOCAL AlienFires(int nA);
HBITMAP LOCAL ObjToBitmap(int nObj);
#endif


/*------------*/
/*   LRS.C    */
/*------------*/

LRESULT CALLBACK _export LRSDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void UpdateLRS(int nQuad);
#ifdef LRS_C
WORD CALLBACK _export NumFlashProc(HWND hWnd, WORD wMsg, int TimID, DWORD dwTim);
void LOCAL PaintLRS(HWND hWnd);
#endif



/*------------*/
/*   NAV.C    */
/*------------*/

LRESULT CALLBACK _export NAVDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#ifdef NAV_C
void LOCAL PaintNAV(HWND hWnd);
int LOCAL NavKeyDown(WORD wKey);
#endif


/*------------*/
/*   PHO.C    */
/*------------*/

LRESULT CALLBACK _export PHODefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#ifdef PHO_C
void LOCAL PaintPHO(HWND hWnd);
#endif



/*------------*/
/*   PHA.C    */
/*------------*/

LRESULT CALLBACK _export PHADefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#ifdef PHA_C
void LOCAL PaintPHA(HWND hWnd);
void LOCAL DrawPhaThermo(HDC hDC);
void LOCAL SetLockedProc(void);
WORD CALLBACK _export LockedProc(HWND hWnd, WORD wMsg, int TimID, DWORD dwTim);WORD CALLBACK _export LockedProc(HWND hWnd, WORD wMsg, int TimID, DWORD dwTim);
#endif
void FreeLockedProc(void);



/*------------*/
/*   SHE.C    */
/*------------*/

LRESULT CALLBACK _export SHEDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#ifdef SHE_C
void LOCAL PaintSHE(HWND hWnd);
void LOCAL DrawThermo(HDC hDC);
void LOCAL FreeOvalProc(void);
void LOCAL SetOvalProc(void);
WORD CALLBACK _export OvalProc(HWND hWnd, WORD wMsg, int TimID, DWORD dwTim);
#endif


/*------------*/
/*   DAM.C    */
/*------------*/

LRESULT CALLBACK _export DAMDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#ifdef DAM_C
void LOCAL PaintDAM(HWND hWnd);
#endif


/*------------*/
/*   COM.C    */
/*------------*/

LRESULT CALLBACK _export COMDefProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DisplayComMessage(LPCSTR lpszCmd, ...);
#ifdef COM_C
void LOCAL ReAllocBuffer(HWND hWnd, int nNewX, int nNewY);
void LOCAL PaintCOM(HWND hWnd);
void LOCAL VirtualKeyStroke(HWND hWnd, int nChar);
void LOCAL CharKeyStroke(HWND hWnd, int nChar);
int  LOCAL ExecuteCommand(LPSTR lpCmd);
void LOCAL DisplayBaseInfo(int nQuad);
void LOCAL DisplayObjectInfo(int nY, int nX);
void LOCAL DoRicochet(void);
void LOCAL DamReport(void);
void LOCAL AnalysisReport(void);
#endif


/*------------*/
/*  SOUND.C   */
/*------------*/

int  SoundInit(void);
void SoundShutDown(void);
void KillSound(void);
void PlaySound(int nResID, BOOL bLoop);
void PlaySoundEx(int nResID, BOOL bLoop);
int  ResumeBackground(void);


/*------------*/
/*   OPEN.C   */
/*------------*/

void Opening(void);
#ifdef OPEN_C
WORD LOCAL OpenProc(HWND hWnd, HDC hDC);
#endif
