#ifndef CTHINGSLOOPTHREAD_H
#define CTHINGSLOOPTHREAD_H
#include <QThread>
#include "src/include/things.h"

class ThingsLoopThread : public QThread
{
    Q_OBJECT
public:
    ThingsLoopThread(Things* things);
   ~ ThingsLoopThread();

    void stopRun();
protected:
    void run();

public :
    bool m_bRunFlg;
    Things* m_pThings;

public :


};

#endif // CTHINGSLOOPTHREAD_H
