#ifndef AVR
#include <stdio.h>
#endif
#include <string.h>
#include <stdlib.h>
#include "src/include/common.h"
#include "src/include/cwlib.h"
#include "src/include/things.h"
#include "src/include/tparser.h"
#include "src/include/base64.h"
#include "src/include/connection.h"
#include "src/include/vsync.h"

#ifdef AVR
//#include <arduino.h>
#endif
/*  input buffer structure

    o------------o----o------o-------------o
    |            |    |      |             |
    |            |    |      |             +----o +in_buf_len
    |            |    |      |
    |            |    |      +------------------o +in_buf_end
    |            |    |
    |            |    +-------------------------o -in_buf_last_STX
    |            |
    |            +------------------------------o +in_package_len
    |
    +-------------------------------------------o 0

    * +  in_buf[x] doesn't  contain data
    * -  in_buf[x] contain data
*/

#ifdef T_DEBUG
void print_ch(uint8_t ch) {
    switch (ch) {
    case STX:
#ifdef AVR
        Serial.print(F("<STX>"));
#else
        printf("<STX>");
#endif
        break;
    case ETX:
#ifdef AVR
        Serial.print(F("<ETX>"));
#else
        printf("<ETX>");
#endif
        break;
    case US:
#ifdef AVR
        Serial.print(F("<US>"));
#else
        printf("<US>");
#endif
        break;
    case GS:
#ifdef AVR
        Serial.print(F("<GS>"));
#else
        printf("<GS>");
#endif
        break;
    default:
#ifdef AVR
        if ((byte)ch < ' ') {
            Serial.print(F("<"));
            Serial.print(ch, HEX);
            Serial.print(F(">"));
        } else {
            Serial.write(ch);
        }
#else
        if (ch < ' ') {
            printf("<%x>", ch);
        } else {
            printf("%c", ch);
        }
#endif
    }
};
#endif

Things::Things(const char* TID, const char *PASSKEY, Connection *conn,
               const size_t len_in, const size_t MTU) {
    init(TID, PASSKEY, conn, len_in, MTU);
};

Things::Things(Connection *conn,
               const size_t len_in, const size_t len_out,
               const size_t MTU) {
    init(NULL, NULL, conn, len_in,  MTU);
};

Things::~Things() {
    if (parent != NULL) { 
        parent->do_disconnect();
    };
    free(in_buf);
    if (tid != NULL) { free(tid); };
    if (sid != NULL) { free(sid); };
    if (addr != NULL) { free(addr); };
    if (_username != NULL) { free(_username); };
    if (_password != NULL) { free(_password); };
    if (_vars != NULL) { delete _vars; };
    int i;
    for (i = 0; i < request_handle_cnt; i++) {
        free(request_handles[i].path);
    };
    if (request_handles != NULL) {
        free(request_handles);
    };
};

void Things::init(const char* TID, const char* PASSKEY, Connection *conn,
               const size_t len_in, const size_t MTU) {
    idle  = true;

    counter_tx_byte    = 0;
    counter_tx_package = 0;
    counter_tx_error   = 0;
    counter_rx_byte    = 0;
    counter_rx_package = 0;
    counter_rx_error   = 0;

    _seqnum            = 0;
    passkey            = NULL;
    

    if (TID == NULL) {
        _login_with_tid = false;
        _auto_login     = false;
        tid             = NULL;
    } else {
        _login_with_tid = true;
        _auto_login     = true;
        tid             = (char*)malloc(strlen(TID) + 1);
        strcpy(tid, TID);
        if (passkey != NULL) {
            passkey         = (char*)malloc(strlen(PASSKEY) + 1);
            strcpy(passkey, PASSKEY);
        }
    }
    in_buf      = (char*)malloc(len_in);
    in_buf_len  = len_in;
    mtu         = MTU;
    _username   = NULL;
    _password   = NULL;
    addr        = NULL;
    sid         = NULL;

    user_on_request         = NULL;
    user_on_post            = NULL;

    context_user_on_request = NULL;
    context_user_on_post    = NULL;

    request_handles    = NULL;
    request_handle_cnt = 0;

    parent      = conn;
    if (parent) {
        parent->owner = this;
    };

    _vars = NULL;
    set_vars(new TObject((char*)NULL, NULL));
    on_request_url("/v", Things::on_request_var,  this);
    _sync_var = new VSync(this);
    reset();

#ifdef AVR
    Serial.print(F("size of Things = ")); Serial.println(sizeof(Things));
    Serial.print(F("        tid = "));    Serial.println(tid);
    Serial.print(F(" in_buf_len = "));    Serial.println(in_buf_len);
    Serial.print(F("        mtu = "));    Serial.println(mtu);
#endif
};

