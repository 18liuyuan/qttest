#include "thingsloopthread.h"
#include <QDebug>

ThingsLoopThread::ThingsLoopThread(Things *things)
{
    m_pThings = things;
    m_bRunFlg = false;
}

ThingsLoopThread::~ThingsLoopThread()
{
    m_bRunFlg = false;
}

void ThingsLoopThread::stopRun()
{
     m_bRunFlg = false;
}


void ThingsLoopThread::run()
{
    m_bRunFlg = true;

    while(m_bRunFlg){
        //qDebug()<<"loop";
        m_pThings->loop();
        QThread::msleep(1000);
    }

}
