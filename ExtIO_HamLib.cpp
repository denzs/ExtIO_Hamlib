
#define EXTIO_HAMLIB_EXPORTS	1
#define BORLAND                 0
#include "ExtIO_HamLib.h"

//---------------------------------------------------------------------------
// #define WIN32_LEAN_AND_MEAN             // Selten verwendete Teile der Windows-Header nicht einbinden.
#include <windows.h>
#include <string.h>
#include <stdio.h>

#if BORLAND
#include "hamlib/rig.h"
#else
#include <hamlib/rig.h>
#endif
//---------------------------------------------------------------------------

#define OPTIONAL_IFC		1
#define SINGLE_FRG_OFFSET	1

// Version fuer dieses ExtIO
#define VER_MAJOR			1
#define VER_MINOR			5

#define HAMLIB_PATH			"C:\\"
#define	HAMLIB_BASENAME		"ExtIO_HamLib"
#define	HAMLIB_POSTNAME		"A"

#if 1
// ohne "C:\", ohne Post "-A"
#define HAMLIB_NAME			HAMLIB_BASENAME
#elif 0
// mit "C:\\", ohne Post "-A"
#define HAMLIB_NAME			HAMLIB_PATH HAMLIB_BASENAME
#else
// mit "C:\\", mit Post "-A"
#define HAMLIB_NAME			HAMLIB_PATH HAMLIB_BASENAME "-" HAMLIB_POSTNAME
#endif

long	glLOfreq = 0L;
char	gcMode = 'U';

int		giLog = 1;
enum rig_debug_level_e	geRigDebugLevel = RIG_DEBUG_NONE;
rig_model_t	giRigModel = 0;
rmode_t	geRMode = RIG_MODE_NONE;
FILE	* psLogFile = 0;
RIG		* gpsRIG = 0;
#if SINGLE_FRG_OFFSET
long	glLOoffset = 0L;
#else
long	glLOsetOffset = 0L;
long	glLOgetOffset = 0L;
long	glLOtoTune = 0L;
#endif

int		giSetRMode = 0;
char	gacTemp[1024];
char	gacRigPort[1024];
char	gacRigConf[1024];
bool	gbInitHW = false;
bool	gbLogRawDataReady = true;


