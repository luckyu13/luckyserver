#pragma once

#include <string>

class AutoLock {
public:
    AutoLock(std::mutex& mutex): m_mutex(mutex) {  m_mutex.lock(); }
    ~AutoLock() { m_mutex.unlock(); }

public:
    void lock() { m_mutex.lock(); }
    void unlock() { m_mutex.unlock(); }

private:
    std::mutex& m_mutex;
};

static bool str_eq(const std::string& s1, const std::string& s2) {
    if (s1.length() != s2.length()) {
        return false;
    }
    for (size_t i=0; i<s1.length(); ++i) {
        if (tolower(s1[i] != s2[i])) return false;
    }
    return true;
}
