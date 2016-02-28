#ifndef IVIEWINTERNAL_H
#define IVIEWINTERNAL_H

class IViewInternal
{
public:
    ~IViewInternal() = default;
    virtual void Update(int type, int value1, int value2, int value3) = 0;

};

#endif // IVIEWINTERNAL_H
