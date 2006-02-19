/******************************************************************
*  $Id: nanohttp-base64.h,v 1.1 2006/02/19 22:22:41 snowdrop Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2003  Ferhat Ayaz
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
#ifndef __nanohttp_base64_h
#define __nanohttp_base64_h

#ifdef __cplusplus
extern "C" {
#endif

extern void base64_encode(const unsigned char *instr, unsigned char *outstr);

extern void base64_decode(const unsigned char *instr, unsigned char *outstr);

#ifdef __cplusplus
}
#endif

#endif
