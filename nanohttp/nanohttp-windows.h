/******************************************************************
 *  $Id: nanohttp-windows.h,v 1.1 2004/08/31 16:34:57 rans Exp $
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
 * Email: ayaz@jprogrammer.net
 ******************************************************************/
#ifndef NANO_HTTP_WINDOWS_H 
#define NANO_HTTP_WINDOWS_H 

#ifdef WIN32

static void WSAReaper(void *x)
{
	short int connections;
	short int i;
	char junk[10];
	int rc;
	time_t ctime;

	for (;;) {
		connections=0;
		ctime=time((time_t)0);
		for (i=0;i<_httpd_max_connections;i++) {
 			if (_httpd_connection[i].tid==0) continue;
			GetExitCodeThread((HANDLE)_httpd_connection[i].tid, &rc);
			if (rc!=STILL_ACTIVE) continue;
			connections++;
			if ((ctime-_httpd_connection[i].atime<_httpd_max_idle)||
				(_httpd_connection[i].atime==0)) continue;
			log_verbose3("Reaping socket %u from (runtime ~= %d seconds)", 
				_httpd_connection[i].sock, ctime-_httpd_connection[i].atime);
			shutdown(_httpd_connection[i].sock, 2);
			while (recv(_httpd_connection[i].sock, junk, sizeof(junk), 0)>0) { };
			closesocket(_httpd_connection[i].sock);
			TerminateThread(_httpd_connection[i].tid, (DWORD)&rc);
			CloseHandle(_httpd_connection[i].tid);
			memset((char *)&_httpd_connection[i], 0, sizeof(_httpd_connection[i]));
		}
		Sleep(100);
	}
	return;
}

#endif

#endif
