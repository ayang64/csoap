#include "csoaplog.h"

#include <stdarg.h>
#include <stdio.h>

static
void _SoapLogWrite(SoapLogLevel level,
		   const char* function,
		   const char* format,
		   va_list ap)
{
  char buffer[1054];
  char buffer2[1054];
  sprintf(buffer, "[%s] [%d] %s\n",  function, level, format);
  vsprintf(buffer2, buffer, ap);
/*     printf(buffer2);
    fflush(stdout);
*/
}


void SoapTraceEnter(const char* function,
		    const char* format, ...)
{
  va_list ap;
  
  va_start(ap, format);
  _SoapLogWrite(LOG_TRACE, function, format, ap);
  va_end(ap);
}


void SoapTraceLeave(const char* function, 
		    const char* format, ...)
{
  va_list ap;
  
  va_start(ap, format);
  _SoapLogWrite(LOG_TRACE, function, format, ap);
  va_end(ap);
}


void SoapTraceLeaveWithError(const char* function,
			     const char* format, ...)
{
  va_list ap;
  
  va_start(ap, format);
  _SoapLogWrite(LOG_ERROR, function, format, ap);
  va_end(ap);
}


void SoapLog(SoapLogLevel level, const char* function,
	     const char* format, ...)
{
  va_list ap;
  
  va_start(ap, format);
  _SoapLogWrite(level, function, format, ap);
  va_end(ap);
}

	     
