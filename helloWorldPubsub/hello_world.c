#include "mlib_service.h"
#include "mlib_log.h"
#include "mlib_api.h"
#include "mlib_timer.h"

void
Hello_publish_loop(void *arg) {
	mlib_service *svc = (mlib_service *) arg;
	json_t *jHello = json_object();

    json_object_set_new(jHello, "Greeting",
            json_string("Hello World from IOx Micro Services"));

	PubSub.publish_v1(svc, "custom.helloWorld", jHello);
	json_decref(jHello);
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

    mlib_service *hello = MLIB.service_initialize("Hello_World_Pubsub",
                                     NULL, NULL,
                                     NULL, Sample_connection_ready);
    if (!hello) {
        mlib_log_err("mlib_service_init failed for Hello World Service.");
    }

    MLIB.service_wait_for_init(hello);

	mlib_log_info("MLIB service <Hello World> is all set");

	mlib_timer *timer = mlib_timer_create(Hello_publish_loop, (void *) hello);
	mlib_timer_start(timer, 2000);

	MLIB.wait_for_services_finish();
}
