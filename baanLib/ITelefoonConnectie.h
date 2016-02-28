#ifndef ITELEFOONCONNECTIE_H
#define ITELEFOONCONNECTIE_H

class ITelefoonConnectie
{
public:
    ~ITelefoonConnectie() = default;

    virtual void Connect(void) = 0;
    virtual void Disconnect(void) = 0;
    virtual void SetActiveRegelaar(int regelaar, int richting, int snelheid) = 0;
};



#endif // ITELEFOONCONNECTIE_H


