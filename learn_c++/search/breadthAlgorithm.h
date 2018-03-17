#ifndef _BREADTHALGORITHM_H_
#define _BREADTHALGORITHM_H_

#include "baseAlgorithm.h"

class breadthAlgorithm : public baseAlgorithm
{
    public:
        breadthAlgorithm();
        ~breadthAlgorithm();
        void search() override;
    private:

    protected:
};

#endif /* _BREADTHALGORITHM_H_ */