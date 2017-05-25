#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "minmea.h"
#include "mlib_ph.h"
#include "mlib_dmc.h"
#include "gps_service.h"
#include "mlib_log.h"

#define PROTOCOL  "gps"
#define MAX_COUNT  25
#define GPS_SCHEMA  "gpsSchema"
#define SERVICE_NAME "urn:cisco:system:service:protocolHandler:sample_gps"
#define PROV_SVC "urn:cisco:system:service:provisioning"

#define JSON_FILE "GPS_schema.txt"

gps_state_t *gps_p;

void
GPS_read_config() {
	mlib_error_t rc = MLIB_SUCCESS;

	rc = mlib_util_ini_config_getn("gps-config", "gps-config.interval",
			&gps_p->interval, "/data/package_config.ini");
	if (rc != MLIB_SUCCESS) {
		gps_p->interval = 5000;
	}
	mlib_log_info("GPS Interval is %ld", gps_p->interval);
}


void
GPS_publish_measure(void *arg) {

	GPS_read_measure();
	
	pthread_mutex_lock(&gps_p->mutex);
	mlib_ph_add_float_to_tuple(gps_p->ds, gps_p->tuple, gps_p->longitude, 0);
	mlib_ph_add_float_to_tuple(gps_p->ds, gps_p->tuple, gps_p->latitude, 1);
	mlib_ph_add_float_to_tuple(gps_p->ds, gps_p->tuple, gps_p->speed, 2);

	service_sdk_ph_publish_v1(gps_p->ph, gps_p->tuple_ctx, gps_p->tuple);
	
	pthread_mutex_unlock(&gps_p->mutex);
}

void gps_ready (mlib_service *service) {
    mlib_log_info("GPS protocol service %s ready",
                    service->name);
	if (gps_p != NULL && gps_p->timer != NULL) {
    	mlib_log_info("Starting the timer");
		gps_p->timer = mlib_timer_create(GPS_publish_measure, (void *) gps_p);
		mlib_timer_start(gps_p->timer, gps_p->interval);	
	}
}

void gps_disc (mlib_service *service) {
    mlib_log_info("GPS protocol service %s disconnected",
                    service->name);
	if (gps_p != NULL && gps_p->timer != NULL) {
    	mlib_log_info("stopping the timer");
		mlib_timer_stop(gps_p->timer);	
		MLIB.free(gps_p->timer);
		gps_p->timer = NULL;
	}
}

mlib_ph_t *
GPS_set_up_ph() {
	ph_svc_conn_cbs_t *ph_conn_cbs = mlib_ph_conn_cb_init(
                    NULL, NULL, gps_disc, gps_ready);

    mlib_ph_t *gps_ph = service_sdk_ph_init(
                                PROTOCOL, SERVICE_NAME,
                                ph_conn_cbs, NULL);

	if (gps_ph == NULL) {
		mlib_log_err("Protocol handler initialization failed for GPS");
        return NULL;
	}

	MLIB.service_wait_for_init(gps_ph->protocolService);
	sleep(1);
	return (gps_ph);
}

void
GPS_set_publish_time(void *link, json_t *rid, json_t *params)
{
    if (!link || !rid ) {
        mlib_log_err("NULL input arguments");
        json_t *err = json_string("NULL input arguments");
        RPC.response(link, rid, err);
        json_decref(err);
        return;
    }
	char *u = json_dumps(params, JSON_INDENT(2));
	mlib_log_info("param is %s", u);
    MLIB.free(u);

    int t = 0;
    json_t *value = json_object_get(params, "value");
    if (value) {
        json_t *time = json_object_get(value, "gps-interval-seconds");
        if (time) {
            t = (uint32_t)json_integer_value(time);
        } else {
    		mlib_log_err("The JSON object was not proper");
        	json_t *err = json_string("Invalid JSON arguments");
        	RPC.response(link, rid, err);
        	json_decref(err);
			return;
		}
    } else {
    	mlib_log_err("The JSON object was not proper");
        json_t *err = json_string("Invalid JSON arguments");
        RPC.response(link, rid, err);
        json_decref(err);
		return;
	}

    pthread_mutex_lock(&gps_p->mutex);

    gps_p->interval = t * 1000;
    mlib_log_info("Resetting temperature publish time to %ld seconds", 
					gps_p->interval);

	mlib_timer_start(gps_p->timer, gps_p->interval);	

    pthread_mutex_unlock(&gps_p->mutex);

    json_t *obj = json_object();
    RPC.response(link, rid, obj);
    json_decref(obj);
}