REQUEST* Things::req_get_by_id(uint16_t id) {
    uint16_t i;
    for (i = 0; i < REQUEST_QUEUE_MAX_LEN; i++) {
        if (req_queue[i].req_id == id) {
            return &req_queue[i];
        };
    };
    return NULL;
};
void Things::req_drop(REQUEST* req) {
    req->req_id   = 0;
    req->callback = NULL;
    req->timeout  = 0;
};
int Things::req_get_id() {
    int i;
    for (i = 0; i < REQUEST_QUEUE_MAX_LEN; i++) {
        if (req_queue[i].req_id == 0) {
            req_queue[i].req_id  = req_cnt + 1;
            req_queue[i].timeout = REQUEST_TIMEOUT;
            req_cnt++;
            if (req_cnt >= REQUEST_QUEUE_MAX_LEN) {
                req_cnt = 0;
            };
            return req_queue[i].req_id;
        };
    };
    return -1;
};

void Things::reset() {
    uint16_t i;
    authed = false;
    for (i = 0; i < REQUEST_QUEUE_MAX_LEN; i++) {
        req_queue[i].req_id    = 0;
        req_queue[i].timeout   = 0;
        req_queue[i].callback  = NULL;
    
    };
    if (addr != NULL) { free(addr); };
    addr = NULL;
    if (sid != NULL) { free(sid); };
    sid = NULL;

    timer_connect            = 0;
    timer_do_keep_alive      = 0;
    timer_keep_alive_timeout = 0;
    timer_login              = 0;
    timer_reconnect_interval = 0;
    timer_wait_aloha         = 0;
    timer_vars_push          = 0;

    cnt_keep_alive_retry     = 0;
    event_q_head             = 0;
    event_q_tail             = 0;

    state_stream         = STREAM_STATE_WAIT_STX;
    state_socket         = SOCKET_IDLE;

    in_buf[0]            = 0;
    in_buf_end           = 0;
    in_buf_last_STX      = 0;
    in_package_len       = 0;
};

void Things::check_timer(uint8_t* timer, SOCKET_EVENT e) {
    if (*timer > 0) {
        *timer = *timer - 1;
        if (*timer == 0) {
            post_event(e, NULL, 0);
        };
    }
};
Event* Things::get_next_event() {
    Event* e;
    if (event_q_tail == event_q_head) {
        return NULL;
    };
    e  = &event_queue[event_q_tail];
    event_q_tail = (event_q_tail + 1) % EVENT_QUEUE_MAX_LEN;
    return e;
};
void Things::in_buf_remove_package_and_check_for_next_package() {
    if (in_package_len == 0) {
        return;
    };
    memmove(&in_buf[0],
            &in_buf[in_package_len],
            in_buf_len - in_package_len);

    in_buf_end = in_buf_end - in_package_len;
    if (in_buf_last_STX >= in_package_len) {
        in_buf_last_STX = in_buf_last_STX - in_package_len;
    } else {
        in_buf_last_STX = in_buf_end;
    }

    size_t pos = 0; int found = 0;
    while (pos < in_buf_end) {
        if (in_buf[pos] == ETX) {found = 1; break; };
        pos++;
    };
    if (found) {
        in_package_len  = pos + 1;
        in_buf_last_STX = pos + 1;
        idle = false;
    } else {
        in_package_len = 0;
    };
};
void Things::process_input_buf(){
    get_data_from_connection();
    if (event_q_tail != event_q_head) return;
    if (in_package_len == 0) return;
    if (in_buf[0] == 0) return;
    in_buf[in_package_len - 1] = 0;
    in_buf[0] = 0;
    char* cmd = get_token_first(&in_buf[1], GS);
    cmd = get_token_next(cmd, US);
    char* data = get_token_rest(cmd);
    size_t len = strlen(data);
    if (cmd != NULL) {
        if (strcmp(cmd, CMD_ALOHA) == 0) {
            post_event(E_cmd_aloha, data, len);
        } else if (strcmp(cmd, CMD_LOGIN_REPLY) == 0) {
            post_event(E_cmd_login_reply, data, len);
        } else if (strcmp(cmd, CMD_KEEP_ALIVE_REPLY) == 0) {
            post_event(E_cmd_keep_alive_reply, data, len);
        } else if (strcmp(cmd, CMD_DATA) == 0) {
            post_event(E_cmd_data, data, len);
        };
    };
};


