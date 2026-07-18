#pragma once

#define ANALYSIS_HRESULT(InValue) \
{\
    if(FAILED(InValue))\
    {\
        EG_LOG_ERROR("Error = %i", (int)InValue);\
    }\
    else if(SUCCEEDED(InValue))\
    {\
        EG_LOG_SUCCESS("Success = %i", (int)InValue);\
    }\
}
