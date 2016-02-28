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
#ifndef TD_H
#define TD_H

/////////////////////////////////////////////////////////////////////////////
// 
#include "ITd.h"
#include <vector>

class Td : public ITd
{
public:
    Td(int aantalLogs);
    ~Td();

    virtual int tdCreate (const std::string& string);

    virtual void tdStart (int id);
    virtual void tdStop (int id);

    virtual int tdValueString (const std::string& string);
    // print een getal in de logging
    // precondition is het aanmelden met tdValueString
    virtual void tdValue (int stringId, int value);

    virtual void tdDump ();
    const std::size_t TD_MAX_NAMES=100;
private:
    enum tag_t
    {
        TD_START,
        TD_STOP,
        TD_NAM, // not used in my implementation
        TD_DSC,
    };

    struct tdData_t
    {
        tag_t tag;
        int id;
        long long time;
    };

    long long readTSC (void);
    long long tdClock (void);

    std::size_t gAantalLogs;
    std::size_t gKop;
    std::size_t gStaart;
    int gLogging;
    std::vector<tdData_t> gLogs;
    std::vector<std::string> tdNameData;
    std::vector<std::string> tdStringData;
};











#endif // TD_H

