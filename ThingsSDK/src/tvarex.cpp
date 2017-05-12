#ifdef AVR
#include <arduino.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "src/include/tutils.h"
#include "src/include/tobject.h"
#include "src/include/tparser.h"
#include "src/include/tvarex.h"

VarEx::VarEx(Things *t) {
    _t       = t;
    _vars    = NULL;
    _var_cnt = 0;
};

VarEx::~VarEx() {
    int i;
    for (i = 0; i < _var_cnt; i++) {
        delete _vars[i].var;
        free(_vars[i].addr);
    }
    free(_vars);
};

void VarEx::push_vars(int delta) {
    char *msg;
    int i;
    for (i = 0; i < _var_cnt; i++) {
        TObject *v = _vars[i].var;
        if (delta) {
            if (v->changed()) {
                msg = v->to_string(T_FLAGS_CHANGED);
                _t->cmd_begin(CMD_PUSH, ".");
                _t->data_send(",v,d,");
                _t->data_send(msg);
                _t->cmd_end();
                v->pack();
                v->commit();
            };
        } else {
            msg = v->to_string();
            _t->cmd_begin(CMD_PUSH, ".");
            _t->data_send(",v,f,");
            _t->data_send(msg);
            _t->cmd_end();
            v->pack();
            v->commit();
        };
    };
};
