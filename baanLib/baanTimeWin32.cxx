// Baan: A railway model controlling program
// Copyright (C) 1998 - 2007 Eric Kathmann
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA  02111-1307, USA.

#include "baanTime.h"
#include <windows.h>

unsigned int baanTimeGetInms()
{
	SYSTEMTIME systemTime;
	FILETIME ft;
	unsigned int msec;
	ULARGE_INTEGER temp;

	GetSystemTime(&systemTime);
	SystemTimeToFileTime(&systemTime, &ft);  
	temp.LowPart = ft.dwLowDateTime;
	temp.HighPart = ft.dwHighDateTime;
	msec=(unsigned int)(temp.QuadPart /10000);
	return msec;
}

unsigned int baanTimeGetInus()
{
	SYSTEMTIME systemTime;
	FILETIME ft;
	unsigned int usec;
	ULARGE_INTEGER temp;

	GetSystemTime(&systemTime);
	SystemTimeToFileTime(&systemTime, &ft);  
	temp.LowPart = ft.dwLowDateTime;
	temp.HighPart = ft.dwHighDateTime;
	usec=(unsigned int)(temp.QuadPart /10);
	return usec;
}

