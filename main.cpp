#define UNICODE
#pragma comment(linker,"/opt:nowin98")
#include<winsock2.h>
#include<windows.h>
#pragma comment(lib,"ws2_32")

#define MACLEN (6)
#define PACKLEN (17*MACLEN)

TCHAR szClassName[]=TEXT("Window");

CHAR H2I(CHAR m)
{
	return m<='9'?m-'0':(m|32)-'a'+10;
}

BOOL CreateMagicPacket(LPCSTR lpszSource,LPBYTE lpszDest)
{
	int t=0,i;
	CHAR nibble1,nibble2;
	BOOL BroadCast=1;
	while(t<MACLEN)
	{
		lpszDest[t++]=(CHAR)0xFF;
	}
	for(i=0;t<MACLEN+MACLEN;++i)
	{
		nibble1=lpszSource[i];
		nibble2=lpszSource[++i];
		lpszDest[t++]=(H2I(nibble1)<<4)|H2I(nibble2);
	}
	for (;t<PACKLEN;++t)
	{
		lpszDest[t]=lpszDest[t-MACLEN];
	}
	return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		CreateWindow(TEXT("STATIC"),TEXT("MAC AddressF"),WS_VISIBLE|WS_CHILD,10,10,128,30,hWnd,(HMENU)100,((LPCREATESTRUCT)lParam)->hInstance,0);
		CreateWindowEx(WS_EX_CLIENTEDGE,TEXT("EDIT"),0,WS_VISIBLE|WS_CHILD,138,10,512,30,hWnd,(HMENU)101,((LPCREATESTRUCT)lParam)->hInstance,0);
		CreateWindow(TEXT("BUTTON"),TEXT("Create Magic Packet"),WS_VISIBLE|WS_CHILD,10,50,200,30,hWnd,(HMENU)102,((LPCREATESTRUCT)lParam)->hInstance,0);
		CreateWindow(TEXT("STATIC"),TEXT("Magic PacketF"),WS_VISIBLE|WS_CHILD,10,90,128,30,hWnd,(HMENU)103,((LPCREATESTRUCT)lParam)->hInstance,0);
		CreateWindowEx(WS_EX_CLIENTEDGE,TEXT("EDIT"),0,WS_VISIBLE|WS_CHILD|ES_READONLY|ES_MULTILINE,138,90,512,256,hWnd,(HMENU)104,((LPCREATESTRUCT)lParam)->hInstance,0);
		break;
	case WM_COMMAND:
		if(LOWORD(wParam)==102)
		{
			CHAR szTextMac[1024];
			BYTE byMagic[1024];
			CHAR szTextMagic[5*PACKLEN+1];
			GetDlgItemTextA(hWnd,101,szTextMac,1024);
			if( CreateMagicPacket(szTextMac,byMagic) )
			{
				CHAR szTemp[6];// 0:'0' 1:'x' 2:'F' 3:'F' 4:',' 5:'\0'
				szTextMagic[0]=0;
				for(int i=0;i<PACKLEN;i++)
				{
					wsprintfA(szTemp,"0x%02X,",byMagic[i]);
					lstrcatA(szTextMagic,szTemp);
				}
				SetDlgItemTextA(hWnd,104,szTextMagic);
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return 0;
}

EXTERN_C void __cdecl WinMainCRTStartup()
{
	MSG msg;
	HINSTANCE hInstance=GetModuleHandle(0);
	WNDCLASS wndclass={
		CS_HREDRAW|CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd=CreateWindow(
			szClassName,
			TEXT("Window"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			0,
			CW_USEDEFAULT,
			0,
			0,
			0,
			hInstance,
			0
		);
	ShowWindow(hWnd,SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while(GetMessage(&msg,0,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ExitProcess(msg.wParam);
}

#if _DEBUG
void main(){}
#endif
