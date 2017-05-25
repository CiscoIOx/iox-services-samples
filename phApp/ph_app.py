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
import thread
import time
import ph_dm_config

def web_socket_sub( protoName, topicName):
    print "Started thread"
    nbi_label = "nbi"
    nbi_host = os.environ[nbi_label+"_IP_ADDRESS"]
    nbi_port = os.environ[nbi_label+"_TCP_9999_PORT"]
    topic = topicName
    url = "wss://%s:%s/api/v1/mw/topics/%s" % (nbi_host, nbi_port, topic)

    while 1:
        try:
            websocket.enableTrace(True)
            ws = websocket.create_connection(
                   url,
                    header = ["Authorization: Bearer " + oauth.access_token],
                   sslopt= {"cert_reqs": ssl.CERT_NONE}
                 )
            while True:
                 print "RCVD<%s>: %s" %  (protoName, ws.recv())
        except:
            print "Web Socket exception"
            time.sleep(2)

def setup_modbus_data_models():
    ret = ph_dm_config.get_data_model("dataschemas", "VCNL4000SensorSchema")
    if ret == -1:
        ret = ph_dm_config.configure_data_model("dataschemas", "data_schema2.json")
        if ret == -1:
            return -1
    print ("VCNL schema success")
    ret = ph_dm_config.get_data_model("dataschemas", "GPSSchema")
    if ret == -1:
        ret = ph_dm_config.configure_data_model("dataschemas", "data_schema1.json")
        if ret == -1:
            return -1
    print ("GPS schema success")
    ret = ph_dm_config.get_data_model("devicetypes", "VCNL_GPS")
    if ret == -1:
        ret = ph_dm_config.configure_data_model("devicetypes", "device_type.json")
        if ret == -1:
            return -1
    print ("VCNL_GPS device type success")
    ret = ph_dm_config.get_data_model("devices", "IOX_Sim")
    if ret == -1:
        ret = ph_dm_config.configure_data_model("devices", "device.json")
        if ret == -1:
            return -1
    print ("IOX_Sim device success")
    return 0

def setup_dnp3_data_models():
    ret = ph_dm_config.get_data_model("dataschemas", "Dnp3Schema")
    print ("ret %d"% ret)
    if ret == -1:
        ret = ph_dm_config.configure_data_model("dataschemas", "data_schema3.json")
        if ret == -1:
            return -1
    print ("Dnp3 Schema success")
    ret = ph_dm_config.get_data_model("devicetypes", "Dnp3Type1")
    if ret == -1:
        ret = ph_dm_config.configure_data_model("devicetypes", "device_type1.json")
        if ret == -1:
            return -1
    print ("DNP3 device type success")
    ret = ph_dm_config.get_data_model("devices", "SampleDNP3Device")
    if ret == -1:
        ret = ph_dm_config.configure_data_model("devices", "device1.json")
        if ret == -1:
            return -1
    print ("SampleDNP3 device success")
    return 0

def delete_modbus_data_models():
    ph_dm_config.delete_data_model("devices", "IOX_Sim")
    ph_dm_config.delete_data_model("devicetypes", "VCNL_GPS")
    ph_dm_config.delete_data_model("dataschemas", "VCNL4000SensorSchema")
    ph_dm_config.delete_data_model("dataschemas", "GPSSchema")
    print ("Modbus Data models deleted")
    return 0
		
def delete_dnp3_data_models():
    ph_dm_config.delete_data_model("devices", "SampleDNP3Device")
    ph_dm_config.delete_data_model("devicetypes", "Dnp3Type1")
    ph_dm_config.delete_data_model("dataschemas", "Dnp3Schema")
    print ("DNP3 Data models deleted")
    return 0

def handler(signum, frame):
    print 'Signal handler called with signal', signum
    delete_modbus_data_models()
    delete_dnp3_data_models()
    sys.exit(0)

signal.signal(signal.SIGTERM, handler)
signal.signal(signal.SIGINT, handler)

if setup_modbus_data_models() == -1:
    print("Failed to Create Modbus Data Models")
    exit

if setup_dnp3_data_models() == -1:
    print("Failed to Create DNP3 Data Models")
    exit

print("Data Models successfully set")
	
# Create two threads as follows
try:
   thread.start_new_thread( web_socket_sub, ("MODBUS", "system.devices.IOX_Sim.**", ) )
   thread.start_new_thread( web_socket_sub, ("DNP3", "system.devices.SampleDNP3Device.**", ) )
except:
   print "Error: unable to start thread"

while 1:
   pass
