#include "baanUtility.h"
#include "baan.h"
#include <stdio.h>
#include <string.h>

// copies the fullname into the output when the
// file can be opened.
// returns 0 on succes
int
baanDocFileName (char *output, char *input, char *dir)
{
    char temp[MAX_FILENAME];
    FILE *file;
    int len;

    // TODO add length error checking!
    // gebruik geen strncpy want die add nullen aan het einde als source
    // kleiner is dan N

    // the input is a full path or a default name
    file = fopen (input, "rb");
    if (file)
    {
        // succesfull opened
        strcpy (output, input);
        fclose (file);
        return 0;
    }
    // check if we need to prefix it with the dir
    strcpy (temp, dir);
    len = strlen (dir);
    strcpy (&temp[len], input);
    file = fopen (temp, "rb");
    if (file)
    {
        strcpy (output, temp);
        fclose (file);
        return 0;
    }
    return 1;
}
