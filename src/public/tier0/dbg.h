//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ========//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//
#ifndef DBG_H
#define DBG_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/basetypes.h"
#include "dbgflag.h"
#include "logging.h"
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef POSIX
#define __cdecl
#endif

//-----------------------------------------------------------------------------
// dll export stuff
//-----------------------------------------------------------------------------

class Color;


//-----------------------------------------------------------------------------
// Usage model for the Dbg library
//
// 1. Assertions.
// 
//   Assertions are used to detect and warn about invalid states
//   
//   To use an assertion, use
//
//   Assert( (f == 5) );
//   AssertMsg( (f == 5), ("F needs to be %d here!\n", 5) );
//   AssertFunc( (f == 5), BadFunc() );
//   AssertEquals( f, 5 );
//   AssertFloatEquals( f, 5.0f, 1e-3 );
//
//   The first will simply report that an assertion failed on a particular
//   code file and line. The second version will display a print-f formatted message 
//   along with the file and line, the third will display a generic message and
//   will also cause the function BadFunc to be executed, and the last two
//   will report an error if f is not equal to 5 (the last one asserts within
//   a particular tolerance).
//
// 2. Code activation
//
//   To cause code to be run only in debug builds, use DBG_CODE:
//   An example is below.
//
//   DBG_CODE(
//				{
//					int x = 5;
//					++x;
//				}
//           ); 
//
//   Code can be activated based on the dynamic spew groups also. Use
//  
//   DBG_DCODE( "group", level,
//              { int x = 5; ++x; }
//            );
//
// 3. Breaking into the debugger.
//
//   To cause an unconditional break into the debugger in debug builds only, use DBG_BREAK
//
//   DBG_BREAK();
//
//	 You can force a break in any build (release or debug) using
//
//	 DebuggerBreak();
//-----------------------------------------------------------------------------

PLATFORM_INTERFACE void _ExitOnFatalAssert( const tchar* pFile, int line );
PLATFORM_INTERFACE bool ShouldUseNewAssertDialog();

PLATFORM_INTERFACE bool SetupWin32ConsoleIO();

// Returns true if they want to break in the debugger.
PLATFORM_INTERFACE bool DoNewAssertDialog( const tchar *pFile, int line, const tchar *pExpression );

/* Used to define macros, never use these directly. */

#define  _AssertMsg( _exp, _msg, _executeExp, _bFatal )	\
	do {																\
		if (!(_exp)) 													\
		{ 																\
			LoggingResponse_t ret = Log_Assert( "%s (%d) : %s\n", __TFILE__, __LINE__, _msg );	\
			_executeExp; 												\
			if ( ret == LR_DEBUGGER )									\
			{															\
				if ( !ShouldUseNewAssertDialog() || DoNewAssertDialog( __TFILE__, __LINE__, _msg ) ) \
					DebuggerBreak();									\
				if ( _bFatal )											\
					_ExitOnFatalAssert( __TFILE__, __LINE__ );			\
			}															\
		}																\
	} while (0)

#define  _AssertMsgOnce( _exp, _msg, _bFatal ) \
	do {																\
		static bool fAsserted;											\
		if (!fAsserted )												\
		{ 																\
			_AssertMsg( _exp, _msg, (fAsserted = true), _bFatal );		\
		}																\
	} while (0)

/* Spew macros... */

// AssertFatal macros
// AssertFatal is used to detect an unrecoverable error condition.
// If enabled, it may display an assert dialog (if DBGFLAG_ASSERTDLG is turned on or running under the debugger),
// and always terminates the application

#ifdef DBGFLAG_ASSERTFATAL

#define  AssertFatal( _exp )									_AssertMsg( _exp, _T("Assertion Failed: ") _T(#_exp), ((void)0), true )
#define  AssertFatalOnce( _exp )								_AssertMsgOnce( _exp, _T("Assertion Failed: ") _T(#_exp), true )
#define  AssertFatalMsg( _exp, _msg )							_AssertMsg( _exp, _msg, ((void)0), true )
#define  AssertFatalMsgOnce( _exp, _msg )						_AssertMsgOnce( _exp, _msg, true )
#define  AssertFatalFunc( _exp, _f )							_AssertMsg( _exp, _T("Assertion Failed: " _T(#_exp), _f, true )
#define  AssertFatalEquals( _exp, _expectedValue )				AssertFatalMsg2( (_exp) == (_expectedValue), _T("Expected %d but got %d!"), (_expectedValue), (_exp) ) 
#define  AssertFatalFloatEquals( _exp, _expectedValue, _tol )   AssertFatalMsg2( fabs((_exp) - (_expectedValue)) <= (_tol), _T("Expected %f but got %f!"), (_expectedValue), (_exp) )
#define  VerifyFatal( _exp )									AssertFatal( _exp )
#define  VerifyEqualsFatal( _exp, _expectedValue )				AssertFatalEquals( _exp, _expectedValue )

