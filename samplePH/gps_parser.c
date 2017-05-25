#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "minmea.h"
#include "gps_service.h"
#include "mlib_service.h"
#include "mlib_log.h"
#include "assert.h"

static int get_line_count(char *fileName) {
	FILE *fp = fopen(fileName , "r");
	char c;
	int count=0;

	if(fp == NULL) 
	{
		perror("Error opening file");
		return(0);
	}

	for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;

	printf("Count is %d\n", count);
    fclose(fp);
	return (count);
}

gps_state_t *GPS_create_state(char *fileName)
{
	FILE *fp;
	gps_state_t *gps_p;
	int i, count = 0;
	int message_count = get_line_count(fileName);
	if (message_count == 0) {
		return(NULL);
	}

	fp = fopen(fileName , "r");
	if(fp == NULL) 
	{
		perror("Error opening file");
		return(NULL);
	}

	gps_p = (gps_state_t *) MLIB.malloc(sizeof(gps_state_t));
	if(gps_p == NULL) 
	{
		mlib_log_err("Error allocating gps state");
		return(NULL);
	}
	memset(gps_p, 0, sizeof(gps_state_t));

	gps_p->gps_table = (char **) MLIB.malloc(sizeof(char **) * message_count);
	if(gps_p->gps_table == NULL) 
	{
		mlib_log_err("Error allocating gps table");
		MLIB.free(gps_p);
		return(NULL);
	}

	pthread_mutex_init(&gps_p->mutex, NULL);

	for (i=0; i<message_count; i++)
		gps_p->gps_table[i] = (char *) MLIB.malloc(sizeof(char)*MINMEA_MAX_LENGTH);

    while (fgets(gps_p->gps_table[count], MINMEA_MAX_LENGTH, fp) != NULL) {
		count++;
    }

	assert(count == message_count);
	gps_p->message_count = message_count;

	fclose(fp);

	return gps_p;
}

json_t * 
GPS_parse_message(char  *line) {
	json_t *json_counter = json_object();
	int i;
	char *buffer = (char *)MLIB.malloc(MINMEA_MAX_LENGTH * sizeof(char));
	if (buffer == NULL) return json_null();

	mlib_log_debug("Line parsed is %s", line);

    pthread_mutex_lock(&gps_p->mutex);
	switch (minmea_sentence_id(line, false)) {
            case MINMEA_SENTENCE_RMC: {
                struct minmea_sentence_rmc frame;
                if (minmea_parse_rmc(&frame, line)) {
					sprintf(buffer, "%f", 
						(gps_p->latitude = minmea_tocoord(&frame.latitude)));
					json_object_set_new_nocheck(json_counter,
                                             "latitude",
                                             json_string(buffer));
					sprintf(buffer, "%f", 
						(gps_p->longitude = minmea_tocoord(&frame.longitude)));
					json_object_set_new_nocheck(json_counter,
                                             "longitude",
                                             json_string(buffer));
					sprintf(buffer, "%f", 
						(gps_p->speed = minmea_tocoord(&frame.speed)));
					json_object_set_new_nocheck(json_counter,
                                             "speed",
                                             json_string(buffer));
                }
                else {
                    mlib_log_err("$xxRMC sentence is not parsed\n");
                }
            } break;

            case MINMEA_SENTENCE_GGA: {
                struct minmea_sentence_gga frame;
                if (minmea_parse_gga(&frame, line)) {
					sprintf(buffer, "%d", 
						(gps_p->fix_quality = frame.fix_quality));
					json_object_set_new_nocheck(json_counter,
                                             "fix_quality",
                                             json_string(buffer));
                }
                else {
                    mlib_log_err("$xxGGA sentence is not parsed\n");
                }
            } break;

            case MINMEA_SENTENCE_GST: {
                struct minmea_sentence_gst frame;
                if (minmea_parse_gst(&frame, line)) {
					sprintf(buffer, "%f", 
						minmea_tofloat(&frame.latitude_error_deviation));
					json_object_set_new_nocheck(json_counter,
                                             "latitude_deviation",
                                             json_string(buffer));
					sprintf(buffer, "%f", 
						minmea_tofloat(&frame.longitude_error_deviation));
					json_object_set_new_nocheck(json_counter,
                                             "longitude_deviation",
                                             json_string(buffer));
					sprintf(buffer, "%f", 
						minmea_tofloat(&frame.altitude_error_deviation));
					json_object_set_new_nocheck(json_counter,
                                             "altitude_deviation",
                                             json_string(buffer));
                }
                else {
                    mlib_log_err("$xxGST sentence is not parsed\n");
                }
            } break;

            case MINMEA_SENTENCE_GSV: {
                struct minmea_sentence_gsv frame;
                if (minmea_parse_gsv(&frame, line)) {
					sprintf(buffer, "%d of %d", 
										frame.msg_nr, frame.total_msgs);
					json_object_set_new_nocheck(json_counter,
                                             "message_order",
                                             json_string(buffer));
					sprintf(buffer, "%d",  frame.total_sats);
					json_object_set_new_nocheck(json_counter,
                                             "total_satellites",
                                             json_string(buffer));
					sprintf(buffer, "%d",  frame.total_sats);
					json_object_set_new_nocheck(json_counter,
                                             "total_satellites",
                                             json_string(buffer));
									
                    for (i = 0; i < 4; i++) {
						json_t *json_sats = json_object();
						char buffer_1[32];
						sprintf(buffer, "%d",  frame.sats[i].nr);
						json_object_set_new_nocheck(json_sats,
                                             "nr",
                                             json_string(buffer));
						sprintf(buffer, "%d",  frame.sats[i].elevation);
						json_object_set_new_nocheck(json_sats,
                                             "elevation",
                                             json_string(buffer));
						sprintf(buffer, "%d",  frame.sats[i].azimuth);
						json_object_set_new_nocheck(json_sats,
                                             "azimuth",
                                             json_string(buffer));
						sprintf(buffer, "%d",  frame.sats[i].snr);
						json_object_set_new_nocheck(json_sats,
                                             "snr",
                                             json_string(buffer));
						sprintf(buffer_1, "satellite-%d", i+1);
						json_object_set_new_nocheck(json_counter, buffer_1, json_sats); 
					}
                }
                else {
                    mlib_log_err("$xxGSV sentence is not parsed\n");
                }
            } break;

            case MINMEA_INVALID: {
                mlib_log_err("$xxxxx sentence is not valid\n");
            } break;

            default: {
                mlib_log_err("$xxxxx sentence is not parsed\n");
            } break;
        }
    	pthread_mutex_unlock(&gps_p->mutex);

		MLIB.free(buffer);
		char *d = json_dumps(json_counter, JSON_ENCODE_ANY | JSON_INDENT(2));
		mlib_log_info("Parsed json %s", d);
		MLIB.free(d);
		return(json_counter);
}

