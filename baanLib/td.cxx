// Baan: A railway model controlling program
// Copyright (C) 1998 - 2001 Eric Kathmann
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

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "td.h"



/////////////////////////////////////////////////////////////////////////////

long long Td::readTSC (void)
{
    long long t;
#if defined(_WIN32)
    unsigned int a, b;
    unsigned int *c = (unsigned int *) &t;
    _asm
    {
        _emit 0x0f;
        _emit 0x31;
        mov a, eax;                 // low
        mov b, edx;                 // high
    }
    c[0] = a;
    c[1] = b;
#else
    struct timespec cur_time;

    clock_gettime (CLOCK_REALTIME, &cur_time);
    t =
            (long long) cur_time.tv_nsec +
            (long long) cur_time.tv_sec * (long long) 1000000000;
    //  __asm__ volatile (".byte 0x0f,0x31":"=A" (t));
#endif

    return t;
}

long long Td::tdClock (void)
{
    long long t;

    t = readTSC ();
    {
        static long long save;
        if (save == 0)
        {
            save = t;
        }
        t = t - save;
    }
    return t;

}



Td::Td(int aantalLogs)
{
    gLogs.resize(aantalLogs);
    gAantalLogs = aantalLogs;
    gKop = 0;
    gStaart = 0;
    gLogging = 1;
}

Td::~Td()
{
}

int Td::tdCreate (const std::string& string)
{
    int id = -1;
    if (tdNameData.size() < (TD_MAX_NAMES))
    {
        id = tdNameData.size();
        tdNameData.push_back(string);
    }
    return id;
}



int Td::tdValueString ( const std::string& string)
{
    int stringId = -1;
    if (tdStringData.size() < TD_MAX_NAMES)
    {
        stringId = tdStringData.size();
        tdStringData.push_back(string);
    }
    return stringId;
}

void Td::tdValue (int id, int value)
{
    assert ((std::size_t)id < tdStringData.size());
    if (gLogging)
    {
        tdData_t *pLogs;

        pLogs = &gLogs[gKop++];
        if (gKop == gAantalLogs)
        {
            gKop = 0;
        }
        if (gKop == gStaart)
        {
            gStaart += 1;
            if (gStaart == gAantalLogs)
            {
                gStaart = 0;
            }
        }
        pLogs->tag = TD_DSC;
        pLogs->id = id;
        pLogs->time = value;
    }
}

void Td::tdStart (int id)
{
    if (gLogging)
    {
        tdData_t *pLogs;


        pLogs = &gLogs[gKop++];
        if (gKop == gAantalLogs)
        {
            gKop = 0;
        }
        if (gKop == gStaart)
        {
            gStaart += 1;
            if (gStaart == gAantalLogs)
            {
                gStaart = 0;
            }
        }
        pLogs->tag = TD_START;
        pLogs->id = id;
        pLogs->time = tdClock ();
    }
}

void Td::tdStop (int id)
{
    if (gLogging)
    {
        tdData_t *pLogs;


        pLogs = &gLogs[gKop++];
        if (gKop == gAantalLogs)
        {
            gKop = 0;
        }
        if (gKop == gStaart)
        {
            gStaart += 1;
            if (gStaart == gAantalLogs)
            {
                gStaart = 0;
            }
        }
        pLogs->tag = TD_STOP;
        pLogs->id = id;
        pLogs->time = tdClock ();
    }
}

void Td::tdDump ()
{
    FILE *file;
    std::size_t x;
    int log;
    long long offset = 0;

    // schakel het loggen uit
    log = gLogging;
    gLogging = 0;
    file = fopen ("baan.tdi", "w");
    fprintf (file, "CPU x86\n");
    fprintf (file, "TIME 1000000000\n");  // tijd referentie in ns op linux
    fprintf (file, "SPEED 1000000000\n");
    fprintf (file, "MEMSPEED 1000\n");

    for (x = 0; x < tdNameData.size(); x++)
    {
        if (tdNameData[x].size() != 0)
        {
            fprintf (file, "NAM 0 %zu %s\n", x, tdNameData[x].c_str());
        }
    }
    for (x = 0; x < tdStringData.size(); x++)
    {
        if (tdStringData[x].size() != 0)
        {
            fprintf (file, "DNM 0 %zu %s\n", x, tdStringData[x].c_str());
        }
    }

    // alleen als er een logging was
    offset = gLogs[gStaart].time;
    for (x = 0; (x < gAantalLogs) && (gStaart != gKop); x++)
    {
        tdData_t *pLogs;

        pLogs = &gLogs[gStaart++];
        if (gStaart == gAantalLogs)
        {
            gStaart = 0;
        }

        switch (pLogs->tag)
        {
        case TD_START:
            fprintf (file, "STA 0 %d %-10.9lg\n", pLogs->id,
                     (double) (pLogs->time - offset));
            break;
        case TD_STOP:
            fprintf (file, "STO 0 %d %-10.9lg\n", pLogs->id,
                     (double) (pLogs->time - offset));
            break;
        case TD_DSC:
            fprintf (file, "DSC 0 %d %lg\n", pLogs->id, (double) pLogs->time);
            break;
        case TD_NAM:
            assert(0);
            break;

        }
    }
    fprintf (file, "END\n");
    fclose (file);
    gStaart = 0;
    gKop = 0;
    gLogging = log;
}