#define  AssertFatalMsg1( _exp, _msg, a1 )									AssertFatalMsg( _exp, (const tchar *)(CDbgFmtMsg( _msg, a1 )))
#define  AssertFatalMsg2( _exp, _msg, a1, a2 )								AssertFatalMsg( _exp, (const tchar *)(CDbgFmtMsg( _msg, a1, a2 )))
#define  AssertFatalMsg3( _exp, _msg, a1, a2, a3 )							AssertFatalMsg( _exp, (const tchar *)(CDbgFmtMsg( _msg, a1, a2, a3 )))
#define  AssertFatalMsg4( _exp, _msg, a1, a2, a3, a4 )						AssertFatalMsg( _exp, (const tchar *)(CDbgFmtMsg( _msg, a1, a2, a3, a4 )))
#define  AssertFatalMsg5( _exp, _msg, a1, a2, a3, a4, a5 )					AssertFatalMsg( _exp, (const tchar *)(CDbgFmtMsg( _msg, a1, a2, a3, a4, a5 )))
#define  AssertFatalMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				AssertFatalMsg( _exp, (const tchar *)(CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6 )))
#define  AssertFatalMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				AssertFatalMsg( _exp, (const tchar *)(CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6 )))
#define  AssertFatalMsg7( _exp, _msg, a1, a2, a3, a4, a5, a6, a7 )			AssertFatalMsg( _exp, (const tchar *)(CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6, a7 )))
#define  AssertFatalMsg8( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8 )		AssertFatalMsg( _exp, (const tchar *)(CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6, a7, a8 )))
#define  AssertFatalMsg9( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8, a9 )	AssertFatalMsg( _exp, (const tchar *)(CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6, a7, a8, a9 )))

#else // DBGFLAG_ASSERTFATAL

#define  AssertFatal( _exp )									((void)0)
#define  AssertFatalOnce( _exp )								((void)0)
#define  AssertFatalMsg( _exp, _msg )							((void)0)
#define  AssertFatalMsgOnce( _exp, _msg )						((void)0)
#define  AssertFatalFunc( _exp, _f )							((void)0)
#define  AssertFatalEquals( _exp, _expectedValue )				((void)0)
#define  AssertFatalFloatEquals( _exp, _expectedValue, _tol )	((void)0)
#define  VerifyFatal( _exp )									(_exp)
#define  VerifyEqualsFatal( _exp, _expectedValue )				(_exp)

#define  AssertFatalMsg1( _exp, _msg, a1 )									((void)0)
#define  AssertFatalMsg2( _exp, _msg, a1, a2 )								((void)0)
#define  AssertFatalMsg3( _exp, _msg, a1, a2, a3 )							((void)0)
#define  AssertFatalMsg4( _exp, _msg, a1, a2, a3, a4 )						((void)0)
#define  AssertFatalMsg5( _exp, _msg, a1, a2, a3, a4, a5 )					((void)0)
#define  AssertFatalMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				((void)0)
#define  AssertFatalMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				((void)0)
#define  AssertFatalMsg7( _exp, _msg, a1, a2, a3, a4, a5, a6, a7 )			((void)0)
#define  AssertFatalMsg8( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8 )		((void)0)
#define  AssertFatalMsg9( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8, a9 )	((void)0)

#endif // DBGFLAG_ASSERTFATAL

// Assert macros
// Assert is used to detect an important but survivable error.
// It's only turned on when DBGFLAG_ASSERT is true.

#ifdef DBGFLAG_ASSERT

