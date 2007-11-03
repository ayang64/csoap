/******************************************************************
 *  $Id: nanohttp-logging.h,v 1.6 2007/11/03 22:40:11 m0gg Exp $
 * 
 * CSOAP Project:  A http client/server library in C
 * Copyright (C) 2007 Heiko Ronsdorf
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
 ******************************************************************/
#ifndef __nanohttp_logging_h
#define __nanohttp_logging_h

/** @file nanohttp-logging.h Logging definitions and prototypes
 *
 * @defgroup NANOHTTP_LOGGING Logging interface
 * @ingroup NANOHTTP
 */
/**@{*/

/** @defgroup NANOHTTP_CMDLINE_LOGGING Commandline flags
 * @ingroup NANOHTTP_CMDLINE
 */
/**@{*/
#define NHTTP_ARG_LOGFILE	"-NHTTPlog"
#define NHTTP_ARG_LOGLEVEL	"-NHTTPloglevel"
/**@}*/

/** Loglevel definition */
typedef enum nanohttp_loglevel
{
  NANOHTTP_LOG_OFF,     /**< Logging completely turned off (use at your
			     own risk). */
  NANOHTTP_LOG_VERBOSE, /**< Debugging messages that may overflow the
			     log */
  NANOHTTP_LOG_DEBUG,   /**< Message that contain information normally
			     of use only when debugging the library */
  NANOHTTP_LOG_INFO,    /**< Informaional messages */
  NANOHTTP_LOG_WARN,    /**< Warning messages */
  NANOHTTP_LOG_ERROR,   /**< A condition that should be corrected
			     immediately, such as a broken network
			     connection */
  NANOHTTP_LOG_FATAL    /**< A panic condition */
} nanohttp_loglevel_t;

#define NANOHTTP_LOG_LEVEL_OFF_STRING     "OFF"
#define NANOHTTP_LOG_LEVEL_VERBOSE_STRING "VERBOSE"
#define NANOHTTP_LOG_LEVEL_DEBUG_STRING   "DEBUG"
#define NANOHTTP_LOG_LEVEL_INFO_STRING    "INFO"
#define NANOHTTP_LOG_LEVEL_WARN_STRING    "WARN"
#define NANOHTTP_LOG_LEVEL_ERROR_STRING   "ERROR"
#define NANOHTTP_LOG_LEVEL_FATAL_STRING   "FATAL"
#define NANOHTTP_LOG_LEVEL_UNKNOWN_STRING "UNKNOWN"

#ifdef __cplusplus
extern "C" {
#endif

/** This function sets a new the loglevel and returns the previous
 * one.
 *
 * @param level The new loglevel.
 *
 * @return The old loglevel.
 */
extern nanohttp_loglevel_t nanohttp_log_set_loglevel(nanohttp_loglevel_t level);

/** This function returns the current loglevel.
 *
 * @return The current loglevel.
 */
extern nanohttp_loglevel_t nanohttp_log_get_loglevel(void);

/** This function set the name of a logfile.
 *
 * @param filename The filename of the logfile.
 */
extern void nanohttp_log_set_logfile(const char *filename);

/** This function returns the filename of the current logfile.
 *
 * @return Pointer to the filename or NULL otherwise.
 */
extern const char *nanohttp_log_get_logfile(void);

#define NANOHTTP_LOG_DISABLED   0x00 /**< Logging disabled */
#define NANOHTTP_LOG_FOREGROUND	0x01 /**< Logging to stdout enabled */
#define NANOHTTP_LOG_SYSLOG     0x02 /**< Syslog logging enabled */

/** This function sets the type of logging
 *
 * @return The old logtype.
 *
 * @see - NANOHTTP_LOG_DISABLED
 *      - NANOHTTP_LOG_FOREGROUND
 *      - NANOHTTP_LOG_SYSLOG
 */
extern int nanohttp_log_set_logtype(int type);

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER <= 1200
extern char *VisualC_funcname(const char *file, int line); /* not thread safe! */
#define __FUNCTION__  VisualC_funcname(__FILE__, __LINE__)
#endif
#endif

#ifdef __cplusplus
}
#endif

#define log_verbose(fmt, ...) _nanohttp_log_printf(NANOHTTP_LOG_VERBOSE, \
                             NANOHTTP_LOG_LEVEL_VERBOSE_STRING " %s: " fmt "\n", \
                             __FUNCTION__, ## __VA_ARGS__)

#define log_debug(fmt, ...)   _nanohttp_log_printf(NANOHTTP_LOG_DEBUG, \
                             NANOHTTP_LOG_LEVEL_DEBUG_STRING " %s: " fmt "\n", \
                             __FUNCTION__, ## __VA_ARGS__)

#define log_info(fmt, ...)    _nanohttp_log_printf(NANOHTTP_LOG_INFO, \
                             NANOHTTP_LOG_LEVEL_INFO_STRING " %s: " fmt "\n", \
                             __FUNCTION__, ## __VA_ARGS__)

#define log_warn(fmt, ...)    _nanohttp_log_printf(NANOHTTP_LOG_WARN, \
                             NANOHTTP_LOG_LEVEL_WARN_STRING " %s: " fmt "\n", \
                             __FUNCTION__, ## __VA_ARGS__)

#define log_error(fmt, ...)   _nanohttp_log_printf(NANOHTTP_LOG_ERROR, \
                             NANOHTTP_LOG_LEVEL_ERROR_STRING " %s: " fmt "\n", \
                             __FUNCTION__, ## __VA_ARGS__)

#define log_fatal(fmt, ...)   _nanohttp_log_printf(NANOHTTP_LOG_FATAL, \
                             NANOHTTP_LOG_LEVEL_FATAL_STRING " %s: " fmt "\n", \
                             __FUNCTION__, ## __VA_ARGS__)
/**@}*/

#endif
