// overlay.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "overlay.h"

static int ret = 0;

#define CHECK_RETURN(fun) \
ret = fun; \
if (ret != 0) \
{ \
    printf("ERROR: failed call! ret = %d\n", ret); \
    goto _finish; \
}

int main()
{
    YuvOverlay* overlay = new YuvOverlay("out.yuv", 1920, 1080, "yuv420p");

    CHECK_RETURN(overlay->initialize());

    for (int i=0; i<overlay->getFrameNum(); i++)
    {
        CHECK_RETURN(overlay->overlayFrame(i, i));
    }

_finish:
    CHECK_RETURN(overlay->destroy());
    delete overlay;
    return 0;
}