mlib_error_t
GPS_add_rpc_methods() {
	mlib_error_t rc = MLIB_SUCCESS;
    mlib_param set_gps_interval[] = {
                {"value","map"},
           };

    rc = RPC.operation_add(gps_p->ph->protocolService, 
								"getGPSRaw", "Return GPS Raw data",
                                    "map", GPS_get_raw,
                                    NULL, 0);

	if (rc != MLIB_SUCCESS) return rc;
    rc = RPC.operation_add(gps_p->ph->protocolService, 
								"getGPSParsed", "Return GPS Parsed data",
                                    "map", GPS_get_parsed,
                                    NULL, 0);

	if (rc != MLIB_SUCCESS) return rc;
    rc = RPC.operation_add(gps_p->ph->protocolService, 
								"setGPSInterval", 
								"Set GPS Publishing interval in Seconds",
                                    "string", GPS_set_publish_time,
                                    set_gps_interval, 1);
	return rc;
}

mlib_error_t
GPS_create_schema() {
	mlib_error_t rc = MLIB_SUCCESS;
	int count = 0;
	RPCInvoke *r = NULL;
	json_error_t err;

	json_t *schema_json = json_load_file(JSON_FILE, 0, &err);
	if (schema_json == NULL) {
        mlib_log_err("could not load JSON.\n");
        return (MLIB_RPC_INVOKE_FAILURE);
	}

    json_t *params = json_object();
    json_object_set_new_nocheck(params, "value", schema_json);

	char *u = json_dumps(params, JSON_INDENT(2));
	mlib_log_info("schema is %s", u);
    MLIB.free(u);

    while (rc != MLIB_RETRY_LATER && count < MAX_COUNT) {
        rc = RPC.invoke_b(gps_p->ph->protocolService, &r,
                                    PROV_SVC,
                                    "addDataSchema",
                                    params);
        if (rc != MLIB_RETRY_LATER && rc != MLIB_SERVICE_NOT_PRESENT) {
            break;
        } else {
            count++;
            sleep(1);
        }
    }
	if (rc != MLIB_SUCCESS || r == NULL) {
		json_decref(params);
        return (MLIB_RPC_INVOKE_FAILURE);
	}

	RPC.invoke_b_wait_for_response(r);

    mlib_log_info("Creation of GPS data schema done %d:%d result", rc, r->rc);

	RPC.invoke_b_free_response(r);
	json_decref(params);
	return (rc);
}

mlib_error_t
GPS_check_schema() {
	mlib_error_t rc = MLIB_SUCCESS;
	dataschema_t *ds = NULL;
	if ( (ds = dmc_get_dataschema(GPS_SCHEMA)) == NULL) {
 		GPS_create_schema(); 
		ds = dmc_get_dataschema(GPS_SCHEMA); 
		if (ds == NULL)  return (MLIB_RPC_INVOKE_FAILURE);
	}
	gps_p->ds = ds;
	gps_p->tuple_ctx = mlib_ph_tuple_create_ctx_v1(ds, GPS_PARSED_TOPIC);
	gps_p->tuple = mlib_ph_tuple_create(ds);
	if (gps_p->tuple_ctx == NULL || gps_p->tuple == NULL)  
		return (MLIB_RPC_INVOKE_FAILURE);

	return (MLIB_SUCCESS);
}

int
main(int argc, char **argv) {
	mlib_error_t rc = MLIB_SUCCESS;

	if (MLIB.initialize(argc, argv) != MLIB_SUCCESS) {
        printf("mlib_initialize failed.\n");
        return 1;
    }

	gps_p = GPS_create_state("GPS_dataset.txt");
	if (gps_p == NULL) {
        mlib_log_err("GPS create state failed.\n");
        return 1;
	}

	GPS_read_config(); 
	if (rc != MLIB_SUCCESS) {
        mlib_log_err("GPS method registration failed.\n");
        return 1;
	}

	gps_p->ph = GPS_set_up_ph();
	if (gps_p->ph == NULL) return (-1);

	rc = GPS_add_rpc_methods(); 
	if (rc != MLIB_SUCCESS) return (-1);

	rc = GPS_check_schema(); 
	if (rc != MLIB_SUCCESS) return (-1);

	gps_p->timer = mlib_timer_create(GPS_publish_measure, (void *) gps_p);
	if (gps_p->timer == NULL) return (-1);
	mlib_timer_start(gps_p->timer, gps_p->interval);	

	MLIB.wait_for_services_finish();
    return 0;

}
/* vim: set ts=4 sw=4 et: */