void Things::on_response(char *from, char *to, char *data) {
    T_RESPONSE res;
    char* p;
    p = get_token_first(data, ','); res.req_id      = atoi(p);
    p = get_token_next(p, ',');     res.status_code = atoi(p);
    p = get_token_next(p, ',');     res.header      = p;
    p = get_token_rest(p);
    res.body = p;
    REQUEST* req = req_get_by_id(res.req_id);
    if (req != NULL) {  // maybe cleared by timeout
        if (req->callback != NULL) {
            req->callback(this, &res, req->context);
        };
        req_drop(req);
    }
};
void Things::on_post(char *from, char *to, char *data) {
    // printf("\npost from:[%s] to: [%s] with data[%s]\n", from, to, data);
    T_POST post;
    char* p;
    post.from = from;
    post.to   = to;
    p = get_token_first(data,   ',');  post.readers = p;
    p = get_token_next(p, ',');  post.src     = p;
    p = get_token_next(p, ',');  post.type    = p;
    p = get_token_rest(p);       post.message = p;
    if ((strcmp(to, addr) == 0) && (strcmp(post.type, "v") == 0)) {
        _sync_var->process_var_message(&post);
    } else {
        if (user_on_post != NULL) {
            user_on_post(this, &post, context_user_on_post);
        };
    }
};
void Things::on_request_url(const char *path, 
                            Things_callback handle, void *context) {
    if (handle == NULL) { // delete handle
        return;
    }
    int i;
    for (i = 0; i < request_handle_cnt; i++) {
        if (strcmp(request_handles[i].path, path) == 0) {
            request_handles[i].handle  = handle;
            request_handles[i].context = context;
            return;
        }
    };
    // not exists
    request_handle_cnt++;
    request_handles = (REQUEST_HANDLE*)realloc(request_handles,
                              request_handle_cnt * sizeof(REQUEST_HANDLE));
    i = request_handle_cnt - 1;
    request_handles[i].path = (char*)malloc(strlen(path) + 1);
    strcpy(request_handles[i].path, path);
    request_handles[i].handle  = handle;
    request_handles[i].context = context;
};

void Things::on_request(char *from, char *to, char *data){
    char* p;
    T_REQUEST req;
    req.from   = from;
    req.to     = to;
    p          = get_token_first(data, ',');
    req.req_id = atoi(p);
    p          = get_token_rest(p);
    req.url    = p;
    int path_len = 0;
    char ch = req.url[0];
    while (ch != 0) {
        if (ch == '?') {
            break;
        }
        path_len++;
        ch = req.url[path_len];
    };
    int i;
    int found = 0;
    if (strcmp(req.to, addr) == 0) {
        for (i = 0; i < request_handle_cnt; i++) {
            if (strlen(request_handles[i].path) != path_len) {
                continue;
            }
            if (strncmp(request_handles[i].path, req.url, path_len) == 0) {
                if (ch != 0) {
                    req.url = req.url + path_len + 1;
                } else {
                    req.url = NULL;
                }
                found = 1;
                break;
            }
        };
    }
    if (found) {
        request_handles[i].handle(this, &req, request_handles[i].context);
    } else if (user_on_request != NULL) {
        user_on_request(this, &req, context_user_on_request);
    };
};

void Things::do_reconnect() {
    if (parent == NULL) { return; };
    reset();
    if (parent->connected()) {
        parent->do_disconnect();
    };
    if (parent->do_connect_to() < 0) {
        post_event(E_socket_error, NULL, 0);
    } else {
        timer_connect = TIME_CONNECT_TIMEOUT;
    }
    emit_event(E_CONNECTING, NULL);
};
void Things::connect_to(const char *conn_str ) {
    if (!parent) { return; };
    if (sid) {
        free(sid);
        sid = NULL;
    };
    parent->set_connect_info(conn_str);
    post_event(E_user_connect_to, NULL, 0);
}

void Things::on_cmd_aloha(Event* e){
    int result = 0;
    char* p = get_token_first(e->data, US);
    while (1) {
        if (p == NULL) {
            emit_event(E_SERVER_RESPONSE_ERROR, NULL);
            result = -1;
            break;
        };
        if (strcmp(p, "tc") != 0) { //server_sign
            result = -2;
            emit_event(E_SERVER_RESPONSE_ERROR, NULL);
            break;
        }; 
        p = get_token_next(p, US);
        if (p == NULL) {
            result = -3;
            emit_event(E_SERVER_RESPONSE_ERROR, NULL);
            break;
        };
        if (strcmp(p, "1") != 0)  { //service_type
            result = -3;
            emit_event(E_SERVER_SERVICE_NOT_AVAILABLE, NULL);
            break;
        }; 
        p = get_token_next(p, US);
        if (p == NULL) {
            emit_event(E_SERVER_RESPONSE_ERROR, NULL);
            result = -4;
            break;
        };
        if (strcmp(p, "1") != 0)  { //version
            result = -4;
            emit_event(E_SERVER_VERSION_NOT_SUPPORT, NULL);
            break;
        };
        p = get_token_next(p, US);
        if (p == NULL) {
            emit_event(E_SERVER_RESPONSE_ERROR, NULL);
            result = -5;
            break;
        };
        if (strcmp(p, "1") != 0) { //server_state
            emit_event(E_SERVER_NOT_READY, NULL);
            result = -5;
            break;
        };
        break;
    };
    timer_wait_aloha = 0;
    if (result == 0) {
        post_event(E_server_available, NULL, 0);
    } else {
        post_event(E_server_not_available, NULL, 0);
    };
};
void Things::on_server_available(Event* e){
    if (_auto_login) {
        send_cmd_login_to_parent();
    };
    emit_event(E_CONNECTED, NULL);
}