/*
 * To summarize, the only mandatory entry points, those that *must* be implemented,
 * and whose presence is checked by Winrad, are the following:
 *
    InitHW                        = _InitHW@12
    OpenHW                        = _OpenHW@0
    StartHW                       = _StartHW@4
    StopHW                        = _StopHW@0
    CloseHW                       = _CloseHW@0
    SetHWLO                       = _SetHWLO@4
    GetStatus                     = _GetStatus@0
    SetCallback                   = _SetCallback@4
 *
 * The remaining/optional entry points:
 *
    GetHWLO                       = _GetHWLO@0
    GetHWSR                       = _GetHWSR@0
    ShowGUI                       = _ShowGUI@0
    HideGUI                       = _HideGUI@0
    RawDataReady                  = _RawDataReady@16
    TuneChanged                   = _TuneChanged@4
    IFLimitsChanged               = _IFLimitsChanged@8
    GetTune                       = _GetTune@0
    GetMode                       = _GetMode@0
    GetFilters                    = _GetFilters@12
    ModeChanged                   = _ModeChanged@4
 *
 *
 *
	// example code from WinRad
 	  extHWtype = none;
	  if( NULL != (hextIO = LoadLibrary( _T(dllname))))
	  {
		 InitHW          = (pfnInitHW) GetProcAddress(hextIO,         "InitHW" );		// called after LoadLibrary() or on SelExtHWClick()
		 OpenHW          = (pfnOpenHW) GetProcAddress(hextIO,         "OpenHW");		// called on SelExtHWClick()
		 CloseHW         = (pfnCloseHW)GetProcAddress(hextIO,         "CloseHW" );		// called on Quit of WinRad
		 StartHW         = (pfnStartHW)GetProcAddress(hextIO,         "StartHW");		// called on StartStopClick()
		 StopHW          = (pfnStopHW) GetProcAddress(hextIO,         "StopHW");		// called on StartStopClick()
		 SetCallback     = (pfnSetCallback) GetProcAddress(hextIO,    "SetCallback");	// called on SelExtHWClick()
		 SetHWLO         = (pfnSetHWLO)GetProcAddress(hextIO,         "SetHWLO");			// several
		 GetHWLO         = (pfnGetHWLO)GetProcAddress(hextIO,         "GetHWLO");		// called on MyIdleHandler()
		 GetStatus       = (pfnGetStatus)GetProcAddress(hextIO,       "GetStatus");			// never???
		 GetHWSR         = (pfnGetHWSR) GetProcAddress(hextIO,        "GetHWSR");		// called on MyIdleHandler() or on SelExtHWClick()
		 RawDataReady    = (pfnRawDataReady)GetProcAddress(hextIO,    "RawDataReady");		// never???
		 ShowGUI         = (pfnShowGUI)GetProcAddress(hextIO,         "ShowGUI");
		 HideGUI         = (pfnHideGUI)GetProcAddress(hextIO,         "HideGUI");
		 TuneChanged     = (pfnTuneChanged)GetProcAddress(hextIO,     "TuneChanged");	// called on SelExtHWClick() with (FF1freq + extLOfreq + 0.5)
																						//   or on SetNCONixieFreq()
		 GetTune         = (pfnGetTune) GetProcAddress(hextIO,        "GetTune");		// called on MyIdleHandler()
		 ModeChanged     = (pfnModeChanged)GetProcAddress(hextIO,     "ModeChanged");	// called on BFMClick() or BUSBClick() or BLSBClick() or ..
		 GetMode         = (pfnGetMode) GetProcAddress(hextIO,        "GetMode");		// called on MyIdleHandler()
		 IFLimitsChanged = (pfnIFLimitsChanged)GetProcAddress(hextIO, "IFLimitsChanged");	// called on SetParam() or MyIdleHandler() or ..
		 FiltersChanged  = (pfnFiltersChanged)GetProcAddress(hextIO,  "FiltersChanged");	// called on AdjustBlueWindow()
		 GetFilters      = (pfnGetFilters)GetProcAddress(hextIO,      "GetFilters");	// called on MyIdleHandler() or AdjustBlueWindow()

		// check that the mandatory entry points are present
		 if(InitHW == NULL  || CloseHW == NULL || OpenHW == NULL ||
			StartHW == NULL || StopHW == NULL  || SetCallback == NULL ||
			SetHWLO == NULL || GetStatus == NULL)
		 {
			FreeLibrary(hextIO);
			hextIO = NULL;
			SelExtHW->Visible = false;
		 }
		 else
		 {
			SelExtHW->Visible = true;
			extHWready = InitHW(extHWname, extHWmodel, (int &)extHWtype);
			SetCallback(extIOCallback);
		 }
	  }
 *
 */

#if BORLAND
//---------------------------------------------------------------------------
//   Important note about DLL memory management when your DLL uses the
//   static version of the RunTime Library:
//
//   If your DLL exports any functions that pass String objects (or structs/
//   classes containing nested Strings) as parameter or function results,
//   you will need to add the library MEMMGR.LIB to both the DLL project and
//   any other projects that use the DLL.  You will also need to use MEMMGR.LIB
//   if any other projects which use the DLL will be performing new or delete
//   operations on any non-TObject-derived classes which are exported from the
//   DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
//   EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
//   the file BORLNDMM.DLL should be deployed along with your DLL.
//
//   To avoid using BORLNDMM.DLL, pass string information using "char *" or
//   ShortString parameters.
//
//   If your DLL uses the dynamic version of the RTL, you do not need to
//   explicitly add MEMMGR.LIB as this will be done implicitly for you
//---------------------------------------------------------------------------

#pragma argsused
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)

#else
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
#endif
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


