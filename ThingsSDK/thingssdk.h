#ifndef ThingsSDK_H
#define ThingsSDK_H

#include "thingssdk_global.h"

#include "src/include/things.h"
#include "src/include/conn_win_socket.h"
#include "thingsloopthread.h"
#include <QString>

#define MAX_BUF_IN             20000
#define MAX_BUF_OUT            2000
#define MTU                    1000

/*code=12成功,其它失败，错误码common.h的T_EVENT*/
typedef void(__stdcall* LPLOGINCALLBACK)(int code, void* context);

class THINGSSDKSHARED_EXPORT ThingsSDK
{

public:
    ThingsSDK();
    ~ThingsSDK();
public:
    int getLoginStatus();
    QString getLoginName();
    int start();
    int stop();
    int login(char* ip, int port, char* user, char* pwd, LPLOGINCALLBACK callback, void* context);
    int logout();
    static void on_things_event(Things *this_t, T_EVENT event, void *data, void *context);
    static void on_things_request(Things *this_t, void *data, void *context);
    static void on_things_post(Things *this_t, void *data, void *context);
private:
    Things* m_pThings;
    ThingsLoopThread *m_pLoopThread;

public:
    QString m_sIp;
    QString m_sUser;
    QString m_sPwd;
    int m_nPort;

    /*登录状态，0未登录，1正在登陆，2已登录*/
    int m_nLoginStatus;

    LPLOGINCALLBACK m_pLoginCallback;
    void* m_pLoginContext;
};

#endif // THINGSSDK_H
