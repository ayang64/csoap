#ifndef CSOAP_LOG_H
#define CSOAP_LOG_H


typedef enum tagSoapLogLevel
{
  LOG_TRACE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR
}SoapLogLevel;

void SoapTraceEnter(const char* function,
		    const char* format, ...);

void SoapTraceLeave(const char* function,
		    const char* format, ...);
void SoapTraceLeaveWithError(const char* function,
			     const char* format, ...);

void SoapLog(SoapLogLevel level, const char* function,
	     const char* format, ...);
	     
#endif