static
void stripEOL( char * pacStr )
{
	while ( strlen(pacStr) > 0 )
	{
		if ( pacStr[ strlen(pacStr) -1 ] == 10 )
			pacStr[ strlen(pacStr) -1 ] = 0;
		else if ( pacStr[ strlen(pacStr) -1 ] == 13 )
			pacStr[ strlen(pacStr) -1 ] = 0;
		else
			break;
	}
}

static
char* getNextLine( FILE * psFile, char * pacLine, int iMaxLen )
{
	int iLen = 0;
	char *ret;
	do
	{
		pacLine[0] = '\0';
		ret = fgets( pacLine, iMaxLen, psFile );	// read line
		iLen = strlen(pacLine);
	} while ( ret && (iLen <= 1 || ( iLen >= 1 && '#' == pacLine[0] ) ));
	return ret;
}

static void openLog()
{
	if ( psLogFile )
		return;
	psLogFile = fopen( HAMLIB_NAME ".log", "a");

	if ( psLogFile )
	{
		fprintf(psLogFile, "ExtIO_HamLib version %d.%d\n", VER_MAJOR, VER_MINOR );
		fflush(psLogFile);
	}
}

static
bool readConfig()
{
	FILE * psFile = fopen( HAMLIB_NAME ".cfg", "r");
	if ( !psFile )
	{
		openLog();
		if ( psLogFile )
		{
			fprintf(psLogFile, "Cannot open " HAMLIB_NAME ".cfg for read\n" );
			fflush(psLogFile);
		}
		return false;
	}

	int iRigModel = 0;
	int iTmp = 0;
	gacRigPort[0] = 0;
	gacRigConf[0] = 0;

	getNextLine( psFile, gacTemp, 1023 );		// 1st line: create Log File ?
	sscanf( gacTemp, "%d", &giLog );

	getNextLine( psFile, gacTemp, 1023 );		// 2nd line has to be model; see "rigctl --list"
	sscanf( gacTemp, "%d", &iRigModel );
	giRigModel = (rig_model_t)iRigModel;

	getNextLine( psFile, gacRigPort, 1023 );	// 3rd line has to be port (f.e. "/proc/ttyS0")
	stripEOL( gacRigPort );						// strip LF & CR at End Of Line

#if SINGLE_FRG_OFFSET
	getNextLine( psFile, gacTemp, 1023 );		// 4th line: offset
	sscanf( gacTemp, "%ld", &glLOoffset );
#else
	getNextLine( psFile, gacTemp, 1023 );		// 4th line: rig_set_freq( LO + x ) ; x = +12000
	sscanf( gacTemp, "%ld", &glLOsetOffset );

	getNextLine( psFile, gacTemp, 1023 );		// 5th line: LO = rig_get_freq() + y ; y = -12000
	sscanf( gacTemp, "%ld", &glLOgetOffset );

	getNextLine( psFile, gacTemp, 1023 );		// 6th line: Tune = LO + z ; z = +12000
	sscanf( gacTemp, "%ld", &glLOtoTune );
#endif

	getNextLine( psFile, gacTemp, 1023 );		// 5th / 7th line has to be 0 or 1; 1 to set rigmode
	sscanf( gacTemp, "%d", &giSetRMode );

	getNextLine( psFile, gacTemp, 1023 );		// 6th / 8th line has to be the rigmode
	sscanf( gacTemp, "%d", &iTmp );
	geRMode = (rmode_t)iTmp;

	getNextLine( psFile, gacTemp, 1023 );		// 7th / 9th line has to be the rig debug level
	sscanf( gacTemp, "%d", &iTmp );
	geRigDebugLevel = (enum rig_debug_level_e)iTmp;

	getNextLine( psFile, gacRigConf, 1023 );	// 8th / 10th line has to be conf params; see "rigctl --set-conf=<PARM=VAL>"
	stripEOL( gacRigConf );						// strip LF & CR at End Of Line

	fclose( psFile );

	return true;
}


