#*************************************************************************
#*
#*  (c) 1992 Joe Jaworski
#*	 18405 Tamarind Street
#*  Fountain Valley, CA 92708
#*	 ALL RIGHTS RESERVED.
#*
#* Created and Written by Joe Jaworski
#* Microsoft C 6.00A, SDK 3.1
#*
#*
#* Revision:	$Revision:   1.4  $
#* Date:			$Date:   20 Feb 1993 09:27:18  $
#* Log File:	$Logfile:   C:/src/st/vcs/st.mav  $
#* Log:			$Log:   C:/src/st/vcs/st.mav  $
#* 
#*    Rev 1.4   20 Feb 1993 09:27:18   jaworski
#* Initial Revision
#*
#**************************************************************************

#============================================================
all: st.exe
DBUG = 0
NAME= st
INCL= st.h resource.h protos.h exclude.h
RES=  
OBJS = st.obj init.obj utils.obj loadsave.obj panel.obj btn.obj msg.obj SRS.obj LRS.obj PHO.obj PHA.obj NAV.obj SHE.obj DAM.obj COM.obj	sound.obj open.obj
LIBS= mlibjoes libw mlibcew commdlg mmsystem toolhelp win87em oldnames
#============================================================
st.obj:	     st.c	$(INCL)
init.obj:    init.c	$(INCL)
utils.obj:   utils.c	$(INCL)
loadsave.obj:loadsave.c	$(INCL)
panel.obj:   panel.c	$(INCL)
btn.obj:     btn.c	$(INCL)
msg.obj:     msg.c	$(INCL)
SRS.obj:     SRS.c	$(INCL)
LRS.obj:     LRS.c	$(INCL)
PHO.obj:     PHO.c	$(INCL)
PHA.obj:     PHA.c	$(INCL)
NAV.obj:     NAV.c	$(INCL)
SHE.obj:     SHE.c	$(INCL)
DAM.obj:     DAM.c	$(INCL)
COM.obj:     COM.c	$(INCL)
sound.obj:   sound.c	$(INCL)
open.obj:    open.c	$(INCL)
#============================================================



!IF $(DBUG)
CLF=-c -AM -Gsw2 -Od -Zdpi -FPi -W3 -WX -D DEBUG -D STRICT -NT _$*
LNK= /NOD /CO /align:16	/map /li
!ELSE
CLF=-c -AM -Gsw2 -O2 -Ob2 -Zp -FPi -W3 -WX -D STRICT -NT _$*
LNK= /NOD /align:16
!ENDIF


.SUFFIXES: .c .asm .rc .obj .res

.c.obj:
    cl $(CLF) $*.c 

$(NAME).res: $(NAME).rc $(INCL)	$(RES)
    rc -r $(NAME).rc


!IF $(DBUG)
$(NAME).exe: $(OBJS) $(NAME).def $(NAME).res
    link @<<$(NAME).lrf
$(LNK) $(OBJS), $(NAME).exe,$(NAME).map, $(LIBS), $(NAME).def
<<
    rc -t -31 $(NAME).res $(NAME).exe
    mapsym $(NAME)
    cvpack $(NAME).exe

!ELSE
$(NAME).exe: $(OBJS) $(NAME).def $(NAME).res 
    link @<<$(NAME).lrf
$(LNK) $(OBJS), $(NAME).exe,NUL, $(LIBS), $(NAME).def
<<
    rc -t -31 $(NAME).res $(NAME).exe
!ENDIF