void Things::reset_keep_alive_timer() {
    if (timer_keep_alive_timeout > 0) {
        timer_do_keep_alive = TIME_KEEP_LIVE_INTERVAL;
        timer_keep_alive_timeout = 0;
        cnt_keep_alive_retry     = 0;
    }
};
void Things::on_cmd_keep_alive_reply(Event* e){
    char* p_time = get_token_first(e->data, US);
    if ((p_time != NULL) && (p_time[0] != 0))  {
        emit_event(E_SERVER_TIME, p_time);
    }
    reset_keep_alive_timer();
};
void Things::on_cmd_login_reply(Event* e){
    int result = 0;
    int i;
    char* p_code = get_token_first(e->data, US);
    if (p_code != NULL) {
        i = strcmp(p_code, "200");
        if (i != 0) { result = -1; };
    } else {
        result = -5;
    };
    timer_login = 0;
    if (result == 0) {
        char* p_addr = get_token_next(p_code, US);  // address
        p_addr = get_token_first(p_addr, ',');
        addr = (char*)malloc(strlen(p_addr) + 1);
        strcpy(addr, p_addr);
        char* p_sid  = get_token_next(p_addr, ',');
        if (sid != NULL) { free(sid); };
        sid = (char*)malloc(strlen(p_sid) + 1);
        strcpy(sid, p_sid);
        char* p_tid  = get_token_next(p_sid, ',');
        if (tid != NULL) { free(tid); };
        tid = (char*)malloc(strlen(p_tid) + 1);
        strcpy(tid, p_tid);
        post_event(E_authed, NULL, 0);
        char* p_time = get_token_next(p_tid, ',');
        emit_event(E_SERVER_TIME, p_time);
    } else {
        post_event(E_auth_fail, NULL, 0);
    };
};
void Things::on_cmd_data(Event* e){
    char* to;
    char* from;
    char* cmd;
    char* data;
    char* p;
  
    p = get_token_first(e->data, US); to   = p;
    p = get_token_next(p, US);  from = p;
    p = get_token_next(p, ','); cmd  = p;
    data = get_token_rest(p);
    if (strcmp(cmd, CMD_POST)          == 0) {on_post(from, to, data); }
    else if (strcmp(cmd, CMD_REQUEST)  == 0) {on_request(from, to, data); }
    else if (strcmp(cmd, CMD_RESPONSE) == 0) {on_response(from, to, data);}
};
void Things::on_authed(Event* e){
    authed = true;
    push_vars(0);
    emit_event(E_AUTHED, NULL);
    timer_do_keep_alive = TIME_KEEP_LIVE_INTERVAL;
};
void Things::on_auth_fail(Event* e){
    emit_event(E_AUTH_FAIL, NULL);
    timer_reconnect_interval = TIME_RECONNECT_INTERVAL;
};
void Things::on_keep_alive_timeout(Event* e){
    cnt_keep_alive_retry++;
    if (cnt_keep_alive_retry > CNT_KEEP_LIVE_MAX_RETRY) {
        post_event(E_keep_alive_max_retry, NULL, 0);
    } else {
        send_cmd_keep_alive_to_parent();
    };
};

int  Things::need_release_buffer(const SOCKET_EVENT code) {
    switch (code) {
    case E_cmd_aloha:
    case E_cmd_data:
    case E_cmd_login_reply:
    case E_cmd_keep_alive_reply:
        return 1;
        break;
    default:
        return 0;
    }
};

