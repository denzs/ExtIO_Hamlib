
#ifdef EXTIO_HAMLIB_EXPORTS
#define EXTIO_HAMLIB_API __declspec(dllexport) __stdcall
#else
#define EXTIO_HAMLIB_API __declspec(dllimport)
#endif

#if 1
extern "C" bool EXTIO_HAMLIB_API InitHW(char *name, char *model, int& type);
extern "C" bool EXTIO_HAMLIB_API OpenHW(void);
extern "C" int  EXTIO_HAMLIB_API StartHW(long freq);
extern "C" void EXTIO_HAMLIB_API StopHW(void);
extern "C" void EXTIO_HAMLIB_API CloseHW(void);
extern "C" int  EXTIO_HAMLIB_API SetHWLO(long LOfreq);
extern "C" long EXTIO_HAMLIB_API GetHWLO(void);
extern "C" long EXTIO_HAMLIB_API GetHWSR(void);
extern "C" long EXTIO_HAMLIB_API GetTune(void);
extern "C" void EXTIO_HAMLIB_API GetFilters(int& loCut, int& hiCut, int& pitch);
extern "C" char EXTIO_HAMLIB_API GetMode(void);
extern "C" void EXTIO_HAMLIB_API ModeChanged(char mode);
extern "C" int  EXTIO_HAMLIB_API GetStatus(void);
extern "C" void EXTIO_HAMLIB_API ShowGUI(void);
extern "C" void EXTIO_HAMLIB_API HideGUI(void);
extern "C" void EXTIO_HAMLIB_API IFLimitsChanged(long low, long high);
extern "C" void EXTIO_HAMLIB_API TuneChanged(long freq);
extern "C" void EXTIO_HAMLIB_API SetCallback(void (* Callback)(int, int, float, short *));
// void extIOCallback(int cnt, int status, float IQoffs, short IQdata[]);
extern "C" void EXTIO_HAMLIB_API RawDataReady(long samprate, int *Ldata, int *Rdata, int numsamples);

#endif
