#include "socketCom.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <boost/lexical_cast.hpp>

SocketServerCom::SocketServerCom(string socketPoort, callBack cb) : 
    m_socketPoort(socketPoort)
{
    m_stop = false;
    m_close = false;
    m_connected = false;
    m_socket = -1;
    m_socketChannel = -1;
    m_callBack = cb;
}

void SocketServerCom::ReadChannel(void)
{
    printf("readStart\n");
    SocketData data;
    char ontvangenData[256];
    ontvangenData[0] = 0;
    int ontvangenDataIndex = 0;
    while(m_stop==false)
    {
        char buffer[256];
        bzero(buffer,256);
        int n = read(m_socketChannel,buffer,255);
        if (n <= 0) 
        {
            printf("ERROR reading from socket\n");
            m_stop = true;
            SocketData data;
            data.SetStopProcessing();
            m_queue.Push(data);
        }
        else
        {
            for (int i=0;i<n;i++)
            {
                if (ontvangenData[0] == 0)
                {
                    if (buffer[i] == '<')
                    {
                        ontvangenData[0] = '<';
                        ontvangenDataIndex = 1;
                    }
                }
                else if (buffer[i] == '>')
                {
                    ontvangenData[ontvangenDataIndex++] = '>';
                    ontvangenData[ontvangenDataIndex] = 0;

                    printf("Here is the message: <%s> len=%ul\n",ontvangenData,strlen(ontvangenData));
                    data.ProcessReceivedBuffer(ontvangenData,m_callBack);
                    ontvangenData[0] = 0;
                }
                else
                {
                    ontvangenData[ontvangenDataIndex++] = buffer[i];
                }
            }
        }
    }
    printf("readEnd\n");
}
void SocketServerCom::WriteChannel(void)
{
    printf("writeStart\n");
    // Enable address reuse
    while (m_close == false)
    {
        if (m_socket == 0)
        {
    printf("writeStart again\n");
            m_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (m_socket < 0)
            {
                printf("ERROR opening socket");
            }
        }
    int on = 1;
    int ret = setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );     
    if (ret != 0)
    {
        printf("ERROR setting socketOptions");
        return;
    }

    struct sockaddr_in serv_addr, cli_addr;

    bzero((char *) &serv_addr, sizeof(serv_addr));
    int portno = atoi(m_socketPoort.c_str());
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(m_socket, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
    {
        printf("ERROR on binding");
    }
    listen(m_socket,5);
    socklen_t clilen = sizeof(cli_addr);
    m_socketChannel = accept(m_socket, (struct sockaddr *) &cli_addr, &clilen);
    if (m_socketChannel < 0) 
    {
        printf("ERROR on accept");
    }
        
    m_read = boost::thread ( boost::bind( &SocketServerCom::ReadChannel, this ) );
    m_connected = true;
    while(m_stop==false)
    {
        SocketData waarde;
        m_queue.WaitAndPop(waarde);
        if ((m_stop == false) && (waarde.GetStopProcessing() == false) )
        {
            string retString; 
            int n = 1;
            while (((retString = waarde.GetStrings()) != "") && (n > 0))
            {
                n = write(m_socketChannel,retString.c_str(),retString.length());
                if (n <= 0)
                {
                    printf("Error writing data end\n");
                }
            }
        }
    }
    m_connected = false;
    shutdown(m_socketChannel,SHUT_RDWR);
    shutdown(m_socket,SHUT_RDWR);
    close(m_socketChannel);
    close(m_socket);
    m_socket = 0;
    m_read.join();
    m_stop = false;
    }
    printf("writeEnd\n");
}


void SocketServerCom::Connect(void)
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
        printf("ERROR opening socket");
        return;
    }
    m_write = boost::thread ( boost::bind( &SocketServerCom::WriteChannel, this) );
}

void SocketServerCom::Disconnect(void)
{
    m_close = true;
    m_stop = true;
    SocketData waarde;
    waarde.SetStopProcessing();
    m_queue.Push(waarde);
    shutdown(m_socketChannel,SHUT_RDWR);
    shutdown(m_socket,SHUT_RDWR);
    close(m_socketChannel);
    close(m_socket);
    m_write.join();
    m_socketChannel = -1;
    m_socket = -1;
}

void SocketServerCom::Send(SocketData &data)
{
    m_queue.Push(data);
}

bool SocketServerCom::GetConnected()
{
    return m_connected;
}

SocketServerCom::~SocketServerCom()
{
    Disconnect();
}

SocketData::SocketData()
{
    m_stopProcessing = false;
    m_index = 0;
    for(unsigned int i=0;i<sizeof(m_whichData)/sizeof(m_whichData[0]);i++)
    {
        m_whichData[i] = false;
    }
}


