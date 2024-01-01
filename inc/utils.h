#pragma once

#include "server.h"

class AutoLock {
public:
    AutoLock(std::mutex& mutex): m_mutex(mutex) {
        m_mutex.lock();
    }

    ~AutoLock() {
        m_mutex.unlock();
    }

private:
    std::mutex& m_mutex;
};