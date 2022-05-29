#include "event.h"
#include "log.h"
#include "net.h"
#include "util.h"

void mg_call(struct mg_connection *c, int ev, void *ev_data) {
  // Run user-defined handler first, in order to give it an ability
  // to intercept processing (e.g. clean input buffer) before the
  // protocol handler kicks in
  if (c->fn != NULL) c->fn(c, ev, ev_data, c->fn_data);
  if (c->pfn != NULL) c->pfn(c, ev, ev_data, c->pfn_data);
}

void mg_error(struct mg_connection *c, const char *fmt, ...) {
  char mem[256], *buf = mem;
  va_list ap;
  va_start(ap, fmt);
  mg_vasprintf(&buf, sizeof(mem), fmt, ap);
  va_end(ap);
  MG_ERROR(("%lu %p %s", c->id, c->fd, buf));
  c->is_closing = 1;             // Set is_closing before sending MG_EV_CALL
  mg_call(c, MG_EV_ERROR, buf);  // Let user handler to override it
  if (buf != mem) free(buf);
}

const char * mg_ev_str[] = {
        "MG_EV_ERROR",
        "MG_EV_OPEN",
        "MG_EV_POLL",
        "MG_EV_RESOLVE",
        "MG_EV_CONNECT",
        "MG_EV_ACCEPT",
        "MG_EV_READ",
        "MG_EV_WRITE",
        "MG_EV_CLOSE",
        "MG_EV_HTTP_MSG",
        "MG_EV_HTTP_CHUNK",
        "MG_EV_WS_OPEN",
        "MG_EV_WS_MSG",
        "MG_EV_WS_CTL",
        "MG_EV_MQTT_CMD",
        "MG_EV_MQTT_MSG",
        "MG_EV_MQTT_OPEN",
        "MG_EV_SNTP_TIME",
        "MG_EV_USER",
};

