De range van de getallen die de microcontroller teruggeeft ligt tussen
de [0..255]. Waarbij 0 als kortsluiting word gezien. De 0 spanning ligt
ongeveer op de halve outgangs waarde. Ik neem meestal 126 als de 0 waarde. Dus
als onder de 126 wordt weergegeven staat er een negative spanning op de uitgang
van de microcontroller.

Het trein programma doet bij het opstarten de 0 waardes van alle uitgangen even meten
en slaat die op zodat kleine variaties in componenten worden gecompenseerd.

De waardes 255-25=230 en 25 zijn de default initializatie waardes van
spannings meeting in de controller. Worden weergegeven als 230-126 = 104 
en 25-126 = -101
Als de controller nog niets gemeten
heeft reporteerd die deze een van deze waardes (afhankelijk van vooruit
of terug). Deze waardes worden dan vertaalt in +LEV en -LEV om aan tegeven
dat de uitgang niet actief was.
De kortsluit drempel ligt op 70 tussen de 255-70=185 dus vertaald
70-126=-56 en 185-126=59.

Fouten die ik gezien heb in kaartjes:

Meet tips met ohm meter.
Tussen de uitgangen van de microcontoller moet je ongeveer 60Kohm meten. 

Verbinding tussen PD6 en PD7 (uitgang microcontroller)
De twee uitgangen gedragen zicht als volgt:
Positieve spanning dan gaan beiden uitgangen aan met als gevolg dat de
null spanning van het andere blok ongelijk word aan nul.
Negative spanning dan gaan beiden uitgangen ook aan maar de spanning is dan
blijkbaar niet voldoende dus word kortsluiting gerapporteerd op de uitgang 
die aangestuurd word. Terwijl er een kortsluiging gerapporteerd word is de actuele
spanning meeting nog steeds aftelezen met het blok programma ouder dan 1-1-2012.
Een voorbeeld van de waardes als er tussen 4 en 5 uitgang een verbinding is. 
4 nul    1    1    1    1    1   19 
4 nul    1    1    1    1 SHRT    1 
4 act +LEV +LEV +LEV +LEV   72 +LEV 
4 act -LEV -LEV -LEV -LEV  -55 -LEV 
5 nul    1    1    1    1   19    1 
5 nul    1    1    1    1    1 SHRT 
5 act +LEV +LEV +LEV +LEV +LEV   72 
5 act -LEV -LEV -LEV -LEV -LEV  -53 

Deze volgende fout kwam voor in de voedings lijn van de -15v. Er zat een onderbreking
in de verbinding tussen de voeding van uitgang 4 en 5.
0 nul    1    1    2    2    1    2 
0 nul SHRT    2    2    2    1    2 
0 act   73 +LEV +LEV +LEV +LEV +LEV 
0 act  -56 -LEV -LEV -LEV -LEV -LEV 
1 nul    1    2    2    2    1    2 
1 nul    1 SHRT    2    2    1    2 
1 act +LEV   73 +LEV +LEV +LEV +LEV 
1 act -LEV  -56 -LEV -LEV -LEV -LEV 
2 nul    1    1    2    2    1    2 
2 nul    1    2 SHRT    2    1    2 
2 act +LEV +LEV   73 +LEV +LEV +LEV 
2 act -LEV -LEV  -56 -LEV -LEV -LEV 
3 nul    1    1    2    2    1    2 
3 nul    1    2    2 SHRT    1    2 
3 act +LEV +LEV +LEV   73 +LEV +LEV 
3 act -LEV -LEV -LEV  -56 -LEV -LEV 
4 nul    1    1    2    2    1    2 
4 nul    1    1    2    2    1    2 
4 act +LEV +LEV +LEV +LEV   73 +LEV 
4 act -LEV -LEV -LEV -LEV -123 -LEV 
5 nul    1    1    2    2    1    2 
5 nul    1    1    2    2    1    2 
5 act +LEV +LEV +LEV +LEV +LEV   73 
5 act -LEV -LEV -LEV -LEV -LEV -122 



Volgende fout R65 10K weerstand zat met sluiting op de ground.
0 nul    2    2    2    2    1 SHRT 
0 nul    2    2    2    2    1 SHRT 
0 act   74 +LEV +LEV +LEV +LEV +LEV 
0 act -123 -LEV -LEV -LEV -LEV -LEV 
1 nul    2    2    2    1    1 SHRT 
1 nul    2    2    2    2    1 SHRT 
1 act +LEV   75 +LEV +LEV +LEV +LEV 
1 act -LEV -123 -LEV -LEV -LEV -LEV 
2 nul    2    2    2    2    1 SHRT 
2 nul    2    2    2    1    1 SHRT 
2 act +LEV +LEV   74 +LEV +LEV +LEV 
2 act -LEV -LEV -123 -LEV -LEV -LEV 
3 nul    2    2    2    1    1 SHRT 
3 nul    2    2    2    2    1 SHRT 
3 act +LEV +LEV +LEV   74 +LEV +LEV 
3 act -LEV -LEV -LEV -124 -LEV -LEV 
4 nul    2    2    2    1    1 SHRT 
4 nul    2    2    2    1    1 SHRT 
4 act +LEV +LEV +LEV +LEV   74 +LEV 
4 act -LEV -LEV -LEV -LEV -124 -LEV 
5 nul    2    2    2    2    1 SHRT 
5 nul    2    2    2    2    1 SHRT 
5 act +LEV +LEV +LEV +LEV +LEV   51 
5 act -LEV -LEV -LEV -LEV -LEV -124 

