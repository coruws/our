SRC=main.cpp
NAME=example
OUT=$(NAME).exe
DEBUG=debug
RELEASE=release
LIBS=ntdll.lib kernel32.lib advapi32.lib user32.lib

b_debug:$(DEBUG)
	@cl $(SRC) /Fo$(DEBUG)/ /Fd$(DEBUG)/vc.pdb /c /GS- /Gz /TP /ZI /nologo /W0 /D _DEBUG
    @link $(DEBUG)/*.obj /OUT:$(DEBUG)/$(OUT) /PDB:$(DEBUG)/$(NAME).pdb /LTCG /NOLOGO /NODEFAULTLIB /DEBUG /SUBSYSTEM:WINDOWS,5.01 /ENTRY:"entry" /DYNAMICBASE /FIXED:No $(LIBS)
	@del $(DEBUG)\*.obj
	@del $(DEBUG)\*.idb
$(DEBUG):
	@mkdir $(DEBUG)	
b_release:$(RELEASE)
	@cl $(SRC) /Fo$(RELEASE)/ /Fd$(RELEASE)/vc.pdb /c /GL /Gy /GS- /Gz /TP /Od /W0 /nologo
	@link $(RELEASE)/*.obj /OUT:$(RELEASE)/$(OUT) /LTCG /NOLOGO /NODEFAULTLIB /SUBSYSTEM:WINDOWS,5.01 /ENTRY:"entry" /DYNAMICBASE /FIXED:No $(LIBS)
	@del $(RELEASE)\*.obj
$(RELEASE):
	@mkdir $(RELEASE)