void Things::post_event(const SOCKET_EVENT code,
                        char* data, const size_t len){
    if ((event_q_head + 1) % EVENT_QUEUE_MAX_LEN == event_q_tail) {
        // queue overflow
        return;
    };
    event_queue[event_q_head].code = code;
    event_queue[event_q_head].data = data;
    event_queue[event_q_head].len  = len;
    event_q_head = (event_q_head + 1) % EVENT_QUEUE_MAX_LEN;
};
void Things::get_data_from_connection() {
    if (parent == NULL) { return; };
    if (parent->connected() <= 0) { return; };
    if (in_buf_end >= in_buf_len) {
        in_buf_end = in_buf_last_STX;
        in_buf_end++;
        state_stream = STREAM_STATE_WAIT_STX;
        return;
    };
    
    int len;
    len = parent->do_recv_data(&in_buf[in_buf_end],
                               in_buf_len - in_buf_end);

    if (len == 0) { 
        return;
    } else if (len < 0) {
        post_event(E_socket_error, NULL, 0);
        return;
    };
    counter_rx_byte = counter_rx_byte + len;
    size_t new_data_end = in_buf_end + len;
    if (new_data_end > in_buf_len) {
        // recv return data more than given length
        return;
    };
    // get data in in_buf length = len
    size_t i;
    for (i = in_buf_end; i < new_data_end; i++) {
        char ch = in_buf[i];
        switch (state_stream) {
        case STREAM_STATE_WAIT_STX:
            if (ch == STX) {
#ifdef T_DEBUG
#ifdef AVR
                Serial.print(F("\n\r[recv]"));
#else
               // printf("\n[recv]");
#endif
                print_ch(ch);
#endif
                in_buf[in_buf_end] = ch;
                in_buf_last_STX    = in_buf_end;
                in_buf_end++;
                state_stream = STREAM_STATE_WAIT_ETX;
            } else {
                emit_event(E_FRAME_ERROR_UNEXCEPT_DATA, &ch);
            }
            break;
        case STREAM_STATE_WAIT_ETX:
            if (ch == ETX) {
#ifdef T_DEBUG
                print_ch(ch);
#ifdef AVR
                Serial.println();
#else
                printf("\n");
#endif
#endif
                counter_rx_package++;
                in_buf[in_buf_end] = ch;
                in_buf_end++;
                if (in_package_len == 0) {
                    in_package_len = in_buf_end;
                };
                in_buf_last_STX    = in_buf_end + 1;
                state_stream = STREAM_STATE_WAIT_STX;
            } else if (ch == STX) {
                emit_event(E_FRAME_ERROR_UNEXCEPT_STX, NULL);

#ifdef T_DEBUG
#ifdef AVR
                Serial.print(F("\n[Err recv]"));
#else
                printf("\n[Err recv]");
#endif
                print_ch(ch);
#endif
                counter_rx_error++;
                in_buf_end = in_buf_last_STX;
                in_buf_end++;
            } else {
#ifdef T_DEBUG
                print_ch(ch);
#endif
                in_buf[in_buf_end] = ch;
                in_buf_end++;
            };
        };
    }
};
void Things::stop() {
    switch (state_socket) {
    case SOCKET_IDLE: return;
    case SOCKET_CONNECTING:
    case SOCKET_SERVER_NA:
    case SOCKET_WAIT_RECONNECT:
    case SOCKET_WAIT_ALOHA:
    case SOCKET_CONNECTED:
    case SOCKET_AUTHED:
    case SOCKET_AUTHED_RESPAWN:
        if (parent != NULL) { 
            parent->do_disconnect();
        };
        state_socket = SOCKET_IDLE;
    }
};
void Things::loop() {
    if (parent == NULL) { return; };

    if ((state_socket == SOCKET_AUTHED)
        || (state_socket == SOCKET_AUTHED_RESPAWN)) {

        if (_vars->changed()) {
            push_vars(1);
        };
    };
    if (state_socket == SOCKET_CONNECTING) {
        if (parent == NULL) { return;};
        int connected = parent->connected();
        if (connected == 1) {
            post_event(E_socket_connected, NULL, 0);
        } else if (connected == -1) {
            post_event(E_socket_error, NULL, 0);
        };
    } else {
        process_input_buf();
    };
    Event* e = get_next_event();
    if (e == NULL) {
        idle = true;
        in_buf_remove_package_and_check_for_next_package();
        return;
    };
    idle = false;
    switch (state_socket) {
    case SOCKET_IDLE:
        if (e->code == E_user_connect_to) {
            do_reconnect();
            state_socket = SOCKET_CONNECTING;
        };
        break;
    case SOCKET_CONNECTING:
        if ((e->code ==E_socket_end)
                   || (e->code == E_socket_close)
                   || (e->code == E_connect_timeout)
                   || (e->code == E_socket_error)) {
            emit_event(E_CONNECT_FAIL, NULL);
            timer_reconnect_interval = TIME_RECONNECT_INTERVAL;
            state_socket = SOCKET_WAIT_RECONNECT;
        } else if (e->code == E_socket_connected) {
            timer_connect    = 0;
            timer_wait_aloha = 5;
            state_socket = SOCKET_WAIT_ALOHA;
        };
        break;
    case SOCKET_SERVER_NA:
        break;
    case SOCKET_WAIT_RECONNECT:
        if (e->code == E_reconnect_interval) {
            do_reconnect();
            state_socket = SOCKET_CONNECTING;
        };
        break;
    case SOCKET_WAIT_ALOHA:
        if ((e->code ==E_socket_end)
                   || (e->code == E_socket_close)
                   || (e->code == E_connect_timeout)
                   || (e->code == E_aloha_timeout)
                   || (e->code == E_socket_error)) {
            do_reconnect();
            state_socket = SOCKET_CONNECTING;
        } else if (e->code == E_cmd_aloha) {
            on_cmd_aloha(e);
        } else if (e->code == E_server_available) {
            on_server_available(e);
            state_socket = SOCKET_CONNECTED;
        } else if (e->code == E_server_not_available) {
            timer_reconnect_interval = TIME_RECONNECT_INTERVAL;
            state_socket = SOCKET_WAIT_RECONNECT;
        };
        break;
    case SOCKET_CONNECTED:
        if ((e->code ==E_socket_end)
                   || (e->code == E_socket_close)
                   || (e->code == E_connect_timeout)
                   || (e->code == E_socket_error)) {
            do_reconnect();
            state_socket = SOCKET_CONNECTING;
        } else if (e->code == E_cmd_login_reply) {
            on_cmd_login_reply(e);
        } else if (e->code == E_login_reply_timeout) {
            send_cmd_login_to_parent();
            _login_retry_cnt++;
            if (_login_retry_cnt > 5) {
                do_reconnect();
                state_socket = SOCKET_CONNECTING;
            }
        } else if (e->code == E_authed) {
            state_socket = SOCKET_AUTHED;
            on_authed(e);
            state_socket = SOCKET_AUTHED;
        } else if (e->code == E_auth_fail) {
            on_auth_fail(e);
            if (_auto_login) {
                state_socket = SOCKET_WAIT_RECONNECT;
            };
        };
        break;
    case SOCKET_AUTHED:
        if ((e->code ==E_socket_end)
                   || (e->code == E_socket_close)
                   || (e->code == E_socket_error)) {
            emit_event(E_DISCONNECTED, NULL);
            do_reconnect();
            state_socket = SOCKET_CONNECTING;
        } else if (e->code == E_cmd_keep_alive_reply) {
            on_cmd_keep_alive_reply(e);
        } else if (e->code == E_keep_alive_timeout) {
            on_keep_alive_timeout(e);
            emit_event(E_RESPAWN, NULL);
            state_socket = SOCKET_AUTHED_RESPAWN;
        } else if (e->code == E_do_keep_alive) {
            send_cmd_keep_alive_to_parent();
        } else if (e->code == E_cmd_data) {
            on_cmd_data(e);
            reset_keep_alive_timer();
        } else if (e->code == E_vars_push) {
            push_vars(1);
        };
        break;
    case SOCKET_AUTHED_RESPAWN:
        if ((e->code ==E_socket_end)
                   || (e->code == E_socket_close)
                   || (e->code == E_socket_error)) {
            emit_event(E_DISCONNECTED, NULL);
            do_reconnect();
            state_socket = SOCKET_CONNECTING;
        } else if (e->code == E_cmd_keep_alive_reply) {
            on_cmd_keep_alive_reply(e);
            emit_event(E_RESPAWNED, NULL);
            state_socket = SOCKET_AUTHED;
        } else if (e->code == E_keep_alive_timeout) {
            on_keep_alive_timeout(e);
        } else if (e->code == E_do_keep_alive) {
            send_cmd_keep_alive_to_parent();
        } else if (e->code == E_keep_alive_max_retry) {
            emit_event(E_DISCONNECTED, NULL);
            do_reconnect();
            state_socket = SOCKET_CONNECTING;
        } else if (e->code == E_cmd_data) {
            on_cmd_data(e);
            reset_keep_alive_timer();
            emit_event(E_RESPAWNED, NULL);
            state_socket = SOCKET_AUTHED;
        };
        break;
    default:
        state_socket = SOCKET_IDLE;
    };
};
void Things::time_tick() {
    check_timer(&timer_connect            , E_connect_timeout);
    check_timer(&timer_do_keep_alive      , E_do_keep_alive);
    check_timer(&timer_keep_alive_timeout , E_keep_alive_timeout);
    check_timer(&timer_login              , E_login_reply_timeout);
    check_timer(&timer_reconnect_interval , E_reconnect_interval);
    check_timer(&timer_wait_aloha         , E_aloha_timeout);
    check_timer(&timer_vars_push          , E_vars_push);
  
    int i;
    T_RESPONSE res;
    res.req_id = 0;
    res.status_code = 408;
    res.header = NULL;
    res.body   = NULL;
    for (i = 0; i < REQUEST_QUEUE_MAX_LEN; i++) {
        if (req_queue[i].timeout != 0) {
            req_queue[i].timeout--;
            if (req_queue[i].timeout == 0) {
                res.req_id = req_queue[i].req_id;
                if (req_queue[i].callback != NULL) {
                    req_queue[i].callback(this, &res,
                                          req_queue[i].context);
                };
                req_queue[i].req_id = 0;
            };
        };
    };
};

