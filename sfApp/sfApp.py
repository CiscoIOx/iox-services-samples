#!/usr/bin/python

import sys
import ssl
import os
import signal
import oauth
from datetime import datetime
import thread
import time
import sf_config
import json
import itertools
import httplib
import string


def create_publish_topic(topic):
    nbi_label = "nbi"
    nbi_host = os.environ[nbi_label+"_IP_ADDRESS"]
    nbi_port = os.environ[nbi_label+"_TCP_9999_PORT"]

    headers = {"Content-Type": "application/json",
               "Authorization": "Bearer "+oauth.access_token,
               "Accept": "text/plain"}

    con = httplib.HTTPSConnection("%s:%s" % (nbi_host, nbi_port))
    con.request(
        "POST", 
        "/api/v1/mw/topics/"  + topic,
        None,
        headers
    )   
    response = con.getresponse()
    print("Publish topic create response: %s %s" % (response.status, response.reason)) 
    # Check for 2xx response code
    if response.status/100 != 2 :
        print ("Failed to create publish topic. Code {}".format(response.status))
        con.close()
        return -1
    print ("Success creating publish topic. Received response:")
    print response.read()
    con.close()
    return 0


def publish_to_topic(topic, data):
    nbi_label = "nbi"
    nbi_host = os.environ[nbi_label+"_IP_ADDRESS"]
    nbi_port = os.environ[nbi_label+"_TCP_9999_PORT"]

    payload = {
    "topic": topic,
    "message" : [{
        "values": [{"data":data}]
        }]
    } 
    headers = {"Content-Type": "application/json",
               "Authorization": "Bearer "+oauth.access_token}
    payload_str = json.dumps(payload)
    con = httplib.HTTPSConnection("%s:%s" % (nbi_host, nbi_port))
    con.request(
        "POST", 
        "/api/v1/mw/topics/publish",
        payload_str,
        headers
    )
    # print payload_str   
    response = con.getresponse()
    # Check for 2xx response code
    if response.status/100 != 2 :
        print ("Failed to publish msg to topic. Code {}".format(response.status))
        con.close()
        return -1
    con.close()
    return 0


def publish_data(topic):
    try:
        while True:
            for i in itertools.count(0):
                data = "{}-{}".format(i,string.ascii_letters)
                publish_to_topic(topic, data)
                time.sleep(0.05)
    except KeyboardInterrupt:
        print("Exiting...") 


def setup_mqtt_sf_policy():
    policy="mqttstoreandforward"
    policyId="mytopicPolicy"
    schema="sf_mytopic_schema.json"
    # Delete policy if its already there
    sf_config.delete_sf(policy, policyId)
    ret = sf_config.configure_sf(policy, schema)
    if ret == -1:
        print ("MQTT Policy Configuration Failed")
        return -1
    print ("MQTT Policy success")
    return 0

		
def handler(signum, frame):
    print 'Signal handler called with signal', signum
    sys.exit(0)



def main():
    # Setup signal handling for the process
    signal.signal(signal.SIGTERM, handler)
    signal.signal(signal.SIGINT, handler)
    
    # Generate the oauth token needed for everything
    oauth.InitToken()
   
    # NBI topic for publish
    topic = "mytopic"

    # Create the publish topic
    if create_publish_topic(topic) == -1:
        print("Failed to create publish topic " + topic)
        exit
    
    # Setup the Store & Forward policy, nbi topic name hardcoded in payload json
    if setup_mqtt_sf_policy() == -1:
        print("Failed to Create MQTT SF Policy")
        exit
    
    # Now start generating data for store & forward
    publish_data(topic)


if __name__ == "__main__":
    main()


