#ifdef __MINGW32__

#include "../mingw-compat-threads/mingw.thread.h"
#include "../mingw-compat-threads/mingw.mutex.h"
#include "../mingw-compat-threads/mingw.condition_variable.h"

#else

#include <thread>
#include <mutex>
#include <condition_variable>

#endif