#define  Assert( _exp )           							_AssertMsg( _exp, _T("Assertion Failed: ") _T(#_exp), ((void)0), false )
#define  AssertAligned( adr )                               Assert( ( ( ( intp ) ( adr ) ) & 0xf ) == 0 )
#define  AssertMsg_( _exp, _msg )  							_AssertMsg( _exp, _msg, ((void)0), false )
#define  AssertOnce( _exp )       							_AssertMsgOnce( _exp, _T("Assertion Failed: ") _T(#_exp), false )
#define  AssertMsgOnce( _exp, _msg )  						_AssertMsgOnce( _exp, _msg, false )
#define  AssertFunc( _exp, _f )   							_AssertMsg( _exp, _T("Assertion Failed: ") _T(#_exp), _f, false )
#define  AssertEquals( _exp, _expectedValue )              	AssertMsg2( (_exp) == (_expectedValue), _T("Expected %d but got %d!"), (_expectedValue), (_exp) ) 
#define  AssertFloatEquals( _exp, _expectedValue, _tol )  	AssertMsg2( fabs((_exp) - (_expectedValue)) <= (_tol), _T("Expected %f but got %f!"), (_expectedValue), (_exp) )
#define  Verify( _exp )           							Assert( _exp )
#define  VerifyEquals( _exp, _expectedValue )           	AssertEquals( _exp, _expectedValue )

#define  AssertMsg(  _exp, _msg )  										AssertMsg_( _exp, _T( _msg ) )
#define  AssertMsg1( _exp, _msg, a1 )									AssertMsg_( _exp, (const tchar *)(CDbgFmtMsg( _T( _msg ), a1 )) )
#define  AssertMsg2( _exp, _msg, a1, a2 )								AssertMsg_( _exp, (const tchar *)(CDbgFmtMsg( _T( _msg ), a1, a2 )) )
#define  AssertMsg3( _exp, _msg, a1, a2, a3 )							AssertMsg_( _exp, (const tchar *)(CDbgFmtMsg( _T( _msg ), a1, a2, a3 )) )
#define  AssertMsg4( _exp, _msg, a1, a2, a3, a4 )						AssertMsg_( _exp, (const tchar *)(CDbgFmtMsg( _T( _msg ), a1, a2, a3, a4 )) )
#define  AssertMsg5( _exp, _msg, a1, a2, a3, a4, a5 )					AssertMsg_( _exp, (const tchar *)(CDbgFmtMsg( _T( _msg ), a1, a2, a3, a4, a5 )) )
#define  AssertMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				AssertMsg_( _exp, (const tchar *)(CDbgFmtMsg( _T( _msg ), a1, a2, a3, a4, a5, a6 )) )
#define  AssertMsg7( _exp, _msg, a1, a2, a3, a4, a5, a6, a7 )			AssertMsg_( _exp, (const tchar *)(CDbgFmtMsg( _T( _msg ), a1, a2, a3, a4, a5, a6, a7 )) )
#define  AssertMsg8( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8 )		AssertMsg_( _exp, (const tchar *)(CDbgFmtMsg( _T( _msg ), a1, a2, a3, a4, a5, a6, a7, a8 )) )
#define  AssertMsg9( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8, a9 )	AssertMsg_( _exp, (const tchar *)(CDbgFmtMsg( _T( _msg ), a1, a2, a3, a4, a5, a6, a7, a8, a9 )) )

#else // DBGFLAG_ASSERT

#define  Assert( _exp )										((void)0)
#define  AssertAligned( ptr )								((void)0)
#define  AssertOnce( _exp )									((void)0)
#define  AssertMsg( _exp, _msg )							((void)0)
#define  AssertMsgOnce( _exp, _msg )						((void)0)
#define  AssertFunc( _exp, _f )								((void)0)
#define  AssertEquals( _exp, _expectedValue )				((void)0)
#define  AssertFloatEquals( _exp, _expectedValue, _tol )	((void)0)
#define  Verify( _exp )										(_exp)
#define  VerifyEquals( _exp, _expectedValue )           	(_exp)

#define  AssertMsg1( _exp, _msg, a1 )									((void)0)
#define  AssertMsg2( _exp, _msg, a1, a2 )								((void)0)
#define  AssertMsg3( _exp, _msg, a1, a2, a3 )							((void)0)
#define  AssertMsg4( _exp, _msg, a1, a2, a3, a4 )						((void)0)
#define  AssertMsg5( _exp, _msg, a1, a2, a3, a4, a5 )					((void)0)
#define  AssertMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				((void)0)
#define  AssertMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				((void)0)
#define  AssertMsg7( _exp, _msg, a1, a2, a3, a4, a5, a6, a7 )			((void)0)
#define  AssertMsg8( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8 )		((void)0)
#define  AssertMsg9( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8, a9 )	((void)0)

