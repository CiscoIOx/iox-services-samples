# use python2
# pip install websocket_client
# usage: webclient.py <websocket-url> <access-token>
#!/usr/bin/python

import sys
import ssl
import os
import signal
import websocket
import oauth
from datetime import datetime
import time
import mqtt_api
import traceback
from threading import Thread

def web_socket_sub( protoName, topicName):
    print "Started thread"
    nbi_label = "nbi"
    nbi_host = os.environ[nbi_label+"_IP_ADDRESS"]
    nbi_port = os.environ[nbi_label+"_TCP_9999_PORT"]
    topic = topicName
    url = "wss://%s:%s/api/v1/mw/topics/%s" % (nbi_host, nbi_port, topic)

    websocket.enableTrace(True)
    try:
        ws = websocket.create_connection(                            
                   url,                                                  
                   header = ["Authorization: Bearer " + oauth.access_token],
                   sslopt= {"cert_reqs": ssl.CERT_NONE}
                 ) 
    except Exception, err:                                                              
        print Exception, err
        time.sleep(2)

    while 1:
        try:
            print "RCVD<%s>: %s" %  (protoName, ws.recv())
        except Exception, err:                                                              print Exception, err


def handler(signum, frame):
    print 'Signal handler called with signal', signum
    sys.exit(0)

signal.signal(signal.SIGTERM, handler)
signal.signal(signal.SIGINT, handler)

mqtt_conf = mqtt_api.get_mqtt_policy()
if mqtt_conf == None :
    print("MQTT Config Read failed")
    sys.exit(0)
ret = mqtt_api.configure_mqtt_policy("mqtt_policy.json")
if ret == -1 :
    print("MQTT Config failed")
    sys.exit(0)
print("MQTT Configuration successfully done")
	
try:
   t = Thread(target=web_socket_sub, args=("Store-And-Forward", "system.devices.**"))
   t.start()
except:
   print "Error: unable to start thread"

t.join()
