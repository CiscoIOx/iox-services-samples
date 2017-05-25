#include "mlib_service.h"
#include "mlib_log.h"
#include "mlib_api.h"

void
Sample_invoke_hello_world(void *link, json_t *rid, json_t *params) {
    if (!link || !rid ) {
        mlib_log_err("NULL input arguments");
        RPC.response(link, rid,
                        json_string("NULL input arguments"));
        return;
    }

    json_t *hello = json_object();
    json_object_set_new(hello, "Greeting", 
			json_string("Hello World from IOx Micro Services"));
                    
    RPC.response(link, rid, hello);
	json_decref(hello);
}

mlib_error_t
Sample_add_rpc_method(mlib_service *svc) {
    mlib_error_t rc = MLIB_SUCCESS;

    rc = RPC.operation_add(svc,
                                "invokeHelloWorld", "Return Hello World",
                                    "map", Sample_invoke_hello_world,
                                    NULL, 0);

    return rc;
}

void
Sample_connection_ready(mlib_service *service) {
	mlib_log_info("MLIB service <Hello World> is Connected");
}

int
main(int argc, char **argv) {

	if (MLIB.initialize(argc, argv) != MLIB_SUCCESS) {
        printf("\nMlib Initialization failed\n");
        return(1);
    }

    mlib_service *hello = MLIB.service_initialize("Hello_World",
                                     NULL, NULL,
                                     NULL, Sample_connection_ready);
    if (!hello) {
        mlib_log_err("mlib_service_init failed for Hello World Service.");
    }

    MLIB.service_wait_for_init(hello);

	mlib_log_info("MLIB service <Hello World> is all set");

	if (Sample_add_rpc_method(hello) != MLIB_SUCCESS) {
        mlib_log_err("\nMlib RPC method registration failed\n");
        return(1);
    }

	MLIB.wait_for_services_finish();
}
