#include <common/freertos_mutex.hpp>

#include <cassert>
#include <cstdlib>

// FreeRTOS.h must be included before semphr.h
#include <FreeRTOS.h>
#include <semphr.h>

// Do not check the concept on boards where #include <mutex> fills FLASH
#include <device/board.h>
#if !defined(BOARD_IS_MODULARBED) && !defined(BOARD_IS_DWARF)
    #include <common/concepts.hpp>
static_assert(concepts::Lockable<freertos::Mutex>);
#endif

namespace freertos {

// If these asserts start failing, go fix the Storage definition
static_assert(sizeof(Mutex::Storage) == sizeof(StaticSemaphore_t));
static_assert(alignof(Mutex::Storage) == alignof(StaticSemaphore_t));

static SemaphoreHandle_t handle_cast(Mutex::Storage &mutex_storage) {
    return static_cast<SemaphoreHandle_t>(static_cast<void *>(&mutex_storage));
}

Mutex::Mutex() {
    SemaphoreHandle_t semaphore = xSemaphoreCreateMutexStatic(reinterpret_cast<StaticSemaphore_t *>(&mutex_storage));
    // We are creating static FreeRTOS object here, supplying our own buffer
    // to be used by FreeRTOS. FreeRTOS constructs an object in that memory
    // and gives back a handle, which in current version is just a pointer
    // to the same buffer we provided. If this ever changes, we will have to
    // store the handle separately, but right now we can just use the pointer
    // to the buffer instead of the handle and save 4 bytes per instance.
    configASSERT(semaphore == handle_cast(mutex_storage));
}

Mutex::~Mutex() {
#if INCLUDE_xSemaphoreGetMutexHolder == 1
    assert(xSemaphoreGetMutexHolder(handle_cast(mutex_storage)) == nullptr);
#endif
    vSemaphoreDelete(handle_cast(mutex_storage));
}

void Mutex::unlock() {
    if (xSemaphoreGive(handle_cast(mutex_storage)) != pdTRUE) {
        // Since the semaphore was obtained correctly, this should never happen.
        std::abort();
    }
}

bool Mutex::try_lock() {
    return xSemaphoreTake(handle_cast(mutex_storage), 0) == pdTRUE;
}

void Mutex::lock() {
    if (xSemaphoreTake(handle_cast(mutex_storage), portMAX_DELAY) != pdTRUE) {
        static_assert(INCLUDE_vTaskSuspend);
        // Since we are waiting forever and have task suspension, this should never happen.
        std::abort();
    }
}

} // namespace freertos