void Things::on(THINGS_EVENT event, Things_event_callback cb, void *context) {
    if (event == ON_EVENT) {
        user_on_event  = cb;
        context_user_on_event = context;
    };
};
void Things::on(THINGS_EVENT event, Things_callback cb, void *context) {
    switch(event) {
    case ON_REQUEST:
        user_on_request = cb;
        context_user_on_request = context;
        break;
    case ON_POST:
        user_on_post    = cb;
        context_user_on_post = context;
        break;
    default:
        break;
    };
};


int Things::data_send(const char *data) {
    if (data == NULL) { return 0; };
    if (parent == NULL) { return -1; };
    size_t len = strlen(data);
#ifdef T_DEBUG
    int i;
    for (i = 0; i < len; i++) {
        print_ch(data[i]);
    };
#endif
    counter_tx_byte = counter_tx_byte + len;
    size_t pos = 0;
    ssize_t sent = 0;
    while (pos < len) {
        sent = parent->do_send_data(&data[pos], len - pos);
        if (sent < 0) {
            return sent;
            break;
        }
        pos = pos + sent;
    };
    return len;
};
int Things::data_send(const char ch) {
    if (parent == NULL) { return -1; };
#ifdef T_DEBUG
    print_ch(ch);
#endif
    char buf[2];
    buf[0] = ch;
    buf[1] = 0;
    return parent->do_send_data((char*)&buf, 1);
};
int Things::send_cmd_begin() {
#ifdef T_DEBUG
#ifdef AVR
    Serial.print(F("\n\r[send]"));
#else
    printf("\n[send]");
#endif
#endif
    int ret;
    ret = data_send(STX);
    ret = data_send(GS);
    counter_tx_byte = counter_tx_byte + 2;
    return ret;
};
int Things::cmd_end() {
    int ret = data_send(ETX);
    if (ret < 0) {
        post_event(E_socket_error, NULL, 0);
    };
    counter_tx_byte++;
    counter_tx_package++;
#ifdef T_DEBUG
#ifdef AVR
    Serial.println();
#else
    printf("\n");
#endif
#endif
    idle = false;
    return ret;
};
void Things::send_cmd_login_to_parent() {
    send_cmd_begin();
    data_send(CMD_LOGIN);
    data_send(US);
    data_send(sid);
    data_send(US);
    if (_login_with_tid) {
        data_send('d');
        data_send(US);
        data_send(tid);
        if (this->passkey && (strlen(this->passkey) > 0)) {
            data_send('@');
            data_send(this->passkey);
        }
    } else if (_username != NULL) {
        data_send('p');
        data_send(US);
        data_send(_username);
        data_send(US);
        data_send(_password);
    };
    cmd_end();
    timer_login = TIME_LOGIN_TIMEOUT;
};
void Things::send_cmd_keep_alive_to_parent() {
    send_cmd_begin();
    data_send(CMD_KEEP_ALIVE);
    data_send(US);
    data_send('0');
    cmd_end();
    timer_do_keep_alive = 0;
    timer_keep_alive_timeout = TIME_KEEP_LIVE_TIMEOUT;
};

