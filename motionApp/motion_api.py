import os
import httplib
import time
import oauth
import urllib2

def get_motion_config( ):
    nbi_label = "nbi"
    nbi_host = os.environ[nbi_label+"_IP_ADDRESS"]
    nbi_port = os.environ[nbi_label+"_TCP_9999_PORT"]

    print("%s %s" % (nbi_host, nbi_port)) 
    headers = {
        "Authorization": "Bearer "+oauth.access_token
    }

    print "Getting config"
    con = httplib.HTTPSConnection("%s:%s" % (nbi_host, nbi_port))
    con.request(
        "GET", 
        "/api/v1/mw/motion/config",
	None,
        headers
    )
    print "Done Getting config"
    response = con.getresponse()
    print("%s" % (response.read()))
    if response.status != 200 :
        print ("Failed")
        con.close()
        return None 
    print ("Success")
    con.close()
    return response


def configure_motion( fileName):
    nbi_label = "nbi"
    nbi_host = os.environ[nbi_label+"_IP_ADDRESS"]
    nbi_port = os.environ[nbi_label+"_TCP_9999_PORT"]

    with open ("resources/"+fileName, "r") as myfile:
        payload=myfile.read()
    print("payload %s" % (payload)) 
    send = {}
    headers = {"Content-Type": "application/json",
               "Authorization": "Bearer "+oauth.access_token,
               "Accept": "text/plain"}

    print "Posting config"
    con = httplib.HTTPSConnection("%s:%s" % (nbi_host, nbi_port))
    con.request(
        "POST", 
        "/api/v1/mw/motion/config",
        payload,
        headers
    )
    print "Done Posting config"
    response = con.getresponse()
    print("%s %s %s" % (response.status, response.reason, response.read())) 
    if response.status != 200 :
        print ("Failed")
        con.close()
        return -1
    print ("Success")
    con.close()
    return 0


#configure_data_model("dataschemas", "data_schema1.json")
#configure_data_model("dataschemas", "data_schema2.json")
#configure_data_model("devicetypes", "device_type.json")
#configure_data_model("devices", "device.json")
#time.sleep(2)
#delete_data_model("devices", "IOX_Sim")
#delete_data_model("devicetypes", "VCNL_GPS")
#delete_data_model("dataschemas", "GPSSchema")
#delete_data_model("dataschemas", "VCNL4000SensorSchema")
