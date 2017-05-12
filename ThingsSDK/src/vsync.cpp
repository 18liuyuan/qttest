#ifdef AVR
#include <arduino.h>
#else
#include <stdio.h>
#endif

#include <stdlib.h>
#include <string.h>
#include "src/include/tutils.h"
#include "src/include/vsync.h"
#include "src/include/tobject.h"
#include "src/include/tparser.h"

VSync::VSync(Things *t) {
    _t       = t;
    root     = new TObject((char*)NULL, NULL);
};

VSync::~VSync() {
    delete root;
};

TObject *VSync::sync_with(const char *tid) {
    if (tid == NULL) { return NULL; };
    TObject *v = root->member(tid);
    if (v == NULL) {
        v = new TObject((char*)NULL, NULL);
        root->set(tid, v);
        char buf[20] = "tid:";
        strcpy(&buf[4], tid);
        _t->request(buf, "/v", NULL, NULL);
    }
    return v;
};

void VSync::sync() {
    int i;
    for (i = 0; i < root->member_cnt(); i++) {
        char buf[20] = "tid:";
        strcpy(&buf[4], root->member_name(i));
        _t->request(buf, "/v", NULL, NULL);
    };
};
void VSync::unsync(const char *tid) {
    if (tid == NULL) { return; };
    root->set(tid, NULL);
};

TObject *VSync::get(const char *tid) {
    return root->member(tid);
};

TObject *VSync::update_with(const char *tid, char *path, char *json) {
    if (tid == NULL) { return NULL; };
    TObject *to = root->member(tid);
    if (to == NULL) { return NULL; };
    TObject *v = to->member(path, BY_PATH);
    if (v == NULL) { return NULL; };

    TObject *delta = parse_json(&json);
    if (delta == NULL) { return v; };
    v->update_with(delta, true, BY_PATH);
    delete delta;
    return to;
};

TObject *VSync::set_with(char *tid, char *json) {
    if (tid == NULL) { return NULL; };
    TObject *v = parse_json(&json);
    if (v == NULL) { return NULL; };
    VAR_EVENT e;
    e.tid  = tid;
    e.path = NULL;
    TObject *old = root->member(tid);
    v = root->set(tid, v);
    if (old != NULL) {
        _t->emit_event(E_FOLLOWED_RESET, &e);
    } else {
        _t->emit_event(E_FOLLOWED_NEW, &e);
    }
    return v;
};
void VSync::pack() {
    root->pack();
};

void VSync::process_var_message(T_POST *post) {
    char *p, *cmd, *tid, *seqnum, *path, *body;
    VAR_EVENT e;
    p = get_token_first(post->message, ',');
    cmd     = p; p = get_token_next(p, ',');
    if (strcmp("d", cmd) == 0) {
        seqnum  = p; p = get_token_next(p, ',');
        tid     = p;
        TObject *v = root->member(tid);
        int num1;
        if (v != NULL) {
            v = v->member("__seqnum");
            if (v != NULL) {
                num1 = v->to_int();
                num1 = (num1 + 1) % 100;
            } else {
                num1 = -1;
            } 
        } else {
            num1 = -1;
        }
        int num2 = strtol(seqnum, NULL, 10);
        if (num1 != num2) {  // lost sync
            e.tid  = tid;
            e.path = NULL;
            _t->emit_event(E_FOLLOWED_LOST_SYNC, &e);
            char url[30];
            sprintf(url, "/v?tid=%s", tid);
            _t->request(".", url, NULL, NULL);
            return;
        }
        v->set(num2);
        p = get_token_next(p, ',');
        path = p; p = get_token_rest(p);
        body = p;
    } else if (strcmp("f", cmd) == 0) {
        seqnum  = p; p = get_token_next(p, ',');
        tid     = p;
        p = get_token_rest(p);
        body = p;
        path = NULL;
    } else if (strcmp("r", cmd) == 0) {
        tid = p;
        e.tid  = tid;
        e.path = NULL;
        _t->emit_event(E_FOLLOWED_BEFORE_REMOVE, &e);
        root->del(tid);    
        _t->emit_event(E_FOLLOWED_AFTER_REMOVE, &e);
    } else if (strcmp("s", cmd) == 0) {
        int num = strtol(p, NULL, 10);
        _t->emit_event(E_FOLLOWED_BEFORE_START, &num);
        int i;
        int member_cnt = root->member_cnt();
        for (i = member_cnt - 1; i >= 0 ; i--) {
            root->del(i);
        };
        _t->emit_event(E_FOLLOWED_START, &num);
    } else if (strcmp("e", cmd) == 0) {
        _t->emit_event(E_FOLLOWED_END, NULL);
    }
    if (strcmp(cmd, "f") == 0) {
        set_with(tid, body);
        int num = strtol(seqnum, NULL, 10);
        TObject *o = root->member(tid);
        if (o != NULL) {
            TObject *v = o->member("__seqnum");
            if (v != NULL) {
                v->set(num);
            } else {
                o->new_member("__seqnum")->set(num);
            }
        }
    } else if (strcmp(cmd, "d") == 0) {
        e.tid  = tid;
        e.path = path;
        _t->emit_event(E_FOLLOWED_BEFORE_UPDATE, &e);
        update_with(tid, path, body);
        _t->emit_event(E_FOLLOWED_AFTER_UPDATE, &e);
    }
};
