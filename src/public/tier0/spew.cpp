//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:  Re-implemented spew functions taken from 
// 			 ValveSoftware/GameNetworkingSockets
//
// $NoKeywords: $
//
//=============================================================================//

#include "dbg.h"

SpewRetval_t DefaultSpewFunc(SpewType_t type, tchar const* pMsg)
{
#if defined (POSIX) && !defined( _PS3 ) // No signals on PS3
	if (!s_bSetSigHandler)
	{
		signal(SIGTRAP, SIG_IGN);
		signal(SIGALRM, SIG_IGN);
		s_bSetSigHandler = true;
	}
#endif
#ifdef _PS3
	printf(_T("STEAMPS3 - %s"), pMsg);
#else
	printf(_T("%s"), pMsg);
#endif
	if (type == SPEW_ASSERT)
		return SPEW_DEBUGGER;
	else if (type == SPEW_ERROR)
		return SPEW_ABORT;
	else
		return SPEW_CONTINUE;
}

static SpewOutputFunc_t   s_SpewOutputFunc = DefaultSpewFunc;

static tchar const* s_pFileName;
static int			s_Line;
static SpewType_t	s_SpewType;

void   SpewOutputFunc(SpewOutputFunc_t func)
{
	s_SpewOutputFunc = func;
}

void _ExitOnFatalAssert(tchar const* pFile, int line, tchar const* pMessage)
{
	_SpewMessage(_T("Fatal assert failed: %s, line %d.  Application exiting.\n"), pFile, line);

#ifdef _WIN32
	TerminateProcess(GetCurrentProcess(), EXIT_FAILURE); // die, die RIGHT NOW! (don't call exit() so destructors will not get run)
#elif defined( _PS3 )
	sys_process_exit(EXIT_FAILURE);
#else
	_exit(EXIT_FAILURE);
#endif
}

//-----------------------------------------------------------------------------
// Spew functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Purpose: Lightly clean up a source path (skip to \src\ if we can)
//-----------------------------------------------------------------------------
static tchar const* CleanupAssertPath(tchar const* pFile)
{
#if defined(WIN32)
	for (tchar const* s = pFile; *s; ++s)
	{
		if (!strnicmp(s, _T("\\src\\"), 5))
		{
			return s;
		}
	}
#endif // no cleanup on other platforms

	return pFile;
}


void  _SpewInfo(SpewType_t type, tchar const* pFile, int line)
{
	//
	//	We want full(ish) paths, not just leaf names, for better diagnostics
	//
	s_pFileName = CleanupAssertPath(pFile);

	s_Line = line;
	s_SpewType = type;
}


SpewRetval_t  _SpewMessageType(SpewType_t spewType, tchar const* pMsgFormat, va_list args)
{
	//LOCAL_THREAD_LOCK();

#ifndef _XBOX
	tchar pTempBuffer[5020];
#else
	char pTempBuffer[1024];
#endif

	Assert(_tcslen(pMsgFormat) < sizeof(pTempBuffer)); // check that we won't artifically truncate the string

	/* Printf the file and line for warning + assert only... */
	int len = 0;
	if (spewType == SPEW_ASSERT)
	{
		len = _sntprintf(pTempBuffer, sizeof(pTempBuffer) - 1, _T("%s (%d) : "), s_pFileName, s_Line);
	}
	if (len == -1)
	{
		return SPEW_ABORT;
	}

	/* Create the message.... */
	int val = _vsntprintf(&pTempBuffer[len], sizeof(pTempBuffer) - len - 1, pMsgFormat, args);
	if (val == -1)
	{
		return SPEW_ABORT;
	}
	len += val;
	Assert(len * sizeof(*pMsgFormat) < sizeof(pTempBuffer)); /* use normal assert here; to avoid recursion. */

	// Add \n for warning and assert
	if (spewType == SPEW_ASSERT)
	{
		len += _stprintf(&pTempBuffer[len], _T("\n"));
#ifdef WIN32 
		OutputDebugString(pTempBuffer);
#endif
	}

	Assert((uint)len < sizeof(pTempBuffer) / sizeof(pTempBuffer[0]) - 1); /* use normal assert here; to avoid recursion. */
	Assert(s_SpewOutputFunc);

	/* direct it to the appropriate target(s) */
	SpewRetval_t ret = s_SpewOutputFunc(spewType, pTempBuffer);
	switch (ret)
	{
		// Asserts put the break into the macro so it occurs in the right place
	case SPEW_DEBUGGER:
		if (spewType != SPEW_ASSERT)
		{
			DebuggerBreak();
		}
		break;

	case SPEW_ABORT:
		//		MessageBox(NULL,"Error in _SpewMessage","Error",MB_OK);
				//DMsg( "console",  1, _T("Exiting on SPEW_ABORT\n") );
#ifdef _WIN32
		TerminateProcess(GetCurrentProcess(), EXIT_FAILURE); // die, die RIGHT NOW! (don't call exit() so destructors will not get run)
#elif defined( _PS3 )
		sys_process_exit(EXIT_FAILURE);
#else
		_exit(EXIT_FAILURE); // forcefully shutdown of the process without destructors running
#endif
	default:
		break;
	}

	return ret;
}

SpewRetval_t  _SpewMessage(tchar const* pMsgFormat, ...)
{
	va_list args;
	va_start(args, pMsgFormat);
	SpewRetval_t ret = _SpewMessageType(s_SpewType, pMsgFormat, args);
	va_end(args);
	return ret;
}