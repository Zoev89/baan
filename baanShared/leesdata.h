#ifndef _LEESDATA_H_
#define _LEESDATA_H_

char *EricFgets (char *String, int MaxSize, FILE * file);

// voor error reporting is het handig om de exacte line nummers te hebben
int EricFgetsGetLineCount(FILE *file);
// zonder dat je een file hebt kan je de laatste count toch ophalen
int EricFgetsGetLastLineCount();
void EricFgetsSetLineCount(FILE *file, int count);
// Sluit niet de file af maar verwijderd hem van de tabel
void EricFgetsClose(FILE *file);

#endif // _LEESDATA_H_