//---------------------------------------------------------------------------
extern "C"
bool __declspec(dllexport) __stdcall InitHW(char *name, char *model, int& type)
{
	static bool first = true;

	// The audio data are returned via the sound card managed by Winrad. The external
	// hardware  just controls the LO, and possibly a preselector, under DLL control.
	type = 4;

	// assume not connected
	strcpy(name,  "HamLib - not connected");
	strcpy(model, "not connected");

	if ( !psLogFile  &&  giLog )
		openLog();

	if ( psLogFile )
	{
		fprintf(psLogFile, "InitHW()\n");
		fflush(psLogFile);
	}

	if ( !first )
	{
		if ( psLogFile )
		{
			fprintf(psLogFile, "  !first => return %d\n", (gbInitHW ? 1 : 0) );
			fflush(psLogFile);
		}
		return gbInitHW;
	}

	first = false;

	bool bConfOk = readConfig();
	if ( !bConfOk )
		return false;

	if ( giLog )
		openLog();
	else
	{
		fclose( psLogFile );
		psLogFile = 0;
	}

	if ( psLogFile )
	{
#if SINGLE_FRG_OFFSET
		fprintf(psLogFile, "  read from cfg: rigmodel %d, port '%s', conf '%s', LO offset %ld\n", (int)giRigModel, gacRigPort, gacRigConf, glLOoffset );
#else
		fprintf(psLogFile, "  read from cfg: rigmodel %d, port '%s', conf '%s', LO offsets: set %ld get %ld tune %ld\n", (int)giRigModel, gacRigPort, gacRigConf, glLOsetOffset, glLOgetOffset, glLOtoTune );
#endif
		fflush(psLogFile);
	}

	rig_set_debug ( geRigDebugLevel );

	if ( geRigDebugLevel != RIG_DEBUG_NONE && psLogFile )
		rig_set_debug_file (psLogFile);

	gpsRIG = rig_init( giRigModel );
	if ( !gpsRIG )
	{
		if ( psLogFile )
		{
			fprintf(psLogFile, "  rig_init(%d) failed!\n", giRigModel );
			fflush(psLogFile);
		}
		return false;
	}

	strncpy(gpsRIG->state.rigport.pathname,gacRigPort,FILPATHLEN);

	// Parse list of comma separated PARM=VALUE conf line
	char *pacCfgParm = gacRigConf;
	char *pacCfgVal = strchr(pacCfgParm, '=');
	while (pacCfgVal) {
		*pacCfgVal++ = '\0';
		char *pacCfgEnd = strchr(pacCfgVal, ',');
		if (pacCfgEnd) *pacCfgEnd++ = '\0';

		int retcode = rig_set_conf(gpsRIG, rig_token_lookup(gpsRIG, pacCfgParm), pacCfgVal);
		if (retcode != RIG_OK)
		{
			if ( psLogFile )
			{
				fprintf(psLogFile, "  rig_set_conf() failed with error %s !\n", rigerror(retcode));
				fflush(psLogFile);
			}
			if ( psLogFile )
			{
				fprintf(psLogFile, "  rig_cleanup()\n");
				fflush(psLogFile);
			}
			rig_cleanup(gpsRIG); /* if you care about memory */
			gpsRIG = 0;
			return false;
		}

		pacCfgParm = pacCfgEnd;
		pacCfgVal = strchr(pacCfgParm, '=');
	}

	int retcode = rig_open(gpsRIG);
	if (retcode != RIG_OK)
	{
		if ( psLogFile )
		{
			fprintf(psLogFile, "  rig_open() failed with error %s !\n", rigerror(retcode));
			fflush(psLogFile);
		}
		if ( psLogFile )
		{
			fprintf(psLogFile, "  rig_cleanup()\n");
			fflush(psLogFile);
		}
		rig_cleanup(gpsRIG); /* if you care about memory */
		gpsRIG = 0;
		return false;
	}

	gbInitHW = true;

	const struct rig_caps * psRigCaps = rig_get_caps(giRigModel);
	if ( psRigCaps )
	{
		sprintf(name,  "HamLib - %s", psRigCaps->model_name);	// change with the model of your HW
		strcpy(model, psRigCaps->model_name);	        		// change with the model of your HW
	}
	else
	{
		sprintf(name,  "HamLib - unknown model %d", giRigModel );	// change with the model of your HW
		sprintf(model, "unknown model %d", giRigModel);				// change with the model of your HW
	}

	glLOfreq = 6075000;	// just a default value
	// .......... init here the hardware controlled by the DLL
	// ......... init here the DLL graphical interface, if any
	gbLogRawDataReady = true;

	if ( psLogFile )
	{
		fprintf(psLogFile, "  return with %d\n", ( gbInitHW ? 1 : 0 ) );
		fflush(psLogFile);
	}
	return gbInitHW;
}

