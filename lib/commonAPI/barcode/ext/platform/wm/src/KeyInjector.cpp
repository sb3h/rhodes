#pragma once
#include "KeyInjector.h"
#include "sipapi.h"
#include "keybd.h"

CKeyInjector::CKeyInjector()
{}

CKeyInjector::~CKeyInjector()
{}


// Original Author:		Dave Sheppard
// Original Date:		2/3/2005
void CKeyInjector::SendKey(TCHAR cKey)
{
	BOOL bEscape = FALSE;
	int nHex = 0;
	BYTE bHex = 0;
	BOOL bVirtual = FALSE;
	const BYTE	CharToVk[256] =
		{
				//     0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
				/*0*/ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x09,0x00,0x00,0x00,0x0D,0x00,0x00,
				/*1*/ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x00,
				/*2*/ 0x20,0x31,0xDE,0x33,0x34,0x35,0x37,0xDE,0x39,0x30,0x38,0xBB,0xBC,0xBD,0xBE,0xBF,
				/*3*/ 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0xBA,0xBA,0xBC,0xBB,0xBE,0xBF,
				/*4*/ 0x32,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
				/*5*/ 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0xDB,0x5C,0xDD,0x36,0xBD,
				/*6*/ 0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
				/*7*/ 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0xDB,0x5C,0xDD,0x60,0x60,
				/*8*/ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				/*9*/ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				/*A*/ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				/*B*/ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				/*C*/ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				/*D*/ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				/*E*/ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				/*F*/ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				};

	//  Legacy
	bool l_bEscape = FALSE;
	if ( l_bEscape )
	{
		if ( cKey == ESCAPE_CHAR )
		{
			bEscape = !bEscape;

			if ( bEscape )
			{
				return;
			}
		}
	}
	
	if ( bEscape )
	{
		bEscape = FALSE;
		switch(cKey)
		{
			case TEXT('b'):
				cKey = TEXT('\b');
				break;
			case TEXT('f'):
				cKey = TEXT('\f');
				break;
			case TEXT('n'):
				cKey = TEXT('\n');;
				break;
			case TEXT('r'):
				cKey = TEXT('\r');;
				break;
			case TEXT('t'):
				cKey = TEXT('\t');
				break;
			case TEXT('x'):
				bVirtual = FALSE;
				nHex = 1;
				bHex = 0;
				return;
			case TEXT('v'):
				bVirtual = TRUE;
				nHex = 1;
				bHex = 0;
				return;
		}
	}

	switch(nHex)
	{
		case 1:
		case 2:
			if ( iswxdigit(cKey) )
			{
				bHex *= 16;
				if ( iswdigit(cKey) )
				{
					bHex += (BYTE) ( cKey & 0xF );
				}
				else
				{
					bHex += (BYTE)( ( cKey &0xF ) + 9 );
				}

				if ( nHex == 1 )
				{
					nHex++;
					return;
				}
				else
				{
					nHex = 0;
					cKey = (TCHAR)bHex;
					break;
				}
			}

			nHex = 0;
			bVirtual = FALSE;
			return;
		default:
			break;
	}
	
	if ( bVirtual )
	{
		UINT State1[1] = {KeyStateDownFlag|KeyShiftNoCharacterFlag};
		UINT State2[1] = {KeyStateToggledFlag|KeyShiftNoCharacterFlag};
		UINT Shift1[1] = {KeyStateDownFlag|KeyShiftNoCharacterFlag};
		UINT Shift2[1] = {KeyShiftNoCharacterFlag};
		UINT Key1[1] = {0};
		UINT Key2[1] = {0};
		BYTE bVK;
		bVK = (BYTE)cKey;

		PostKeybdMessage((HWND) -1, bVK, State1[0], 0, Shift1, Key1);
		PostKeybdMessage((HWND) -1, bVK, State2[0], 0, Shift2, Key2);
	}
	else
	{
		UINT State1[1] = {KeyStateDownFlag};
		UINT State2[1] = {KeyStateToggledFlag|KeyShiftNoCharacterFlag};
		UINT Shift1[1] = {KeyStateDownFlag};
		UINT Shift2[1] = {KeyShiftNoCharacterFlag};
		UINT Key1[1] = {cKey};
		UINT Key2[1] = {0};
		BYTE bVK;
	
		bVK = CharToVk[(BYTE)cKey];

		PostKeybdMessage((HWND) -1, bVK, State1[0], 1, Shift1, Key1);
		PostKeybdMessage((HWND) -1, bVK, State2[0], 1, Shift2, Key2);
	}

	bVirtual = FALSE;
}

// Original Author:		Dave Sheppard
// Original Date:		2/3/2005
void CKeyInjector::SendString(LPCTSTR lpszString)
{
	DWORD i;

	HWND hWndFocus = GetForegroundWindow();

	if ( lpszString == NULL )
	{
		return;
	}
	
	for(i=0;i<wcslen(lpszString);i++)
	{
		SendKey(lpszString[i]);
	}
}

