#pragma once

#include "Log/SimpleLog.h"

// ---------------------------------------------------------------------------
// HResultToString — HRESULT error code to readable string
// ---------------------------------------------------------------------------
inline const char* HResultToString(HRESULT hr)
{
    switch (hr)
    {
    // Success
    case S_OK:                              return "S_OK";
    case S_FALSE:                           return "S_FALSE";

    // Common COM errors
    case E_INVALIDARG:                      return "E_INVALIDARG - Invalid argument";
    case E_OUTOFMEMORY:                     return "E_OUTOFMEMORY - Out of memory / VRAM";
    case E_NOTIMPL:                         return "E_NOTIMPL - Not implemented";
    case E_FAIL:                            return "E_FAIL - Unspecified error";
    case E_ACCESSDENIED:                    return "E_ACCESSDENIED - Access denied";
    case E_POINTER:                         return "E_POINTER - Invalid pointer";
    case E_ABORT:                           return "E_ABORT - Operation aborted";
    case E_HANDLE:                          return "E_HANDLE - Invalid handle";

    // DXGI errors
    case DXGI_ERROR_INVALID_CALL:           return "DXGI_ERROR_INVALID_CALL";
    case DXGI_ERROR_NOT_FOUND:              return "DXGI_ERROR_NOT_FOUND";
    case DXGI_ERROR_DEVICE_REMOVED:         return "DXGI_ERROR_DEVICE_REMOVED - GPU removed (driver update / crash / overheat)";
    case DXGI_ERROR_DEVICE_HUNG:            return "DXGI_ERROR_DEVICE_HUNG - GPU hung";
    case DXGI_ERROR_DEVICE_RESET:           return "DXGI_ERROR_DEVICE_RESET - GPU reset";
    case DXGI_ERROR_DRIVER_INTERNAL_ERROR:  return "DXGI_ERROR_DRIVER_INTERNAL_ERROR - Driver internal error";
    case DXGI_ERROR_WAS_STILL_DRAWING:      return "DXGI_ERROR_WAS_STILL_DRAWING - GPU still drawing";
    //case DXGI_ERROR_NON_REDIRECTED_APPLICATION: return "DXGI_ERROR_NON_REDIRECTED_APPLICATION";

    // D3D12 errors
    case D3D12_ERROR_DRIVER_VERSION_MISMATCH:   return "D3D12_ERROR_DRIVER_VERSION_MISMATCH";
    case D3D12_ERROR_ADAPTER_NOT_FOUND:         return "D3D12_ERROR_ADAPTER_NOT_FOUND";

    default:
        return nullptr;  // Unknown error code, caller falls back to hex
    }
}

// ---------------------------------------------------------------------------
// ANALYSIS_HRESULT macro
//   - do { } while(0) wrapper prevents dangling-else issues
//   - Prints hex 0x%08X instead of decimal %i (easier to look up on MSDN)
//   - Shows __FUNCTION__ and __LINE__ for call-site location
//   - HResultToString resolves common error codes to readable descriptions
//   - No success log (S_OK is normal, not noise)
// ---------------------------------------------------------------------------
#define ANALYSIS_HRESULT(InValue) \
    do { \
        const HRESULT _hrResult = (InValue); \
        if (FAILED(_hrResult)) \
        { \
            const char* _hrName = HResultToString(_hrResult); \
            if (_hrName) \
            { \
                EG_LOG_ERROR("[%s:%d] HRESULT = 0x%08X (%s)", \
                             __FUNCTION__, __LINE__, \
                             (unsigned int)_hrResult, _hrName); \
            } \
            else \
            { \
                EG_LOG_ERROR("[%s:%d] HRESULT = 0x%08X", \
                             __FUNCTION__, __LINE__, \
                             (unsigned int)_hrResult); \
            } \
        } \
    } while(0)