//---------------------------------------------------------------------------
extern "C"
bool EXTIO_HAMLIB_API OpenHW(void)
{
	if ( psLogFile )
	{
		fprintf(psLogFile, "OpenHW(): return %d\n", (int)gbInitHW );
		fflush(psLogFile);
	}

	gbLogRawDataReady = true;

	// .... display here the DLL panel ,if any....
	// .....if no graphical interface, delete the following statement

	//::SetWindowPos(F->handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// in the above statement, F->handle is the window handle of the panel displayed 
	// by the DLL, if such a panel exists
	return gbInitHW;
}

//---------------------------------------------------------------------------
extern "C"
int  EXTIO_HAMLIB_API StartHW(long LOfreq)
{
	if ( psLogFile )
	{
		fprintf(psLogFile, "StartHW(%ld)\n", LOfreq);
		fflush(psLogFile);
	}
	if (!gbInitHW)
		return 0;

	gbLogRawDataReady = true;
	SetHWLO(LOfreq);

	// number of complex elements returned each
	// invocation of the callback routine
	return 0;
}

//---------------------------------------------------------------------------
extern "C"
void EXTIO_HAMLIB_API StopHW(void)
{
	if ( psLogFile )
	{
		fprintf(psLogFile, "StopHW(): return void\n");
		fflush(psLogFile);
	}

	return;  // nothing to do with this specific HW
}

//---------------------------------------------------------------------------
extern "C"
void EXTIO_HAMLIB_API CloseHW(void)
{
	if ( psLogFile )
	{
		fprintf(psLogFile, "CloseHW()\n");
		fflush(psLogFile);
	}

	// ..... here you can shutdown your graphical interface, if any............
	if (gbInitHW && gpsRIG )
	{
		if ( psLogFile )
		{
			fprintf(psLogFile, "  rig_close()\n");
			fflush(psLogFile);
		}
		rig_close(gpsRIG); /* close port */
		if ( psLogFile )
		{
			fprintf(psLogFile, "  rig_cleanup()\n");
			fflush(psLogFile);
		}
		rig_cleanup(gpsRIG); /* if you care about memory */
	}
	gbInitHW = false;
	gpsRIG = 0;
	if ( psLogFile )
	{
		fprintf(psLogFile, "  return\n");
		fflush(psLogFile);
	}
}

