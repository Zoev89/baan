#ifndef IPROGRAMMA_H
#define IPROGRAMMA_H
#include <boost/filesystem.hpp>

class IProgramma
{
public:
    virtual boost::filesystem::path GetProgrammaNaam() const = 0;
    virtual void HerlaadProgramma(const boost::filesystem::path& programmaNaam, int regelaarNummer) = 0;

protected:
    virtual ~IProgramma() = default;
};


#endif // IPROGRAMMA_H
