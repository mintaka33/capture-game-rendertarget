// capture.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "capture.h"
#include <stdio.h>

#define CHECK_RETURN(ret) \
if (ret != 0) \
{ \
    printf("ERROR: failed call! ret = %d\n", ret); \
    goto _finish; \
}

int main()
{
    int ret = 0;
    CaptureDesktop* pCapture = new CaptureDesktop;
    FrameProp prop = {};

    ret = pCapture->initialize();
    CHECK_RETURN(ret);

    ret = pCapture->getFrame(&prop);
    CHECK_RETURN(ret);

_finish:
    pCapture->destroy();
    return ret;
}