//---------------------------------------------------------------------------
extern "C"
int  EXTIO_HAMLIB_API SetHWLO(long LOfreq)
{
	// ..... set here the LO frequency in the controlled hardware
	// Set here the frequency of the controlled hardware to LOfreq
	glLOfreq = LOfreq;

	if ( psLogFile )
	{
		fprintf(psLogFile, "SetHWLO(%ld)\n", LOfreq);
		fflush(psLogFile);
	}

	if ( gbInitHW && gpsRIG )
	{
		if ( psLogFile )
		{
			fprintf(psLogFile, "  rig_set_freq(): ");
			fflush(psLogFile);
		}
#if SINGLE_FRG_OFFSET
		int retcode = rig_set_freq( gpsRIG, RIG_VFO_CURR, glLOfreq + glLOoffset );
#else
		int retcode = rig_set_freq( gpsRIG, RIG_VFO_CURR, glLOfreq + glLOsetOffset );
#endif
		if ( retcode != RIG_OK )
		{
			if ( psLogFile )
			{
				fprintf(psLogFile, "  failed with error %s !\n", rigerror(retcode));
				fflush(psLogFile);
			}
			return 0;
		}
		else
		{
			if ( psLogFile )
			{
				fprintf(psLogFile, "  ok.\n");
				fflush(psLogFile);
			}
		}
		if ( giSetRMode )
		{
			if ( psLogFile )
			{
				fprintf(psLogFile, "  rig_set_mode(): ");
				fflush(psLogFile);
			}
			retcode = rig_set_mode(gpsRIG, RIG_VFO_CURR, geRMode, RIG_PASSBAND_NORMAL);
			if ( retcode != RIG_OK )
			{
				if ( psLogFile )
				{
					fprintf(psLogFile, "  failed with error %s !\n", rigerror(retcode));
					fflush(psLogFile);
				}
				return 0;
			}
			else
			{
				if ( psLogFile )
				{
					fprintf(psLogFile, "  ok.\n");
					fflush(psLogFile);
				}
			}
		}
	}

	if ( psLogFile )
	{
		fprintf(psLogFile, "  return\n");
		fflush(psLogFile);
	}
	// 0 The function did complete without errors.
	// < 0 (a negative number N)
	//     The specified frequency  is  lower than the minimum that the hardware  is capable to generate.
	//     The absolute value of N indicates what is the minimum supported by the HW.
	// > 0 (a positive number N) The specified frequency is greater than the maximum that the hardware
	//     is capable to generate.
	//     The value of N indicates what is the maximum supported by the HW.
	return 0;
}

//---------------------------------------------------------------------------
extern "C"
int  EXTIO_HAMLIB_API GetStatus(void)
{
	if ( psLogFile )
	{
		fprintf(psLogFile, "GetStatus(): not supported\n");
		fflush(psLogFile);
	}

	return 0;  // status not supported by this specific HW,
}

//---------------------------------------------------------------------------
extern "C"
void EXTIO_HAMLIB_API SetCallback( void (* Callback)(int, int, float, short *) )
{
	if ( psLogFile )
	{
		fprintf(psLogFile, "SetCallback(): not supported\n");
		fflush(psLogFile);
	}
	// this HW does not return audio data through the callback device
	// nor it has the need to signal a new sampling rate.
	return;
}


//---------------------------------------------------------------------------
#if OPTIONAL_IFC

extern "C"
long EXTIO_HAMLIB_API GetHWLO(void)
{
	if ( psLogFile )
	{
		fprintf(psLogFile, "GetHWLO()\n");
		fflush(psLogFile);
	}
	if ( gbInitHW && gpsRIG )
	{
		freq_t freq;
		if ( psLogFile )
		{
			fprintf(psLogFile, "  rig_get_freq(): ");
			fflush(psLogFile);
		}
		int retcode = rig_get_freq( gpsRIG, RIG_VFO_CURR, &freq );
		if ( RIG_OK == retcode )
		{
			if ( psLogFile )
			{
				fprintf(psLogFile, "  ok\n");
				fflush(psLogFile);
			}
#if SINGLE_FRG_OFFSET
			glLOfreq = (long)freq - glLOoffset;
#else
			glLOfreq = (long)freq + glLOgetOffset;
#endif
		}
		else
		{
			if ( psLogFile )
			{
				fprintf(psLogFile, "  failed with error %s !\n", rigerror(retcode));
				fflush(psLogFile);
			}
		}
	}
	if ( psLogFile )
	{
		fprintf(psLogFile, "  return %ld\n", glLOfreq);
		fflush(psLogFile);
	}
	return glLOfreq;
}

#endif

//---------------------------------------------------------------------------
#if OPTIONAL_IFC

extern "C"
long EXTIO_HAMLIB_API GetHWSR(void)
{
	// This DLL controls just an oscillator, not a digitizer
	if ( psLogFile )
	{
		fprintf(psLogFile, "  GetHWSR(): not supported\n");
		fflush(psLogFile);
	}
	return 0;
}

#endif

//---------------------------------------------------------------------------
#if OPTIONAL_IFC

