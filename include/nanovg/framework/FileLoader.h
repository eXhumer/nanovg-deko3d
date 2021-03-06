/*
** Sample Framework for deko3d Applications
**   FileLoader.h: Helpers for loading data from the filesystem directly into GPU memory
*/
#pragma once
#include "nanovg/framework/common.h"
#include "nanovg/framework/CMemPool.h"

CMemPool::Handle LoadFile(CMemPool& pool, const char* path, uint32_t alignment = DK_CMDMEM_ALIGNMENT);
