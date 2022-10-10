#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <winerror.h>

#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(X) if(X){X->Release(); X=nullptr;}
#endif

#define returnIfError(x)\
    if (FAILED(x))\
    {\
        return x;\
    }

#endif // !DEFINITIONS_HPP