extern "C"
long EXTIO_HAMLIB_API GetTune(void)
{
#if SINGLE_FRG_OFFSET
	long lTune = glLOfreq + glLOoffset;
#else
	long lTune = glLOfreq + glLOtoTune;
#endif
	if ( psLogFile )
	{
		fprintf(psLogFile, "GetTune(): return %ld\n", lTune );
		fflush(psLogFile);
	}
	return lTune;
}

#endif

//---------------------------------------------------------------------------
#if OPTIONAL_IFC

extern "C"
void EXTIO_HAMLIB_API GetFilters(int& loCut, int& hiCut, int& pitch)
{
	if ( psLogFile )
	{
		fprintf(psLogFile, "GetFilters(): return 12000 +/- 6000\n");
		fflush(psLogFile);
	}
	loCut = 12000 -6000;
	hiCut = 12000 +6000;
	pitch = 1000;
}

#endif

//---------------------------------------------------------------------------
#if OPTIONAL_IFC

extern "C"
char EXTIO_HAMLIB_API GetMode(void)
{
	// ‘A’  =  AM  ‘E’  =  ECSS  ‘F’  = FM  ‘L’  =  LSB  ‘U’  =  USB  ‘C’  =  CW  ‘D’  =  DRM
	if ( psLogFile )
	{
		fprintf(psLogFile, "GetMode(): return '%c'\n", gcMode);
		fflush(psLogFile);
	}
	return gcMode;
}

#endif

//---------------------------------------------------------------------------
#if OPTIONAL_IFC

extern "C"
void EXTIO_HAMLIB_API ModeChanged(char mode)
{
	if ( psLogFile )
	{
		fprintf(psLogFile, "ModeChanged('%c')\n", mode);
		fflush(psLogFile);
	}
	gcMode = mode;
}

#endif

//---------------------------------------------------------------------------
#if OPTIONAL_IFC

extern "C"
void EXTIO_HAMLIB_API ShowGUI(void)
{
	// If the DLL has a GUI, now you have to make it visible
	if ( psLogFile )
	{
		fprintf(psLogFile, "ShowGUI(): not supported\n");
		fflush(psLogFile);
	}
	return;
}

#endif

//---------------------------------------------------------------------------
#if OPTIONAL_IFC

extern "C"
void EXTIO_HAMLIB_API HideGUI(void)
{
	// If the DLL has a GUI, now you have to hide it
	if ( psLogFile )
	{
		fprintf(psLogFile, "HideGUI(): not supported\n");
		fflush(psLogFile);
	}
	return;
}

#endif

//---------------------------------------------------------------------------
#if OPTIONAL_IFC

extern "C"
void EXTIO_HAMLIB_API IFLimitsChanged(long low, long high)
{
	// Do whatever you want with the information about the new limits of the
	// spectrum/waterfall window
	if ( psLogFile )
	{
		fprintf(psLogFile, "IFLimitsChanged(%ld, %ld)\n", low, high);
		fflush(psLogFile);
	}
	return;
}

#endif

//---------------------------------------------------------------------------
#if OPTIONAL_IFC

extern "C"
void EXTIO_HAMLIB_API TuneChanged(long freq)
{
	// Do whatever you want with the information about the new frequency tuned by
	// the user
	if ( psLogFile )
	{
		fprintf(psLogFile, "TuneChanged(%ld)\n", freq);
		fflush(psLogFile);
	}
	return;
}

#endif

//---------------------------------------------------------------------------
#if OPTIONAL_IFC

extern "C"
void EXTIO_HAMLIB_API RawDataReady(long samprate, int *Ldata, int *Rdata, int numsamples)
{
	// we don’t know what to do with the raw audio data just passed from Winrad.
	// We also could not implement this entry point.
	if ( psLogFile && gbLogRawDataReady )
	{
		fprintf(psLogFile, "RawDataReady(): not used - further calls will not get logged!\n");
		fflush(psLogFile);
		gbLogRawDataReady = false;
	}
	return;
}

#endif

//---------------------------------------------------------------------------