void Things::login(const char *username, const char *password) {
    if (strlen(username) > MAX_USERNAME_LEN) { return; };
    if (strlen(password) > MAX_PASSWORD_LEN) { return; };
    if (_username != NULL) { free(_username); };
    if (_password != NULL) { free(_password); };

    int len;
    _username = (char *)malloc(strlen(username) * 2 + 10);
    len = base64_encode(_username, username, (int)strlen(username));
    _password = (char *)malloc(strlen(password) * 2 + 10);
    len = base64_encode(_password, password, (int)strlen(password));
    _login_retry_cnt = 0;
    send_cmd_login_to_parent();
};

void Things::cmd_begin(const char *cmd, const char *dst) {
    cmd_beginEx(cmd, addr, dst);
};
void Things::cmd_beginEx(const char *cmd,
                         const char *from, const char *dst) {
    if (state_socket != SOCKET_AUTHED) return;
    send_cmd_begin();
    data_send(CMD_DATA);
    data_send(US);
    data_send(dst);
    data_send(US);
    data_send(from);
    data_send(US);
    data_send(cmd);
};

void Things::push(const char *dst, const char *type, const char *msg) {
    pushEx(addr, dst, type, msg);
};


void Things::pushEx(const char *from, const char *to,
                    const char *type, const char *msg) {
    if (state_socket != SOCKET_AUTHED) return;
    cmd_beginEx(CMD_PUSH, from, to);
    data_send(',');
    data_send(type);
    data_send(',');
    data_send(msg);
    cmd_end();
};

int  Things::request(const char* dst, const char* url,
                     Things_callback handle, void *context) {
    return requestEx(addr, dst, url, handle, context);
};
int  Things::requestEx(const char *from, const char *dst,
                       const char *url, Things_callback cb,
                       void *context) {
    if (state_socket != SOCKET_AUTHED) {return -1;};
    int id = req_get_id();
    if (id < 0) {
        return id;
    }
    char req_id_buf[5];
    sprintf(req_id_buf, "%d", id);
    REQUEST* req = req_get_by_id(id);
    req->callback = cb;
    req->context  = context;

    cmd_beginEx(CMD_REQUEST, from, dst);
    data_send(',');
    data_send((char*)&req_id_buf);
    data_send(',');
    data_send((char*)url);
    cmd_end();
    return id;
};

void Things::response(const char *to, uint8_t req_id,
                      int code, const char* header, const char* body) {
    responseEx(addr, to, req_id, code, header, body);
};

void Things::responseEx(const char *from, const char *to,
                        uint8_t req_id, int code,
                        const char* header,const char* body) {
    if (state_socket != SOCKET_AUTHED) return;
    char s_req_id[5];
    sprintf(s_req_id, "%d", req_id);
    char s_code[5];
    sprintf(s_code, "%d", code);

    cmd_beginEx(CMD_RESPONSE, from, to);
    data_send(',');
    data_send(s_req_id);
    data_send(',');
    data_send(s_code);
    data_send(',');
    data_send(header);
    data_send(',');
    data_send(body);
    cmd_end();
};


void Things::on_request_var_query(Things *this_t,
                                  void *data, void *context) {
    T_REQUEST *req = (T_REQUEST *)data;
#ifdef AVR
    Serial.print(F("request for /vq with param : "));
    Serial.println(req->url);
#else
    printf("request for /vq with param : %s\n", req->url);
#endif
    if (req->url == NULL) {
        this_t->response(req->from, req->req_id,
                         200, NULL, this_t->vars()->to_string());
        this_t->vars()->pack();
    } else {
        if (strncmp(req->url, "var=", 4) != 0) { return; };
        char *var_path = &req->url[4];
        TObject *v     = this_t->vars()->member(var_path);
        if (v == NULL) { return; };
        char *body = v->to_string();
        this_t->response(req->from, req->req_id, 200, NULL, body);
    };
};

