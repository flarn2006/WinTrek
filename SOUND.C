/******************************************
 *		 		STAR TREK CLASSIC	 				
 *								  						
 * Sound playback routines				
 * Revision:	$Revision:   1.4  $
 * Date:			$Date:   20 Feb 1993 09:27:52  $
 * Log File:	$Logfile:   C:/src/st/vcs/sound.c_v  $
 * Log:			$Log:   C:/src/st/vcs/sound.c_v  $
 * 
 *    Rev 1.4   20 Feb 1993 09:27:52   jaworski
 * Initial Revision
 *
 *******************************************/

#define	SOUND_C
#include "exclude.h"

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <mmsystem.h>
#include "joes.h"
#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>

#include "st.h"			 								/* Global include files		*/	
#include "resource.h"
#include "protos.h"

#define RT_WAVE MAKEINTRESOURCE(WAVE)

static WORD 	wSupport;									 /* System supports sounds */
static WORD		wPlaying;							 		 /* ResID of Sound playing */
static LPSTR	lpBackSnd, lpSnd;
static HRSRC	hResBack, hResSnd;
static HGLOBAL	hMemBack, hMemSnd;



/***************************************************************************/

int SoundInit(void)

//	This function initializes global and local parms to support the sound
//	routines. The return value is non-zero if successfully initialized. The
//	return value is 0 if this computer doesn't support sounds.

/***************************************************************************/
{

if(!(wSupport = waveOutGetNumDevs()))
	return 0;							  			  	/* System can't play sounds */

/* Load up the background noise */

	hResBack = FindResource(hSnd, MAKEINTRESOURCE(SND_BACK), RT_WAVE);
	hMemBack = LoadResource(hSnd,	hResBack);
	lpBackSnd= (LPSTR)LockResource(hMemBack);
	wPlaying = 0;

	return wSupport;

} /* End SoundInit */




/***************************************************************************/

void SoundShutDown(void)

//	This function frees any sound memory currently allocated and stops all
// sounds from playing.

/***************************************************************************/
{

	KillSound();
	UnlockResource(hMemBack);
	FreeResource(hMemBack);
	wSupport = 0;

} /* End SoundShutDown */



/***************************************************************************/

void KillSound(void)

//	This function frees any sound memory currently allocated and stops all
// sounds from playing.

/***************************************************************************/
{

	if(wSupport)
		{
		sndPlaySound(NULL, 0);
		wPlaying = 0;

		if(lpSnd)
			{								 /* free general purpose, too	*/
		   UnlockResource(hMemSnd);
			FreeResource(hMemSnd);
			lpSnd = NULL;
			}
		}

} /* End KillSound */




/***************************************************************************/

void PlaySound(int nResID, BOOL bLoop)

//	This function stops any sound that may be playing and begins to
//	play the sound identified by nResID. If bLoop is TRUE, the sound is
//	played continuously until the KillSound() macro is called.

/***************************************************************************/
{
	WORD		wFlags;
	LPSTR		lpSnd2;
	HRSRC		hResSnd2;
	HGLOBAL	hMemSnd2;


	if(!nINISounds)
		return;

	if(wPlaying == (WORD)nResID)
		return;

	LockData(0);
	hResSnd2 = FindResource(hSnd, MAKEINTRESOURCE(nResID), RT_WAVE);
	hMemSnd2 = LoadResource(hSnd,	hResSnd2);
	lpSnd2= (LPSTR)LockResource(hMemSnd2);

	wFlags = SND_ASYNC | SND_MEMORY | SND_NODEFAULT;
	if(bLoop)
		wFlags |= SND_LOOP;

	if(lpSnd2)
		sndPlaySound(lpSnd2, wFlags);
	wPlaying = nResID;
	if(lpSnd)
		{
		UnlockResource(hMemSnd);
		FreeResource(hMemSnd);
		}
	hResSnd = hResSnd2;
	hMemSnd = hMemSnd2;
	lpSnd   = lpSnd2;
	UnlockData(0);




} /* End PlaySound */



/***************************************************************************/

void PlaySoundEx(int nResID, BOOL bLoop)

//	This function waits for any sound to stop playing, then begins to
//	play the sound identified by nResID. If bLoop is TRUE, the sound is
//	played continuously until the KillSound() macro is called.

/***************************************************************************/
{
	WORD		wFlags;

	LPSTR		lpSnd2;
	HRSRC		hResSnd2;
	HGLOBAL	hMemSnd2;
	DWORD		dwTick;

	if(!nINISounds)
		return;

	LockData(0);
	hResSnd2 = FindResource(hSnd, MAKEINTRESOURCE(nResID), RT_WAVE);
	hMemSnd2 = LoadResource(hSnd,	hResSnd2);
	lpSnd2= (LPSTR)LockResource(hMemSnd2);

	wFlags = SND_ASYNC | SND_MEMORY | SND_NODEFAULT | SND_NOSTOP;
	if(bLoop)
		wFlags |= SND_LOOP;

	dwTick == GetTickCountEx();		  						/* just a safety net */

	while(!sndPlaySound(lpSnd2, wFlags))
		{
		MyYield();
		if((GetTickCount() > dwTick + 20000L) || wPlaying == SND_BACK)
			{
			wFlags &= (WORD)(~SND_NOSTOP);
			sndPlaySound(lpSnd2, wFlags);
			if(lpSnd)
				{								 /* free general purpose, too	*/
		   	UnlockResource(hMemSnd);
				FreeResource(hMemSnd);
				}
			}
		}

	wPlaying = nResID;

	hResSnd = hResSnd2;
	hMemSnd = hMemSnd2;
	lpSnd   = lpSnd2;
	UnlockData(0);



} /* End PlaySoundEx */




/***************************************************************************/

int ResumeBackground(void)

//	This function resumes normal background sound playing.

/***************************************************************************/
{
	if(!nINISounds || wPlaying == SND_BACK)				/* already playing */
		return 0;

	if(Condition == NoAlert || Condition == GameOver || bPaused)
		return 0;

	if(sndPlaySound(lpBackSnd, SND_ASYNC | SND_MEMORY | SND_NODEFAULT |
		SND_NOSTOP | SND_LOOP))
		{
		wPlaying = SND_BACK;
		return 1;
		}
	return 0;

} /* End ResumeBackground */



