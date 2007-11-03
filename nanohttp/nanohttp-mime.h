/******************************************************************
*  _  _   _   _  _   __
* | \/ | | | | \/ | | _/
* |_''_| |_| |_''_| |_'/  PARSER
*
*  $Id: nanohttp-mime.h,v 1.18 2007/11/03 22:40:11 m0gg Exp $
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

/** @page nanohttp_mime_page nanoHTTP MIME attachments
 *
 * @section nanohttp_mime_toc_sec Table of contents
 *
 * - @ref nanohttp_mime_client_sec
 *   - @ref nanohttp_mime_client_init_sec
 *   - @ref nanohttp_mime_client_request_sec
 *   - @ref nanohttp_mime_client_response_sec
 *   - @ref nanohttp_mime_client_cleanup_sec
 * - @ref nanohttp_mime_server_sec
 *   - @ref nanohttp_mime_server_init_sec
 *   - @ref nanohttp_mime_server_cleanup_sec
 *
 * @section nanohttp_mime_client_sec nanoHTTP client with MIME attachments
 *
 * @subsection nanohttp_mime_client_init_sec Client initialization
 *
 * See @ref nanohttp_client_init_sec for more information about client
 * initialization.
 *
 * @subsection nanohttp_mime_client_request_sec Request the server
 *
 * The following functions can be used to transmit MIME attachments:
 *
 * @code
 * httpc_mime_begin(conn, url, start_id, "", "text/tml")
 * httpc_mime_next(conn, start_id, "text/xml", "binary")
 * http_output_stream_write(conn->out, buffer, size)
 * httpc_mime_send_file()
 * httpc_mime_end(conn, &res)
 * @endcode
 *
 * @subsection nanohttp_mime_client_response_sec Read the server response
 *
 * @subsection nanohttp_mime_client_cleanup_sec Client cleanup
 *
 * See @ref nanohttp_client_cleanup_sec for more information about client
 * cleanup.
 *
 * @section nanohttp_mime_server_sec nanoHTTP server with MIME attachments
 *
 * @subsection nanohttp_mime_server_init_sec Server initialization
 *
 * @subsection nanohttp_mime_server_cleanup_sec Server cleanup
 *
 * @author	Ferhat Ayaz
 * @version	$Revision: 1.18 $
 *
 * @see		http://www.ietf.org/rfc/rfc2045.txt
 * @see		http://www.ietf.org/rfc/rfc2046.txt
 * @see		http://www.ietf.org/rfc/rfc4288.txt
 * @see		http://www.ietf.org/rfc/rfc4289.txt
 */

/** @file nanohttp-mime.h MIME handling 
 *
 * @defgroup NANOHTTP_MIME MIME handling
 * @ingroup NANOHTTP
 *
 */
/**@{*/

/** @defgroup NANOHTTP_MIME_ERRORS MIME errors
 */
/**@{*/
#define MIME_ERROR			1300
#define MIME_ERROR_NO_BOUNDARY_PARAM	(MIME_ERROR + 1)
#define MIME_ERROR_NO_START_PARAM	(MIME_ERROR + 2)
#define MIME_ERROR_PARSE_ERROR		(MIME_ERROR + 3)
#define MIME_ERROR_NO_ROOT_PART		(MIME_ERROR + 4)
#define MIME_ERROR_NOT_MIME_MESSAGE	(MIME_ERROR + 5)
/**@}*/

#ifdef __cplusplus
extern "C" {
#endif

/** "multipart/related"  MIME Message Builder
 *
 */
extern herror_t mime_get_attachments(content_type_t * ctype, struct http_input_stream_t * in, struct attachments_t ** dest);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
