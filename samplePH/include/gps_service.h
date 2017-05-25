#ifndef _GPS_SVC_H
#define _GPS_SVC_H

#include <pthread.h>
#include <jansson.h>
#include "mlib_ph.h"
#include "mlib_timer.h"

#define GPS_PARSED_TOPIC "system.gps.parsed.event"
#define GPS_RAW_TOPIC "system.gps.raw.event"

typedef struct gps_state {
	mlib_ph_t *ph;

	char 	**gps_table;
	int		message_count;
	int		current_index;

	float	latitude;
	float	longitude;
	float	speed;
	int		fix_quality;

	json_t	*j_gps_metric;
	long	interval;
	pthread_mutex_t mutex;
	dataschema_t *ds;
	tuple_ctx_t *tuple_ctx;
	tuple_t *tuple;

	mlib_timer *timer;
	
	
} gps_state_t;

extern gps_state_t *gps_p;

gps_state_t *
GPS_create_state(char *fileName);

void
GPS_read_measure();

void
GPS_get_raw(void *link, json_t *rid, json_t *params);

void
GPS_get_parsed(void *link, json_t *rid, json_t *params);

#endif // _GPS_SVC_H
