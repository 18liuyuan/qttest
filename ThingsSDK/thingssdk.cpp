#include "ThingsSDK.h"

#include "process.h"
#include <QThread>
#include "thingsloopthread.h"
#include <QDebug>

ThingsSDK::ThingsSDK()
{
    m_pLoopThread = NULL;
    m_pLoginCallback = NULL;
    m_nLoginStatus = 0;

    Window_socket *socket = new Window_socket();
    m_pThings  = new Things( socket, MAX_BUF_IN, MAX_BUF_OUT, MTU);
    m_pThings->on(ON_EVENT, on_things_event, this);
    m_pThings->on(ON_REQUEST, on_things_request, NULL);
    m_pThings->on(ON_POST, on_things_post, NULL);
    start();

}

ThingsSDK::~ThingsSDK(){

    stop();

    delete m_pThings;
    m_pThings = NULL;


    delete m_pLoopThread;
    m_pLoopThread = NULL;
}



void ThingsSDK::on_things_event(Things *this_t, T_EVENT event, void *data, void *context)
{

    switch (event) {
    case E_SERVER_SERVICE_NOT_AVAILABLE:
    case E_SERVER_VERSION_NOT_SUPPORT:
    case E_SERVER_RESPONSE_ERROR:
        qDebug()<<("Invalid server\n");
        break;
    case E_SERVER_NOT_READY:
        qDebug()<<("server not ready\n");
        break;
    case E_CONNECTED :
      //  t->login(username, password);
        qDebug()<<("connected\n");
        // t->login("yy", "1234");
        break;
    case E_CONNECT_FAIL :
        qDebug()<<("connecte fail\n");
        break;
    case E_AUTHED :
        qDebug()<<("authed\n");
       // t->request(".", "/get_profile", &on_response_get_profile, NULL);
        // t->request(".", "/get_parts", &on_response_get_parts, NULL);
        break;
    case E_CONNECTING:
        qDebug()<<("connecting\n");
        break;
    case E_DISCONNECTED:
        qDebug()<<("disconnected\n");
        break;
    case E_RESPAWN:
        qDebug()<<("respawn\n");
        break;
    case E_RESPAWNED:
        qDebug()<<("respawned\n");
        break;
    case E_NEED_RECONNECT:
        qDebug()<<("need reconnect\n");
        break;
    case E_AUTH_FAIL:
        qDebug()<<("auth fail\n");
       // do_login();
       // t->login(username, password);
        break;
    case E_FOLLOWED_NEW:   // event = VAR_EVENT
        // printf("\nfollowed new : tid = %s  path = %s\n", ve->tid, ve->path);
        break;
    case E_FOLLOWED_RESET: // event = VAR_EVENT
        qDebug()<<("\nfollowed reset : tid =   path = \n" );
        break;
    case E_FOLLOWED_BEFORE_UPDATE: // event = VAR_EVENT
    case E_FOLLOWED_AFTER_UPDATE: // event = VAR_EVENT
        qDebug()<<("\nfollowed update : tid =   path = \n");
        break;
    case E_FOLLOWED_LOST_SYNC: // event = VAR_EVENT
        qDebug()<<("\nfollowed lost sync : tid =   path = \n");
        break;
    case E_FOLLOWED_BEFORE_REMOVE: // event = VAR_EVENT
    case E_FOLLOWED_AFTER_REMOVE: // event = VAR_EVENT
        qDebug()<<("\nfollowed remove : tid =   path = \n");
        break;
    case E_FOLLOWED_BEFORE_START:
    case E_FOLLOWED_START:
       // system("date");

        qDebug()<<("\nfollowed start: count = %d\n", *(int*)data);
        break;
    case E_FOLLOWED_END: // event = VAR_EVENT
       // system("date");
        qDebug()<<("\nfollowed end\n");
        // char url[1000];
        // sprintf(url, "/get_profile");
        // t->request(".", url, on_response, NULL);
        break;
    case E_SERVER_TIME:
        qDebug()<<("server time: %s\n", (char*)data);
        break;
}

    ThingsSDK* pSDK = (ThingsSDK*)context;

    switch (event) {
        case E_CONNECTED :
        { //连接成功
            //qDebug()<<QString("连接成功").toStdString();
            qDebug()<<QString("E_CONNECTED");

            this_t->login(pSDK->m_sUser.toLatin1().data(), pSDK->m_sPwd.toLatin1().data());


            break;
        }
        case E_CONNECT_FAIL:
        {//连接失败


            pSDK->m_nLoginStatus = 0;
            qDebug()<<QString("E_CONNECT_FAIL");
            pSDK->m_pLoginCallback(E_CONNECT_FAIL, pSDK->m_pLoginContext);
            //qDebug()<<QString("连接失败").toStdString();;
            break;
        }
        case E_AUTHED : //认证通过
        {
             pSDK->m_nLoginStatus = 2;
             pSDK->m_pLoginCallback(E_AUTHED,pSDK->m_pLoginContext);
            qDebug()<<QString("E_AUTHED");
            break;
        }
        case E_AUTH_FAIL: //认证失败
        {

            pSDK->m_nLoginStatus = 0;
            pSDK->m_pLoginCallback(E_AUTH_FAIL,pSDK->m_pLoginContext);
             qDebug()<<"E_AUTH_FAIL";
            break;
        }
    }
}

void ThingsSDK::on_things_request(Things *this_t, void *data, void *context)
{

}

void ThingsSDK::on_things_post(Things *this_t, void *data, void *context)
{

}



int ThingsSDK::getLoginStatus()
{
    return m_nLoginStatus;
}

QString ThingsSDK::getLoginName(){
    if(m_nLoginStatus == 2){
        return m_sUser;
    } else {
        return "";
    }
}

int ThingsSDK::start()
{
    if(m_pLoopThread == NULL){
        m_pLoopThread =  new ThingsLoopThread(m_pThings);
    }
    m_pLoopThread->start();


}

int ThingsSDK::stop()
{
    if(m_pLoopThread != NULL){
        m_pLoopThread->stopRun();
    }

}




int ThingsSDK::login(char* ip, int port, char* user, char* pwd, LPLOGINCALLBACK callback, void* context){
    char cSvrAddr[255] = {0};
    m_sIp = ip;
    m_sUser = user;
    m_sPwd = pwd;
    m_nPort = port;
    m_pLoginCallback = callback;
    m_pLoginContext = context;
//    char* host = "192.168.1.154";
//    int port = 8008;
    sprintf(cSvrAddr,"host:%s;port:%d",ip,port);


    m_pThings->connect_to(cSvrAddr);
    m_nLoginStatus = 1;

}

int ThingsSDK::logout()
{
    return 0;
}


