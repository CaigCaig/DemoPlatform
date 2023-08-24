#ifndef __THREAD_H
#define __THREAD_H

class Thread {
public:
    Thread();
    
    //void createThread();
    
    virtual void run() = 0;
};





#endif  // __THREAD_H