Losse verbinding voeding +15V tussen transitor 2 en 3
0 nul SHRT    2    2    2    1    2 
0 nul    2    2    2    2    1    2 
0 act   55 +LEV +LEV +LEV +LEV +LEV 
0 act -122 -LEV -LEV -LEV -LEV -LEV 
1 nul    2 SHRT    2    2    1    2 
1 nul    2    2    2    2    1    2 
1 act +LEV   55 +LEV +LEV +LEV +LEV 
1 act -LEV -121 -LEV -LEV -LEV -LEV 
2 nul    2    2 SHRT    2    1    2 
2 nul    2    2    2    2    1    2 
2 act +LEV +LEV   55 +LEV +LEV +LEV 
2 act -LEV -LEV -123 -LEV -LEV -LEV 
3 nul    2    2    2    2    1    2 
3 nul    2    2    2    2    1    2 
3 act +LEV +LEV +LEV   73 +LEV +LEV 
3 act -LEV -LEV -LEV -121 -LEV -LEV 
4 nul    2    2    2    2    1    2 
4 nul    2    2    2    2    1    2 
4 act +LEV +LEV +LEV +LEV   72 +LEV 
4 act -LEV -LEV -LEV -LEV -122 -LEV 
5 nul    2    2    2    2    1    2 
5 nul    2    2    2    2    1    2 
5 act +LEV +LEV +LEV +LEV +LEV   73 
5 act -LEV -LEV -LEV -LEV -LEV -122 


10K weerstand van de spannings deler maakt geen contact op blok 3.
Ik meer ongeveer 4V op de spannings deler inplaats van 2.5V. Dit klop
precies met wat ik bereken als de 10K weerstand er niet is
U = 5V * (33+2.2)/(33+2.2+8.2) = 4V
De waarde 92 is ook vrij precies 4V want
u=(92+126)/256 * 4.8 = 4V
Heb 4.8V genomen want de referentie spanning is blijkbaar toch vrij laag.

Er is nog iets wat opvalt is dat de nul spanning te hoog is van de rest van
de blokken. Ze lijken voor de rest wel goed. Dit ligt aan de referentie spanning
van de microcontroller voor de AD converters. De 100 ohm weerstand blijkt in dit
geval teveel tezijn. Het voorstel van Atmel om 100 ohm te gebruiken is ook met de
tijd aangepast om een spoel te gebruiken. In mijn geval past een spoel niet meer
op de layout en heb ik de weestand verlaag van 100 naar 30 ohm. Dit lost dan ook
het probleem van de referentie spanning genoeg op.
0 nul    6    5    5   92    5    6 
0 nul    6    5    5   92    5    6 
0 act   77 +LEV +LEV +LEV +LEV +LEV 
0 act -120 -LEV -LEV -LEV -LEV -LEV 
1 nul    6    5    5   92    5    6 
1 nul    6    5    5   92    5    5 
1 act +LEV   77 +LEV +LEV +LEV +LEV 
1 act -LEV -121 -LEV -LEV -LEV -LEV 
2 nul    6    5    5   92    5    6 
2 nul    6    5    5   92    5    6 
2 act +LEV +LEV   78 +LEV +LEV +LEV 
2 act -LEV -LEV -122 -LEV -LEV -LEV 
3 nul    6    5    5   92    5    5 
3 nul    6    5    5   92    5    6 
3 act +LEV +LEV +LEV  127 +LEV +LEV 
3 act -LEV -LEV -LEV -117 -LEV -LEV 
4 nul    6    5    5   92    5    5 
4 nul    6    5    5   92    5    6 
4 act +LEV +LEV +LEV +LEV   78 +LEV 
4 act -LEV -LEV -LEV -LEV -121 -LEV 
5 nul    6    5    5   92    5    6 
5 nul    6    5    5   92    5    6 
5 act +LEV +LEV +LEV +LEV +LEV   78 
5 act -LEV -LEV -LEV -LEV -LEV -121 

De volgende output heeft een sluiting tussen de 2.5V en de collector van T22
waardoor uitgang 1 altijd open staat. Bij dit printje is er tevens ook een
sluiting tussen de 2.5V en de 33K weerstand van uitgang 1 en van uitgang 2.
0 nul    1   -9    1    1    1    2 
0 nul SHRT   -9    1    1    2    2 
0 act   65 +LEV +LEV +LEV +LEV +LEV 
0 act  -54 -LEV -LEV -LEV -LEV -LEV 
1 nul    1 SHRT    1    1    2    2 
1 nul    1 SHRT    1    1    2    2 
1 act +LEV   58 +LEV +LEV +LEV +LEV 
1 act -LEV  -54 -LEV -LEV -LEV -LEV 
2 nul    1   -9 SHRT    1    2    2 
2 nul    1   -9 SHRT    1    2    2 
2 act +LEV +LEV   58 +LEV +LEV +LEV 
2 act -LEV -LEV  -56 -LEV -LEV -LEV 
3 nul    1   -9    1    1    1    2 
3 nul    1  -10    1 SHRT    2    2 
3 act +LEV +LEV +LEV   66 +LEV +LEV 
3 act -LEV -LEV -LEV  -54 -LEV -LEV 
4 nul    1   -9    1    1    2    2 
4 nul    1   -9    1    1 SHRT    2 
4 act +LEV +LEV +LEV +LEV   65 +LEV 
4 act -LEV -LEV -LEV -LEV  -54 -LEV 
5 nul    1   -9    1    1    1    2 
5 nul    1   -9    1    1    2 SHRT 
5 act +LEV +LEV +LEV +LEV +LEV   66 
5 act -LEV -LEV -LEV -LEV -LEV  -54 