void
GPS_read_measure() {

    pthread_mutex_lock(&gps_p->mutex);
    int index = gps_p->current_index++;
    gps_p->current_index %= gps_p->message_count;
    char *line = gps_p->gps_table[index];
    pthread_mutex_unlock(&gps_p->mutex);

	json_t *raw = json_object();
	json_object_set_new_nocheck(raw, "NMEA", json_string(line)); 
	PubSub.publish_v1(gps_p->ph->protocolService, GPS_RAW_TOPIC,
                            raw);
	json_decref(raw);
	pthread_mutex_lock(&gps_p->mutex);
	if (gps_p->j_gps_metric != NULL) json_decref(gps_p->j_gps_metric);
	pthread_mutex_unlock(&gps_p->mutex);
    gps_p->j_gps_metric = GPS_parse_message(line);
}

void
GPS_get_raw(void *link, json_t *rid, json_t *params) {
	if (!link || !rid ) {
        mlib_log_err("NULL input arguments");
        json_t *err = json_string("NULL input arguments");
        RPC.response(link, rid, err);
        json_decref(err);
        return;
    }

	json_t *raw = json_object();
	pthread_mutex_lock(&gps_p->mutex);
	json_object_set_new_nocheck(raw, "NMEA", json_string(
					gps_p->gps_table[gps_p->current_index])); 
	pthread_mutex_unlock(&gps_p->mutex);
	RPC.response(link, rid, raw);
	json_decref(raw);
}

void
GPS_get_parsed(void *link, json_t *rid, json_t *params) {
	if (!link || !rid ) {
        json_t *err = json_string("NULL input arguments");
        RPC.response(link, rid, err);
        json_decref(err);
        return;
    }
	pthread_mutex_lock(&gps_p->mutex);

	if (gps_p->j_gps_metric != NULL) {
		RPC.response(link, rid, gps_p->j_gps_metric);
	} else {
		json_t *reply = json_string("No data");
		RPC.response(link, rid, reply);
		json_decref(reply);
	}

	pthread_mutex_unlock(&gps_p->mutex);
}
