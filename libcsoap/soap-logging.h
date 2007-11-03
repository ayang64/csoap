/******************************************************************
 *  $Id: soap-logging.h,v 1.1 2007/11/03 22:40:09 m0gg Exp $
 * 
 * CSOAP Project:  A http client/server library in C
 * Copyright (C) 2003-2006  Ferhat Ayaz
 * Copyright (C) 2007       Heiko Ronsdorf
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 * 
 * Email: hero@persua.de
 ******************************************************************/
#ifndef __soap_logging_h
#define __soap_logging_h

/** @file soap-logging.h Logging definitions and prototypes
 *
 * @defgrpoup CSOAP_LOGGING Logging interface
 * @ingroup CSOAP
 */
/**@{*/

/** @defgroup CSOAP_CDMLINE_LOGGING Commandline flags
 * @ingroup CSOAP_CMDLINE
 */
/**@{*/
#define CSOAP_ARG_LOGFILE	"-CSOAPlog"
#define CSOAP_ARG_LOGLEVEL	"-CSOAPloglevel"
/**@}*/

/** Loglevel definition */
typedef enum soap_loglevel
{
  SOAP_LOG_OFF,     /**< Logging completely turned off (use at your own
		         risk). */
  SOAP_LOG_VERBOSE, /**< Debugging messages thath may overlfow the
		         log */
  SOAP_LOG_DEBUG,   /**< Messages that contain information normallu of
		         use only when debugging the library. */
  SOAP_LOG_INFO,    /**< Infmormational messages */
  SOAP_LOG_WARN,    /**< Warning messages. */
  SOAP_LOG_ERROR,   /**< A condition that should be corrected
		         immediately, such as a broken network
			 connection. */
  SOAP_LOG_FATAL    /**< A panic condition. */
} soap_loglevel_t;

#define SOAP_LOG_LEVEL_OFF_STRING     "OFF"
#define SOAP_LOG_LEVEL_VERBOSE_STRING "VERBOSE"
#define SOAP_LOG_LEVEL_DEBUG_STRING   "DEBUG"
#define SOAP_LOG_LEVEL_INFO_STRING    "INFO"
#define SOAP_LOG_LEVEL_WARN_STRING    "WARN"
#define SOAP_LOG_LEVEL_ERROR_STRING   "ERROR"
#define SOAP_LOG_LEVEL_FATAL_STRING   "FATAL"
#define SOAP_LOG_LEVEL_UNKNOWN_STRING "UNKNWON"

#ifdef __cplusplus
extern "C" {
#endif

/** Set the loglevel.
 *
 * @param level The new loglevel.
 *
 * @return The old loglevel.
 */
extern soap_loglevel_t soap_set_loglevel(soap_loglevel_t loglevel);

/** Get the loglevel.
 *
 * @return The current loglevel.
 */
extern soap_loglevel_t soap_get_loglevel(void);

/** Set the logfile.
 *
 * @param filename The filename of the logfile.
 */
extern void soap_set_logfile(const char *filename);

/** Get the filename of the logfile.
 *
 * @return Pointer to the filename or null otherwise.
 */
extern char *soap_get_logfile(void);

#define SOAP_LOG_DISABLED   0x00 /**< Logging disabled */
#define SOAP_LOG_FOREGROUND 0x01 /**< Logging to stdout enabled */
#define SOAP_LOG_SYSLOG     0x02 /**< Syslog logging enabled */

/** This function sets the type of logging.
 *
 * @return The old logtype.
 *
 * @see - SOAP_LOG_DISABLED
 *      - SOAP_LOG_FOREGROUND
 *      - SOAP_LOG_SYSLOG
 */
extern int soap_log_set_logtype(int type);

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER <= 1200
extern char *VisualC_funcname(const char *file, int line);     /* not thread safe! */
#define __FUNCTION__  VisualC_funcname(__FILE__, __LINE__)
#endif
#endif

#ifdef __cplusplus
}
#endif

#define log_verbose(fmt, ...) _nanohttp_log_printf(SOAP_LOG_VERBOSE, \
                              SOAP_LOG_LEVEL_VERBOSE_STRING " %s: " fmt "\n", \
                              __FUNCTION__, ## __VA_ARGS__)

#define log_debug(fmt, ...)   _nanohttp_log_printf(SOAP_LOG_DEBUG, \
                              SOAP_LOG_LEVEL_DEBUG_STRING " %s: " fmt "\n", \
                              __FUNCTION__, ## __VA_ARGS__)

#define log_info(fmt, ...)    _nanohttp_log_printf(SOAP_LOG_INFO, \
                              SOAP_LOG_LEVEL_INFO_STRING " %s: " fmt "\n", \
                              __FUNCTION__, ## __VA_ARGS__)

#define log_warn(fmt, ...)    _nanohttp_log_printf(SOAP_LOG_WARN, \
                              SOAP_LOG_LEVEL_WARN_STRING " %s: " fmt "\n", \
                              __FUNCTION__, ## __VA_ARGS__)

#define log_error(fmt, ...)   _nanohttp_log_printf(SOAP_LOG_ERROR, \
                              SOAP_LOG_LEVEL_ERROR_STRING " %s: " fmt "\n", \
                              __FUNCTION__, ## __VA_ARGS__)

#define log_fatal(fmt, ...)   _nanohttp_log_printf(SOAP_LOG_FATAL, \
                              SOAP_LOG_LEVEL_FATAL_STRING " %s: " fmt "\n", \
                              __FUNCTION__, ## __VA_ARGS__)

/**@}*/

#endif
