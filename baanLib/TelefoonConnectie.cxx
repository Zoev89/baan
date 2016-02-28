#include "TelefoonConnectie.h"

#include <boost/bind.hpp>


TelefoonConnectie::TelefoonConnectie(std::vector<Regelaar>& regelaars) :
    mRegelaars(regelaars)
{
    socketServer = NULL;
}

TelefoonConnectie::~TelefoonConnectie()
{
    Disconnect();
}

void TelefoonConnectie::Connect()
{
    printf("SocketServer voor telefoon connectie is uit\n");
    // socketServer is nog uit
//    socketServer = new SocketServerCom("5000",boost::bind( &TelefoonConnectie::callBack, boost::ref(this)));
//                                                    //&(*this), _1, _2, _3) );
}

void TelefoonConnectie::callBack(int index, int intData, void * /*data*/)
{
    printf("callback %d data %d\n",index,intData);
    switch(index)
    {
    //                      pInfo->RegelArray[pInfo->HandRegelaar.Index].SetForegroundWindow();

    case SocketData::SETACTIVEREGELAARSNELHEID:
        mRegelaars[m_regelaar].zetGewensteSnelheid(intData);
        break;
    case SocketData::SETACTIVEREGELAARRICHTING:
        if (mRegelaars[m_regelaar].Richting != intData)
        {
            mRegelaars[m_regelaar].RichtingVerandering();
        }
        break;
    }
};

void TelefoonConnectie::Disconnect()
{
    if (socketServer!=NULL)
    {
        delete socketServer;
        socketServer = NULL;
    }
}

void TelefoonConnectie::SetActiveRegelaar(int regelaar, int /*richting*/, int /*snelheid*/)
{
    m_regelaar = regelaar;
    if ((socketServer!=NULL) && (socketServer->GetConnected()==true))
    {
        /*        unsigned char *p=mBaanInfo->RegelArray[m_regelaar].bitmapName->buffer();
        unsigned int *i=(unsigned int *)p;
        int width,height,stride,depth,tiepe;
        width = mBaanInfo->RegelArray[m_regelaar].bitmapName->buffer_width();
        height = mBaanInfo->RegelArray[m_regelaar].bitmapName->buffer_height();
        stride = mBaanInfo->RegelArray[m_regelaar].bitmapName->buffer_linedelta();
        depth = mBaanInfo->RegelArray[m_regelaar].bitmapName->buffer_depth();
        tiepe = mBaanInfo->RegelArray[m_regelaar].bitmapName->buffer_pixeltype();
        printf("w=%d h=%d s=%d d=%d t=%d\n",width,height,stride,depth,tiepe);
        printf("%p %x %x\n",p,*i,i[7]);
        printf("%x %x %x %x\n",p[0],p[1],p[2],p[3]);
        SocketData data;
        data.SetActiveRegelaarSnelheid(snelheid);
        data.SetActiveRegelaarRichting( (richting==0) ? SocketData::VOORUIT : SocketData::ACHTERUIT);
        data.SendNamedBitmap(i,width,height,stride,"Regelaar",m_regelaar);
        socketServer->Send(data);*/
        // redesign telefoon interface hier word de bitmap overgezonden.
    }
}
