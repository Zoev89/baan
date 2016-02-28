#include <stdio.h>

convert2hex(FILE *output, int adres, int count, int buffer[])
{
    int sum,i;
    // 1 byte de size
    // 2 bytes het adres
    // 1 byte 0
    sum = count;
    sum += adres >> 8;
    sum += adres & 0xff;
    fprintf(output,":%02X",count);
    fprintf(output,"%02X",adres >> 8);
    fprintf(output,"%02X00",adres & 0xff);
    // nu de data
    for(i=0;i<count;i++)
    {
        sum += buffer[i];
        fprintf(output,"%02X",buffer[i]);
    }
    // checksom van de gehele data en daar de 2 complement van
    fprintf(output,"%02X\n",0x100 - (sum&0xff));
    
}

int main(int argc,char *argv[])
{
    FILE *input;
    FILE *output;
    int buffer[16],count;
    int adres;
    
    if (argc < 2)
    {
        printf("useagge:\ndec2hex <inputfile> <outputfile>\n");
        return 1;
    }
    input = fopen(argv[1],"r");
    output = fopen(argv[2],"w");
    count = 0;
    adres = 0;
    while(!feof(input))
    {
        if (fscanf(input,"%d,",&buffer[count]) == 1)
        {
        count++;
        if (count == 16)
        {
            convert2hex(output,adres,count,buffer);
            count = 0;
            adres += 16;
        }
        }    
    }
    if (count)
    {
       convert2hex(output,adres,count,buffer);
    }
    fprintf(output,":00000001FF\n");
    fclose(output);
    fclose(input);
            
}
