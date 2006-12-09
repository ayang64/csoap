/******************************************************************
*  _  _   _   _  _   __
* | \/ | | | | \/ | | _/
* |_''_| |_| |_''_| |_'/  PARSER
*
*  $Id: nanohttp-mime.h,v 1.14 2006/12/09 09:27:11 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2003-2004  Ferhat Ayaz
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
* Email: ferhatayaz@yahoo.com
******************************************************************/
#ifndef __nanohttp_mime_h
#define __nanohttp_mime_h

/** @file
 *
 * @author	Ferhat Ayaz
 * @version	$Revision: 1.14 $
 *
 * @see		http://www.ietf.org/rfc/rfc2045.txt
 * @see		http://www.ietf.org/rfc/rfc2046.txt
 * @see		http://www.ietf.org/rfc/rfc4288.txt
 * @see		http://www.ietf.org/rfc/rfc4289.txt
 *
 */

/**
 *
 * MIME errors
 *
 */
#define MIME_ERROR			1300
#define MIME_ERROR_NO_BOUNDARY_PARAM	(MIME_ERROR + 1)
#define MIME_ERROR_NO_START_PARAM	(MIME_ERROR + 2)
#define MIME_ERROR_PARSE_ERROR		(MIME_ERROR + 3)
#define MIME_ERROR_NO_ROOT_PART		(MIME_ERROR + 4)
#define MIME_ERROR_NOT_MIME_MESSAGE	(MIME_ERROR + 5)

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------
  "multipart/related"  MIME Message Builder
 ------------------------------------------------------------------*/

extern herror_t mime_get_attachments(content_type_t * ctype, struct http_input_stream_t * in, struct attachments_t ** dest);

#ifdef __cplusplus
}
#endif

#endif
