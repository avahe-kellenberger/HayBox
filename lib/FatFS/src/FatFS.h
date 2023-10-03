#include <FS.h>
#include <FSImpl.h>
#include "ff15/ff.h"

using namespace fs;

class FatFsImpl : public FSImpl {
    FatFsImpl(uint8_t *start, uint32_t size, uint32_t pageSize, uint32_t blockSize, uint32_t maxOpenFds)
        : _start(start), _size(size), _pageSize(pageSize), _blockSize(blockSize), _maxOpenFds(maxOpenFds),
          _mounted(false) {
    }

    bool begin() override {
        return false;
    }
protected:
    uint8_t *_start;
    uint32_t _size;
    uint32_t _pageSize;
    uint32_t _blockSize;
    uint32_t _maxOpenFds;

    bool     _mounted;
};

/* from diskio_rp2040_flash.h. Needed for USB operations */
extern void disk_read_direct (BYTE* buff, LBA_t sector, UINT num_bytes);


#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_FATFS)
extern FS FatFS;
#endif