#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "mlib_service.h"
#include "mlib_log.h"
#include "mlib_timer.h"

#define TEMPERATURE_RANDOM_TOPIC "system.temperature.random.event"
#define TEMPERATURE_AVERAGE_TOPIC "system.temperature.average.event"
#define SERVICE_NAME "urn:cisco:system:service:average-temperature"
#define TMP_GT_50 "AvgTemperatureGT50"
#define TMP_LT_50 "AvgTemperatureLT50"
#define TMP_COND "TempConditions"

/* Average Temperature */
int average = 0;

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
    s->interval = 5000;
    pthread_mutex_init(&s->mutex, NULL);
    s->timer = NULL;
    return (s);
}

static void 
publish_temperature (void *arg)
{
    temperature_service *s = (temperature_service *)arg;
    
    if (!s) {
        mlib_log_err("%s: s is NULL", __FUNCTION__);
        return;
    }

    pthread_mutex_lock(&s->mutex);
    json_t *avg = json_object();
    json_object_set_new_nocheck(avg, "average-temperature", json_integer(average));
    PubSub.publish_v1(s->s, TEMPERATURE_AVERAGE_TOPIC, avg);
    json_decref(avg);
    pthread_mutex_unlock(&s->mutex);
}

static void 
subscribe_callback(mlib_service *service, json_t *msg)
{
   json_t *updates = NULL;
   json_t *value;
   size_t index;
   int v = 0;

   updates = json_object_get(msg, "updates");
   json_array_foreach(updates, index, value) {
        json_t *val = json_array_get(value, 2);
        json_t *vv = json_object_get(val, "d");
        v = (uint32_t)json_integer_value(vv);
   }

   pthread_mutex_lock(&s->mutex);
   average = ((average + v)/2);
   if (average > 50) {
      mlib_metrics_composite_counter_incr (s->s,
                                     TMP_COND, TMP_GT_50);
   } else if (average < 50) {
      mlib_metrics_composite_counter_incr (s->s,
                                     TMP_COND, TMP_LT_50);
   }

   mlib_log_info("Value is %d and average is %d", v, average);
   pthread_mutex_unlock(&s->mutex);
}

void
get_average_temperature(void *link, json_t *rid, json_t *params) 
{
    if (!link || !rid ) {
        mlib_log_err("NULL input arguments");
        json_t *err = json_string("NULL input arguments");
        RPC.response(link, rid, err);
        json_decref(err);
        return;
    }

    pthread_mutex_lock(&s->mutex);
    json_t *avg = json_object();
    json_object_set_new_nocheck(avg, "average-temperature", json_integer(average));
    pthread_mutex_unlock(&s->mutex);

    RPC.response(link, rid, avg);
    json_decref(avg);
}

mlib_error_t
add_rpc_methods() 
{
    mlib_error_t rc = MLIB_SUCCESS;

    rc = RPC.operation_add(s->s, "getAvgTemperature", 
                                    "Return Average Temperature", "map",
                                    get_average_temperature, NULL, 0);
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

    /* Initialize average-temperature service */
    s->s = MLIB.service_initialize(SERVICE_NAME,
                                   initialized_cb, connected_cb,
                                   disconnected_cb, ready_cb);
    if (!s->s) {
        mlib_log_err("mlib_service_init failed for random temperature service.");
    }
    MLIB.service_wait_for_init(s->s);

    /* Add RPC method to this service */
    if (add_rpc_methods() != MLIB_SUCCESS) {
        mlib_log_err("%s: add_rpc_methods failed", __FUNCTION__);
        return 1;
    }

	if (mlib_metrics_composite_add(s->s, TMP_COND, "##ServiceMetric") 
				!= MLIB_SUCCESS) {
        mlib_log_err("%s: composite metric failed", __FUNCTION__);
        return 1;
	}

	if (mlib_metrics_composite_add_counter (s->s, TMP_COND, 
									TMP_GT_50) != MLIB_SUCCESS) {
        mlib_log_err("%s: composite metric item failed", __FUNCTION__);
        return 1;
	}
	if (mlib_metrics_composite_add_counter (s->s, TMP_COND, 
									TMP_LT_50) != MLIB_SUCCESS) {
        mlib_log_err("%s: composite metric item failed", __FUNCTION__);
        return 1;
	}

    /* Subscribe to Random temperature topic */
    PubSub.subscribe_v2(TEMPERATURE_RANDOM_TOPIC, subscribe_callback);

    /* Create a timer to periodically publish average temperature */
    s->timer = mlib_timer_create(publish_temperature, (void *) s);
    if (!s->timer) {
        mlib_log_err("%s: mlib_timer_create failed", __FUNCTION__);
        return 1;
    }
    mlib_timer_start(s->timer, s->interval);

    MLIB.wait_for_services_finish();
    return 0;
}

