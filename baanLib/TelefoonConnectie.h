#ifndef TELEFOONCONNECTIE_H
#define TELEFOONCONNECTIE_H
#include "ITelefoonConnectie.h"
#include "socketCom.h"
#include "baanTypes.h"
#include "regelaar.h"

class TelefoonConnectie : public ITelefoonConnectie
{
public:
    TelefoonConnectie(std::vector<Regelaar>& regelaars);
    ~TelefoonConnectie();

    virtual void Connect(void);
    virtual void Disconnect(void);
    virtual void SetActiveRegelaar(int regelaar, int richting, int snelheid);
private:
    void callBack(int index, int intData, void *data);
    SocketServerCom *socketServer;
    int m_regelaar;
    std::vector<Regelaar>& mRegelaars;
};



#endif // TELEFOONCONNECTIE_H