#endif // DBGFLAG_ASSERT

#define FILE_LINE_FUNCTION_STRINGIFY(x) #x
#define FILE_LINE_FUNCTION_TOSTRING(x) FILE_LINE_FUNCTION_STRINGIFY(x)
#define FILE_LINE_FUNCTION_STRING __FILE__ "(" FILE_LINE_FUNCTION_TOSTRING(__LINE__) "):" __FUNCTION__ ":"

#define FILE_LINE_STRINGIFY(x) #x
#define FILE_LINE_TOSTRING(x) FILE_LINE_STRINGIFY(x)
#define FILE_LINE_STRING __FILE__ "(" FILE_LINE_TOSTRING(__LINE__) "):"

#define FUNCTION_LINE_STRINGIFY(x) #x
#define FUNCTION_LINE_TOSTRING(x) FUNCTION_LINE_STRINGIFY(x)
#define FUNCTION_LINE_STRING __FUNCTION__ "(" FUNCTION_LINE_TOSTRING(__LINE__) "): "

//////////////////////////////////////////////////////////////////////////
// Legacy Logging System
//////////////////////////////////////////////////////////////////////////

// Channels which map the legacy logging system to the new system.

// Channel for all default Msg/Warning/Error commands.
DECLARE_LOGGING_CHANNEL( LOG_GENERAL );
// Channel for all asserts.
DECLARE_LOGGING_CHANNEL( LOG_ASSERT );
// Channel for all ConMsg and ConColorMsg commands.
DECLARE_LOGGING_CHANNEL( LOG_CONSOLE );
// Channel for all DevMsg and DevWarning commands with level < 2.
DECLARE_LOGGING_CHANNEL( LOG_DEVELOPER );
// Channel for ConDMsg commands.
DECLARE_LOGGING_CHANNEL( LOG_DEVELOPER_CONSOLE );
// Channel for all DevMsg and DevWarning commands with level >= 2.
DECLARE_LOGGING_CHANNEL( LOG_DEVELOPER_VERBOSE );

// Legacy logging functions