void SocketData::SetActiveRegelaarSnelheid(int waarde)
{
    m_activeSnelheid = waarde;
    m_whichData[SETACTIVEREGELAARSNELHEID] = true;
}

void SocketData::SetActiveRegelaarRichting(Richting voorTerug)
{
    m_activeRichting = voorTerug;
    m_whichData[SETACTIVEREGELAARRICHTING] = true;
}

int SocketData::GetActiveRegelaarSnelheid()
{
    return m_activeSnelheid;
}

char SocketData::ConvertToChar(int value)
{
    int byte;
    byte = value +'@';
    if ((byte>'Z') && (byte<'`'))
    {
        byte = byte-'Z'-1+'0';
    }
    if (byte>'z')
    {
        byte = byte-'z'-1+'5';
    }
    return (char)byte;
}

string  SocketData::ConvertARGBToString(unsigned int argb)
{
    char buf[5];
    int b = (argb & 0xfc0000) >> 18;
    buf[0] = ConvertToChar(b);
    b = (argb & 0x03f000) >> 12;
    buf[1] = ConvertToChar(b);
    b = (argb & 0x000fc0) >> 6;
    buf[2] = ConvertToChar(b);
    b = (argb & 0x00003f);
    buf[3] = ConvertToChar(b);
    buf[4] = 0;
    return buf;
}
string SocketData::GetStrings()
{
    string value;
    if ((m_index == SETACTIVEREGELAARSNELHEID) && (m_whichData[m_index++]))
    {
        value = "<activeRegelaarSnelheid value=\"";
        value += boost::lexical_cast<std::string>(m_activeSnelheid);
        value += "\"/>\n";
    } 
    else if ((m_index == SETACTIVEREGELAARRICHTING) && (m_whichData[m_index++]))
    {
        value = "<activeRegelaarRichting value=\"";
        value += boost::lexical_cast<std::string>(m_activeRichting);
        value += "\"/>\n";
    }
    else if ((m_index == SENDBITMAP) && (m_whichData[m_index++]))
    {
        char buf[256];
        sprintf(buf,"<bitmap width=\"%d\" heigth=\"%d\" index=\"%d\" name=\"%s\"/>\n",
           m_width,m_height,m_bitmapIndex,m_bitmapName.c_str());
        value = buf;
        m_whichData[SENDBITMAPINTERNALDATA]=true;
        m_line =0;
        m_pixelPosition = 0;
    }
    else if ((m_index == SENDBITMAPINTERNALDATA) && (m_whichData[m_index++]))
    {
        m_index-=1;
        value = "<p v=\"";
        int endPosition = m_pixelPosition+40;
        endPosition = (endPosition>=m_width) ? m_width-1: endPosition;
        for(int i=m_pixelPosition;i<=endPosition;i++)
        {
            printf("%d",m_argb[m_line*m_stride+i]);
            
            value+=ConvertARGBToString(m_argb[m_line*m_stride+i]);
        }
        m_pixelPosition = endPosition+1;
        if (endPosition == (m_width-1))
        {
            m_pixelPosition = 0;
            m_line+=1;
            if (m_line == m_height)
            {
                m_index += 1; // stop the loop
            }
        }
        value += "\"/>\n";
        printf("%s\n",value.c_str());
        
    }
    return value;
}

void SocketData::SetStopProcessing()
{
    m_stopProcessing = true;
}

bool SocketData::GetStopProcessing()
{
    return m_stopProcessing;
}

bool SocketData::findIntergerValue(char *buf,int *value)
{
    bool retval = false;
    char *p = strchr(buf,'\"');
    if (p!=NULL)
    {
        p++;
        if (sscanf(p,"%d", value) == 1)
            retval = true;
    }
    return retval;
}

void SocketData::SendNamedBitmap(unsigned int *argb, int width, int height, int stride, string name, int index)
{
    m_argb = argb;
    m_width = width;
    m_height = height;
    m_stride = stride/4;
    m_bitmapName = name;
    m_bitmapIndex = index;
    m_whichData[SENDBITMAP] = true;
}


void SocketData::ProcessReceivedBuffer(char *buf, callBack cb)
{
    char keyword[256];
    if (sscanf(buf,"%s",keyword)==1)
    {
        if (strcmp(keyword,"<activeRegelaarSnelheid") == 0)
        {
            if ((cb.empty() == false) && (findIntergerValue(buf, &m_activeSnelheid)))
            {
                cb(SETACTIVEREGELAARSNELHEID, m_activeSnelheid, NULL);
            }
        }
        if (strcmp(keyword,"<activeRegelaarRichting") == 0)
        {
            int value;
            if ((cb.empty() == false) && (findIntergerValue(buf, &value)))
            {
                cb(SETACTIVEREGELAARRICHTING, value, NULL);
            }
        }
    }
}
