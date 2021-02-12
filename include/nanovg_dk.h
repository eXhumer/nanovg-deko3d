#pragma once

#include "nanovg/dk_renderer.hpp"

#ifdef __cplusplus
extern "C" {
#endif

NVGcontext* nvgCreateDk(nvg::DkRenderer *renderer, int flags);
void nvgDeleteDk(NVGcontext* ctx);

#ifdef __cplusplus
}
#endif