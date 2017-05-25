#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "mlib_ph.h"
#include "mlib_dmc.h"
#include "mlib_log.h"

#define PROTOCOL  "sample"
#define MAX_COUNT  25
#define SERVICE_NAME "urn:cisco:system:service:protocolHandler:sample_rpc"
#define PROV_SVC "urn:cisco:system:service:provisioning"

#define N_THREADS 10
#define N_ITER 1000

mlib_ph_t *ph;
char *schemanames[] = {"rpcSchema1", "rpcSchema2", "rpcSchema3"};
char *filenames[] = {"RPC_schema1.txt", "RPC_schema2.txt", 
					 "RPC_schema3.txt"};
volatile int status = 0;

void rpc_ready (mlib_service *service) {
    mlib_log_info("RPC service %s ready", service->name);
	status = 1;
}

void rpc_disc (mlib_service *service) {
    mlib_log_info("RPC service %s disconnected", service->name);
	status = 0;
}

mlib_ph_t *
RPC_setup_ph() {
	ph_svc_conn_cbs_t *ph_conn_cbs = mlib_ph_conn_cb_init(
                    NULL, NULL, rpc_disc, rpc_ready);

    mlib_ph_t *ph = service_sdk_ph_init(
                                PROTOCOL, SERVICE_NAME,
                                ph_conn_cbs, NULL);

	if (ph == NULL) {
		mlib_log_err("Protocol handler initialization failed");
        return NULL;
	}

	MLIB.service_wait_for_init(ph->protocolService);
	return (ph);
}


mlib_error_t
RPC_create_schema(char *filename) {
	mlib_error_t rc = MLIB_SUCCESS;
	int count = 0;
	RPCInvoke *r = NULL;
	json_error_t err;

	json_t *schema_json = json_load_file(filename, 0, &err);
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
        rc = RPC.invoke_b(ph->protocolService, &r,
                                    PROV_SVC,
                                    "addDataSchema",
                                    params);
        if (rc != MLIB_RETRY_LATER && rc != MLIB_SERVICE_NOT_PRESENT) {
            break;
        } else {
            count++;
            usleep(100);
        }
    }
	if (rc != MLIB_SUCCESS || r == NULL) {
		json_decref(params);
        return (MLIB_RPC_INVOKE_FAILURE);
	}

	RPC.invoke_b_wait_for_response(r);

    mlib_log_info("Creation of GPS data schema done %d:%d result", rc, r->rc);

	if (r->rc != MLIB_SUCCESS) {
        mlib_log_err ("Response to Get Scema is Not Success, error : %s",
                      mlib_strerr(r->rc));
        RPC.invoke_b_free_response(r);
        json_decref(params);
        return (MLIB_RPC_INVOKE_FAILURE);
    }

	RPC.invoke_b_free_response(r);
	json_decref(params);
	return (rc);
}

mlib_error_t
RPC_delete_schema(char *schemaname) {
	mlib_error_t rc = MLIB_SUCCESS;
	int count = 0;
	RPCInvoke *r = NULL;
	json_error_t err;

    json_t *params = json_object();
    json_object_set_new_nocheck(params, "dataSchemaId", 
										json_string(schemaname));

	char *u = json_dumps(params, JSON_INDENT(2));
	mlib_log_info("params is %s", u);
    MLIB.free(u);

    while (rc != MLIB_RETRY_LATER && count < MAX_COUNT) {
        rc = RPC.invoke_b(ph->protocolService, &r,
                                    PROV_SVC,
                                    "deleteDataSchema",
                                    params);
        if (rc != MLIB_RETRY_LATER && rc != MLIB_SERVICE_NOT_PRESENT) {
            break;
        } else {
            count++;
            usleep(100);
        }
    }
	if (rc != MLIB_SUCCESS || r == NULL) {
		json_decref(params);
        return (MLIB_RPC_INVOKE_FAILURE);
	}

	RPC.invoke_b_wait_for_response(r);

    mlib_log_info("Deletion of GPS data schema done %d:%d result", rc, r->rc);

	if (r->rc != MLIB_SUCCESS) {
        mlib_log_err ("Response to Get Scema is Not Success, error : %s",
                      mlib_strerr(r->rc));
        RPC.invoke_b_free_response(r);
        json_decref(params);
        return (MLIB_RPC_INVOKE_FAILURE);
    }

	RPC.invoke_b_free_response(r);
	json_decref(params);
	return (rc);
}

