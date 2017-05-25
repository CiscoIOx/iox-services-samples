#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "mlib_service.h"
#include "mlib_log.h"
#include "mlib_timer.h"

#define TEMPERATURE_RANDOM_TOPIC "system.temperature.random.event"
#define SERVICE_NAME "urn:cisco:system:service:random-temperature"

typedef struct temperature_service_ {
    mlib_service    *s;
    long            interval;
    pthread_mutex_t mutex;
    mlib_timer      *timer;
} temperature_service;

temperature_service *s = NULL;

static temperature_service* temperature_service_init(void)
{
    temperature_service *s = NULL;

    s = (temperature_service *)MLIB.malloc(sizeof(temperature_service));
    if (!s) {
        mlib_log_err("%s: mlib_malloc failed", __FUNCTION__);
        return s;
    }
    memset(s, 0, sizeof(temperature_service));
 
    s->s = NULL;
    s->interval = 1000;
    pthread_mutex_init(&s->mutex, NULL);
    s->timer = NULL;
    return (s);
}

static void publish_temperature (void *arg)
{
    temperature_service *s = (temperature_service *)arg;
    
    if (!s) {
        mlib_log_err("%s: s is NULL", __FUNCTION__);
        return;
    }

    pthread_mutex_lock(&s->mutex);
    int t = 0;
    t = rand();
    t = t % 100;
    json_t *val = json_integer(t);
    PubSub.publish_v1(s->s, TEMPERATURE_RANDOM_TOPIC, val);
    json_decref(val);
    pthread_mutex_unlock(&s->mutex);
}

void
set_average_temperature_publish_time(void *link, json_t *rid, json_t *params)
{
    if (!link || !rid ) {
        mlib_log_err("NULL input arguments");
        json_t *err = json_string("NULL input arguments");
        RPC.response(link, rid, err);
        json_decref(err);
        return;
    }

    int t = 0;
    json_t *value = json_object_get(params, "value");
    if (value) {
        json_t *time = json_object_get(value, "temperature-publish-time");
        if (time) {
            t = (uint32_t)json_integer_value(time);
        }
    }
    mlib_log_info("Resetting temperature publish time to %d seconds", t);
    pthread_mutex_lock(&s->mutex);
    mlib_timer_start(s->timer, t * 1000);
    pthread_mutex_unlock(&s->mutex);

    json_t *obj = json_object();
    RPC.response(link, rid, obj);
    json_decref(obj);
}

mlib_error_t
add_rpc_methods()
{
    mlib_error_t rc = MLIB_SUCCESS;

    rc = RPC.operation_add(s->s, "setPublishTime",
                                    "Set Random Temperature Publish Time", "map",
                                    set_average_temperature_publish_time, NULL, 0);
    return rc;
}

static void initialized_cb (mlib_service *service) {}
static void connected_cb (mlib_service *service) {}
static void disconnected_cb (mlib_service *service) {}
static void ready_cb(mlib_service *service) {}

int
main(int argc, char **argv)
{
    if (MLIB.initialize(argc, argv) != MLIB_SUCCESS) {
        printf("mlib_initialize failed.\n");
        return 1;
    }

    s = temperature_service_init();
    if (!s) {
        mlib_log_err("%s: temperature_service_init failed", __FUNCTION__);
        return 1;
    }

    /* Initialize publisher service */
    s->s = MLIB.service_initialize(SERVICE_NAME,
                                   initialized_cb, connected_cb,
                                   disconnected_cb, ready_cb);
    if (!s->s) {
        mlib_log_err("mlib_service_init failed for random temperature service.");
    }
    MLIB.service_wait_for_init(s->s);

    if (add_rpc_methods() != MLIB_SUCCESS) {
        mlib_log_err("%s: add_rpc_methods failed", __FUNCTION__);
        return 1;
    }

    /* Create a timer to periodically publish random temperature */
    s->timer = mlib_timer_create(publish_temperature, (void *) s);
    if (!s->timer) {
        mlib_log_err("%s: mlib_timer_create failed", __FUNCTION__);
        return 1;
    }
    mlib_timer_start(s->timer, s->interval);

    MLIB.wait_for_services_finish();
    return 0;
}