void Things::on_request_var_set(Things *this_t, void *data, void *context) {
    T_REQUEST *req = (T_REQUEST *)data;
#ifdef AVR
    Serial.print(F("request for /vs with param : "));
    Serial.println(req->url);
#else
    printf("request for /vs with param : %s\n", req->url);
#endif
    if (strncmp(req->url, "var=", 4) != 0) { return; };
    Things *t      = (Things*)context;
    char *var_path = &req->url[4];
    TObject *v     = t->vars()->member(var_path);
    if (v == NULL) { return; };
    char *body = v->to_string();
    t->response(req->from, req->req_id, 200, NULL, body);
#ifdef AVR
    Serial.print(F("response with :"));
    Serial.println(body);
#else
    printf("response with : %s\n", body);
#endif
};

void Things::on_request_var_sync(Things *this_t, void *data, void *context){
    T_REQUEST *req = (T_REQUEST *)data;
    Things *t      = (Things*)context;
    t->response(req->from, req->req_id, 200, NULL, NULL);
    t->push_vars(0);
};

void Things::on_request_var(Things *this_t, void *data, void *context) {
    T_REQUEST *req = (T_REQUEST *)data;
    Things *t      = (Things*)context;
    char *url      = req->url;;
    if ((url == NULL) || (url[0] == 0)) { // trigger full update
#ifdef T_DEBUG        
#ifdef AVR
        Serial.println("var update \n");
#else
        printf("var update \n");
#endif
#endif
        t->response(req->from, req->req_id, 200, NULL, NULL);
        t->push_vars(0);
    } else if (strncmp(url, "c=s&v=", 6) == 0) {  // var set
#ifdef T_DEBUG        
#ifdef AVR
        Serial.println("var set \n");
#else
        printf("var set\n");
#endif
#endif
        url = url + 6;
        TObject *delta = parse_json(&url);
        if (delta != NULL) {
            TObject *v = t->vars();
            v->update_with(delta, true, BY_PATH); //  auto insert
            t->response(req->from, req->req_id, 200, NULL, NULL);
            t->push_vars(1);
        } else {
            t->response(req->from, req->req_id, 400, NULL, NULL);
        }
    } else if (strncmp(url, "c=q&v=", 6) == 0) {  // var query
#ifdef T_DEBUG        
#ifdef AVR
        Serial.println("var query \n");
#else
        printf("var query\n");
#endif
#endif
        url = url + 6;
        TObject *result = t->vars()->clone(url);
        char *body = result->to_string();
        t->response(req->from, req->req_id, 200, NULL, body);
        delete result;
    } else {
#ifdef T_DEBUG        
#ifdef AVR
        Serial.println("var url error \n");
#else
        printf("var url errro \n");
#endif
#endif
        t->response(req->from, req->req_id, 404, NULL, NULL);
    };
};



SOCKET_STATE Things::state() {
    return state_socket;
};


void Things::push_vars(int delta) {
    char *msg;
    char s_seqnum[4];
    if (delta) {
        if (_vars->changed()) {
            _seqnum = (_seqnum + 1) % 100;
            sprintf(s_seqnum, "%d", _seqnum);
            msg = _vars->to_string(T_FLAGS_CHANGED);
            cmd_begin(CMD_PUSH, ".");
            data_send(",v,d,");
            data_send(s_seqnum);
            data_send(",");
            data_send(msg);
            cmd_end();
            _vars->pack();
            _vars->commit();
        };
    } else {
        sprintf(s_seqnum, "%d", _seqnum);
        msg = _vars->to_string();
        cmd_begin(CMD_PUSH, ".");
        data_send(",v,f,");
        data_send(s_seqnum);
        data_send(",");
        data_send(msg);
        cmd_end();
        _vars->pack();
        _vars->commit();
    };
    timer_vars_push = PUSH_VAR_INTERVAL;
};

void Things::set_vars(TObject *v) {
    if (_vars != NULL) {
        delete _vars;
    };
    _vars = v;
};

TObject *Things::vars() {
    return _vars;
};

TObject *Things::vars(const char *path) {
    if (path != NULL) {
        return _vars->member(path);
    } else {
        return NULL;
    };
};


VSync *Things::sync_var() {
    return _sync_var;
};

TObject *Things::sync_var(const char *tid) {
    if (_sync_var == NULL) { return NULL; };
    return _sync_var->get(tid);
};

TObject *Things::sync_var(const char *tid, const char *path) {
    if (_sync_var == NULL) { return NULL; };
    TObject * v = _sync_var->get(tid);
    if (v == NULL) { return NULL; };
    return v->member(path);
};

void Things::emit_event(T_EVENT e, void *data) {
    if (user_on_event != NULL) {
        user_on_event(this, e, data,
                      context_user_on_event);
    }
};