mlib_error_t
RPC_get_schema(char *schemaname) {
	mlib_error_t rc = MLIB_SUCCESS;
	int count = 0;
	RPCInvoke *r = NULL;
	json_error_t err;

    json_t *params = json_object();
    json_object_set_new_nocheck(params, "dataSchemaId", 
										json_string(schemaname));

	char *u = json_dumps(params, JSON_INDENT(2));
	mlib_log_info("params is %s", u);
    MLIB.free(u);

    while (rc != MLIB_RETRY_LATER && count < MAX_COUNT) {
        rc = RPC.invoke_b(ph->protocolService, &r,
                                    PROV_SVC,
                                    "getDataSchema",
                                    params);
        if (rc != MLIB_RETRY_LATER && rc != MLIB_SERVICE_NOT_PRESENT) {
            break;
        } else {
            count++;
            usleep(100);
        }
    }
	if (rc != MLIB_SUCCESS || r == NULL) {
		json_decref(params);
        return (MLIB_RPC_INVOKE_FAILURE);
	}

	RPC.invoke_b_wait_for_response(r);

    mlib_log_info("Get of RPC data schema done %d:%d result", rc, r->rc);

	if (r->rc != MLIB_SUCCESS) {
        mlib_log_err ("Response to Get Scema is Not Success, error : %s",
                      mlib_strerr(r->rc));
        RPC.invoke_b_free_response(r);
        json_decref(params);
        return (MLIB_RPC_INVOKE_FAILURE);
    }

    json_t *msg1 = r->resp;
    if(msg1 == NULL) return(MLIB_RPC_INVOKE_FAILURE);

    json_t *json_array1 = json_object_get(msg1, "updates");
    if(json_array1 == NULL) return MLIB_RPC_INVOKE_FAILURE;
    json_t *json_array2 = json_array_get(json_array1, 0);
    if(json_array2 == NULL) return MLIB_RPC_INVOKE_FAILURE;
    json_t *msg = json_array_get(json_array2, 0);
    if(msg == NULL) return MLIB_RPC_INVOKE_FAILURE;

    dataschema_t *ds = mlib_parse_dataschema(msg);
	if (ds == NULL) {
        mlib_log_err ("Parsinng dataschema failed");
		return (MLIB_RPC_INVOKE_FAILURE);
	}

	u = json_dumps(msg, JSON_INDENT(2));
	mlib_log_info("schema is %s", u);
    MLIB.free(u);

	mlib_free_dataschema(&ds);
	RPC.invoke_b_free_response(r);
	json_decref(params);
	return (rc);
}

void *
interate_through_schemas(void *ptr) {
	int i;
	int j;
	mlib_error_t rc = MLIB_SUCCESS;
	
	for (j=0; j < N_ITER; j++) {
		for (i=0; i<3; i++) {
			if (status == 0) {
            	mlib_log_warn("Waiting till connection comes back\n");
				usleep(10);
			}
			rc = RPC_get_schema(schemanames[i]); 
			if (rc != MLIB_SUCCESS) {
            	mlib_log_err("Get Schema failed:[%d]", rc);
				mlib_metrics_meter_mark (ph->protocolService, "RPC_MISSES");
			} else {
				mlib_metrics_meter_mark (ph->protocolService, "RPC_HITS");
			}
		}
	}
	return (NULL);
}

void
create_threads() {
	int i = 0;
    int err;
	pthread_t tid[N_THREADS];

    while(i < N_THREADS)
    {
        err = pthread_create(&(tid[i]), NULL, &interate_through_schemas, NULL);
        if (err != 0) {
            mlib_log_err("can't create thread :[%s]", strerror(err));
        } else {
            mlib_log_warn("Thread-%d created successfully\n", (i+1));
		}
        i++;
    }

	for (i=0; i<N_THREADS; i++) {
		if(pthread_join(tid[i], NULL)) {
			mlib_log_err("Error joining thread\n");
		}
	}
	mlib_log_warn("Threads ALL Completed");

}

int
main(int argc, char **argv) {
	mlib_error_t rc = MLIB_SUCCESS;
	int i;
	int j;

	if (MLIB.initialize(argc, argv) != MLIB_SUCCESS) {
        printf("mlib_initialize failed.\n");
        return 1;
    }

	ph = RPC_setup_ph();
	if (ph == NULL) return (-1);

	if (mlib_metrics_add_meter (ph->protocolService, "RPC_HITS") 
								!= MLIB_SUCCESS) {
    	mlib_log_err("add meter failed");
    	return (-1);
	}
	if (mlib_metrics_add_meter (ph->protocolService, "RPC_MISSES") 
								!= MLIB_SUCCESS) {
    	mlib_log_err("add meter failed");
    	return (-1);
	}


	for (i=0; i<3; i++) {
		rc = RPC_create_schema(filenames[i]); 
		if (rc != MLIB_SUCCESS) return (-1);
		mlib_metrics_meter_mark (ph->protocolService, "RPC_HITS");
	}

	create_threads(); 

	for (i=0; i<3; i++) {
		rc = RPC_delete_schema(schemanames[i]); 
		if (rc != MLIB_SUCCESS) return (-1);
		mlib_metrics_meter_mark (ph->protocolService, "RPC_HITS");
	}

	//MLIB.service_stop(ph->protocolService);
	//MLIB.stop();
	MLIB.wait_for_services_finish();
    return 0;

}
/* vim: set ts=4 sw=4 et: */