PLATFORM_INTERFACE void Msg( const tchar* pMsg, ... );
PLATFORM_INTERFACE void Warning( const tchar *pMsg, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_INTERFACE void Warning_SpewCallStack( int iMaxCallStackLength, const tchar *pMsg, ... ) FMTFUNCTION( 2, 3 );
PLATFORM_INTERFACE void Error( const tchar *pMsg, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_INTERFACE void Error_SpewCallStack( int iMaxCallStackLength, const tchar *pMsg, ... ) FMTFUNCTION( 2, 3 );

// @TODO: these callstack spew functions are currently disabled in the new logging system.  Need to add support for these if desired.
PLATFORM_INTERFACE void _Warning_AlwaysSpewCallStack_Enable( bool bEnable );
PLATFORM_INTERFACE void _Warning_AlwaysSpewCallStack_Length( int iMaxCallStackLength );

PLATFORM_INTERFACE void _Error_AlwaysSpewCallStack_Enable( bool bEnable );
PLATFORM_INTERFACE void _Error_AlwaysSpewCallStack_Length( int iMaxCallStackLength );

PLATFORM_INTERFACE void DevMsg( int level, const tchar* pMsg, ... ) FMTFUNCTION( 2, 3 );
PLATFORM_INTERFACE void DevWarning( int level, const tchar *pMsg, ... ) FMTFUNCTION( 2, 3 );

PLATFORM_OVERLOAD void DevMsg( const tchar* pMsg, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_OVERLOAD void DevWarning( const tchar *pMsg, ... ) FMTFUNCTION( 1, 2 );

PLATFORM_OVERLOAD void ConColorMsg( const Color& clr, const tchar* pMsg, ... ) FMTFUNCTION( 2, 3 );
PLATFORM_OVERLOAD void ConMsg( const tchar* pMsg, ... ) FMTFUNCTION( 1, 2 );

PLATFORM_INTERFACE void ConDMsg( const tchar* pMsg, ... ) FMTFUNCTION( 1, 2 );

// You can use this macro like a runtime assert macro.
// If the condition fails, then Error is called with the message. This macro is called
// like AssertMsg, where msg must be enclosed in parenthesis:
//
// ErrorIfNot( bCondition, ("a b c %d %d %d", 1, 2, 3) );
#define ErrorIfNot( condition, msg ) \
	if ( (condition) )		\
		;					\
	else 					\
	{						\
		Error msg;			\
	}

#ifdef _DEBUG
#define DebugMsg(...)  DevMsg(__VA_ARGS__)
#else
#define DebugMsg(...)
#endif

PLATFORM_INTERFACE void COM_TimestampedLog( char const *fmt, ... ) FMTFUNCTION( 1, 2 );

/* Code macros, debugger interface */

#ifdef _DEBUG

#define DBG_CODE( _code )            if (0) ; else { _code }
#define DBG_CODE_NOSCOPE( _code )	 _code
#define DBG_DCODE( _g, _l, _code )   if (IsSpewActive( _g, _l )) { _code } else {}
#define DBG_BREAK()                  DebuggerBreak()	/* defined in platform.h */ 

#else /* not _DEBUG */

#define DBG_CODE( _code )            ((void)0)
#define DBG_CODE_NOSCOPE( _code )	 
#define DBG_DCODE( _g, _l, _code )   ((void)0)
#define DBG_BREAK()                  ((void)0)

#endif /* _DEBUG */

//-----------------------------------------------------------------------------
// Macro to assist in asserting constant invariants during compilation

#ifdef _DEBUG
#define COMPILE_TIME_ASSERT( pred )	switch(0){case 0:case pred:;}
#define ASSERT_INVARIANT( pred )	static void UNIQUE_ID() { COMPILE_TIME_ASSERT( pred ) }
#else
#define COMPILE_TIME_ASSERT( pred )
#define ASSERT_INVARIANT( pred )
#endif

#ifdef _DEBUG
template<typename DEST_POINTER_TYPE, typename SOURCE_POINTER_TYPE>
inline DEST_POINTER_TYPE assert_cast(SOURCE_POINTER_TYPE* pSource)
{
    Assert( static_cast<DEST_POINTER_TYPE>(pSource) == dynamic_cast<DEST_POINTER_TYPE>(pSource) );
    return static_cast<DEST_POINTER_TYPE>(pSource);
}
#else
#define assert_cast static_cast
#endif

//-----------------------------------------------------------------------------
// Templates to assist in validating pointers:

// Have to use these stubs so we don't have to include windows.h here.
PLATFORM_INTERFACE void _AssertValidReadPtr( void* ptr, int count = 1 );
PLATFORM_INTERFACE void _AssertValidWritePtr( void* ptr, int count = 1 );
PLATFORM_INTERFACE void _AssertValidReadWritePtr( void* ptr, int count = 1 );

PLATFORM_INTERFACE  void AssertValidStringPtr( const tchar* ptr, int maxchar = 0xFFFFFF );
template<class T> inline void AssertValidReadPtr( T* ptr, int count = 1 )		     { _AssertValidReadPtr( (void*)ptr, count ); }
template<class T> inline void AssertValidWritePtr( T* ptr, int count = 1 )		     { _AssertValidWritePtr( (void*)ptr, count ); }
template<class T> inline void AssertValidReadWritePtr( T* ptr, int count = 1 )	     { _AssertValidReadWritePtr( (void*)ptr, count ); }

#define AssertValidThis() AssertValidReadWritePtr(this,sizeof(*this))

//-----------------------------------------------------------------------------
// Macro to protect functions that are not reentrant

#ifdef _DEBUG
class CReentryGuard
{
public:
	CReentryGuard(int *pSemaphore)
	 : m_pSemaphore(pSemaphore)
	{
		++(*m_pSemaphore);
	}
	
	~CReentryGuard()
	{
		--(*m_pSemaphore);
	}
	
private:
	int *m_pSemaphore;
};

#define ASSERT_NO_REENTRY() \
	static int fSemaphore##__LINE__; \
	Assert( !fSemaphore##__LINE__ ); \
	CReentryGuard ReentryGuard##__LINE__( &fSemaphore##__LINE__ )
#else
#define ASSERT_NO_REENTRY()
#endif

// Tier0 uses these for string functions since this abstraction is normally done in tier1.
#ifdef POSIX
	#define Tier0Internal_sntprintf  snprintf
	#define Tier0Internal_vsntprintf vsnprintf
	#define Tier0Internal_vsnprintf  vsnprintf
#else
	#define Tier0Internal_sntprintf  _sntprintf
	#define Tier0Internal_vsntprintf _vsntprintf
	#define Tier0Internal_vsnprintf  _vsnprintf
#endif

//-----------------------------------------------------------------------------
//
// Purpose: Inline string formatter
//

#include "tier0/valve_off.h"
class CDbgFmtMsg
{
public:
	CDbgFmtMsg(const tchar *pszFormat, ...)
	{ 
		va_list arg_ptr;

		va_start(arg_ptr, pszFormat);
		Tier0Internal_vsntprintf(m_szBuf, sizeof(m_szBuf)-1, pszFormat, arg_ptr);
		va_end(arg_ptr);

		m_szBuf[sizeof(m_szBuf)-1] = 0;
	}

	operator const tchar *() const				
	{ 
		return m_szBuf; 
	}

private:
	tchar m_szBuf[256];
};
#include "tier0/valve_on.h"

//-----------------------------------------------------------------------------
//
// Purpose: Embed debug info in each file.
//
#if defined( _WIN32 ) && !defined( _X360 )

	#ifdef _DEBUG
		#pragma comment(compiler)
	#endif

#endif

//-----------------------------------------------------------------------------
//
// Purpose: Wrap around a variable to create a simple place to put a breakpoint
//

#ifdef _DEBUG

template< class Type >
class CDataWatcher
{
public:
	const Type& operator=( const Type &val ) 
	{ 
		return Set( val ); 
	}
	
	const Type& operator=( const CDataWatcher<Type> &val ) 
	{ 
		return Set( val.m_Value ); 
	}
	
	const Type& Set( const Type &val )
	{
		// Put your breakpoint here
		m_Value = val;
		return m_Value;
	}
	
	Type& GetForModify()
	{
		return m_Value;
	}
	
	const Type& operator+=( const Type &val ) 
	{
		return Set( m_Value + val ); 
	}
	
	const Type& operator-=( const Type &val ) 
	{
		return Set( m_Value - val ); 
	}
	
	const Type& operator/=( const Type &val ) 
	{
		return Set( m_Value / val ); 
	}
	
	const Type& operator*=( const Type &val ) 
	{
		return Set( m_Value * val ); 
	}
	
	const Type& operator^=( const Type &val ) 
	{
		return Set( m_Value ^ val ); 
	}
	
	const Type& operator|=( const Type &val ) 
	{
		return Set( m_Value | val ); 
	}
	
	const Type& operator++()
	{
		return (*this += 1);
	}
	
	Type operator--()
	{
		return (*this -= 1);
	}
	
	Type operator++( int ) // postfix version..
	{
		Type val = m_Value;
		(*this += 1);
		return val;
	}
	
	Type operator--( int ) // postfix version..
	{
		Type val = m_Value;
		(*this -= 1);
		return val;
	}
	
	// For some reason the compiler only generates type conversion warnings for this operator when used like 
	// CNetworkVarBase<unsigned tchar> = 0x1
	// (it warns about converting from an int to an unsigned char).
	template< class C >
	const Type& operator&=( C val ) 
	{ 
		return Set( m_Value & val ); 
	}
	
	operator const Type&() const 
	{
		return m_Value; 
	}
	
	const Type& Get() const 
	{
		return m_Value; 
	}
	
	const Type* operator->() const 
	{
		return &m_Value; 
	}
	
	Type m_Value;
	
};

#else

template< class Type >
class CDataWatcher
{
private:
	CDataWatcher(); // refuse to compile in non-debug builds
};

#endif

// Code for programmatically setting/unsetting hardware breakpoints (there's probably a 360 and
#ifdef IS_WINDOWS_PC

typedef void * HardwareBreakpointHandle_t;

enum EHardwareBreakpointType
{
	BREAKPOINT_EXECUTE = 0,
	BREAKPOINT_WRITE,
	BREAKPOINT_READWRITE,
};

enum EHardwareBreakpointSize
{
	BREAKPOINT_SIZE_1 = 1,
	BREAKPOINT_SIZE_2 = 2,
	BREAKPOINT_SIZE_4 = 4,
	BREAKPOINT_SIZE_8 = 8,
};

PLATFORM_INTERFACE HardwareBreakpointHandle_t SetHardwareBreakpoint( EHardwareBreakpointType eType, EHardwareBreakpointSize eSize, const void *pvLocation );
PLATFORM_INTERFACE bool ClearHardwareBreakpoint( HardwareBreakpointHandle_t handle );

class CHardwareBreakPointScopeGuard
{
public:
	CHardwareBreakPointScopeGuard( const void *pvLocation, size_t nLocationSize, EHardwareBreakpointType eType = BREAKPOINT_WRITE )
	{
		EHardwareBreakpointSize eSize = BREAKPOINT_SIZE_4;
		switch ( nLocationSize )
		{
		case 1:
			eSize = BREAKPOINT_SIZE_1;
			break;
		case 2:
			eSize = BREAKPOINT_SIZE_2;
			break;
		case 4:
			eSize = BREAKPOINT_SIZE_4;
			break;
		case 8:
			eSize = BREAKPOINT_SIZE_8;
			break;
		default:
			Warning( _T( "SetHardwareBreakpoint can only work with 1, 2, 4 or 8 byte data fields." ) );
			break;
		}

		m_hBreakPoint = SetHardwareBreakpoint( eType, eSize, pvLocation );
		m_bActive = m_hBreakPoint != (HardwareBreakpointHandle_t)0;
	}

	~CHardwareBreakPointScopeGuard()
	{
		Release();
	}

	void Release()
	{
		if ( !m_bActive )
			return;
		ClearHardwareBreakpoint( m_hBreakPoint );
	}

private:
	bool						m_bActive;
	HardwareBreakpointHandle_t	m_hBreakPoint;
};

#endif // IS_WINDOWS_PC
//-----------------------------------------------------------------------------

#ifdef _WIN32
//-----------------------------------------------------------------------------
// Re-implemented spew functions taken from ValveSoftware/GameNetworkingSockets
//-----------------------------------------------------------------------------

#include <windows.h>

/* Various types of spew messages */
// I'm sure you're asking yourself why SPEW_ instead of DBG_ ?
// It's because DBG_ is used all over the place in windows.h
// For example, DBG_CONTINUE is defined. Feh.
enum SpewType_t
{
	SPEW_MESSAGE = 0,
	SPEW_WARNING,
	SPEW_ASSERT,
	SPEW_ERROR,
	SPEW_LOG,

	SPEW_TYPE_COUNT
};

enum SpewRetval_t
{
	SPEW_DEBUGGER = 0,
	SPEW_CONTINUE,
	SPEW_ABORT
};

/* type of externally defined function used to display debug spew */
typedef SpewRetval_t(*SpewOutputFunc_t)(SpewType_t spewType, const tchar* pMsg);

/* Used to redirect spew output */
void   SpewOutputFunc(SpewOutputFunc_t func);

/* Used to get the current spew output function */
SpewOutputFunc_t GetSpewOutputFunc(void);

/* This is the default spew fun, which is used if you don't specify one */
SpewRetval_t DefaultSpewFunc(SpewType_t type, const tchar* pMsg);

/* Same as the default spew func, but returns SPEW_ABORT for asserts */
SpewRetval_t DefaultSpewFuncAbortOnAsserts(SpewType_t type, const tchar* pMsg);

/* Should be called only inside a SpewOutputFunc_t, returns groupname, level, color */
const tchar* GetSpewOutputGroup(void);
int GetSpewOutputLevel(void);
const Color* GetSpewOutputColor(void);

/* Used to manage spew groups and subgroups */
void   SpewActivate(const tchar* pGroupName, int level);
bool   IsSpewActive(const tchar* pGroupName, int level);

/* Used to display messages, should never be called directly. */
void   _SpewInfo(SpewType_t type, const tchar* pFile, int line);
SpewRetval_t   _SpewMessage(PRINTF_FORMAT_STRING const tchar* pMsg, ...) FMTFUNCTION(1, 2);
SpewRetval_t   _DSpewMessage(const tchar* pGroupName, int level, PRINTF_FORMAT_STRING const tchar* pMsg, ...) FMTFUNCTION(3, 4);
SpewRetval_t   ColorSpewMessage(SpewType_t type, const Color* pColor, PRINTF_FORMAT_STRING const tchar* pMsg, ...) FMTFUNCTION(3, 4);


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
#endif

#endif /* DBG_H */
