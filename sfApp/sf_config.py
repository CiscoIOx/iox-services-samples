import os
import httplib
import time
import oauth

def get_sf( policyService, policyName):
    nbi_label = "nbi"
    nbi_host = os.environ[nbi_label+"_IP_ADDRESS"]
    nbi_port = os.environ[nbi_label+"_TCP_9999_PORT"]

    headers = {
        "Authorization": "Bearer "+oauth.access_token
    }

    con = httplib.HTTPSConnection("%s:%s" % (nbi_host, nbi_port))
    con.request(
        "GET", 
        "/api/v1/mw/" + policyService + "/policies/" + policyName, 
        None, 
        headers
    )
    response = con.getresponse()
    print("%s %s" % (response.status, response.reason)) 
    if response.status != 200 :
        print ("Failed")
        con.close()
        return -1
    print ("Success")
    con.close()
    return 0


def configure_sf( policyService, fileName):
    nbi_label = "nbi"
    nbi_host = os.environ[nbi_label+"_IP_ADDRESS"]
    nbi_port = os.environ[nbi_label+"_TCP_9999_PORT"]

    with open ("resources/"+fileName, "r") as myfile:
        payload=myfile.read()
    print("%s" % (payload)) 
    send = {}
    headers = {"Content-Type": "application/json",
               "Authorization": "Bearer "+oauth.access_token,
               "Accept": "text/plain"}

    con = httplib.HTTPSConnection("%s:%s" % (nbi_host, nbi_port))
    con.request(
        "POST", 
        "/api/v1/mw/" + policyService + "/policies",
        payload,
        headers
    )
    response = con.getresponse()
    print("%s %s" % (response.status, response.reason)) 
    if response.status != 201 :
        print ("Failed")
        con.close()
        return -1
    print ("Success")
    print response.read()
    con.close()
    return 0


def delete_sf( policyService, policyName):
    nbi_label = "nbi"
    nbi_host = os.environ[nbi_label+"_IP_ADDRESS"]
    nbi_port = os.environ[nbi_label+"_TCP_9999_PORT"]

    headers = {
        "Authorization": "Bearer "+oauth.access_token
    }

    con = httplib.HTTPSConnection("%s:%s" % (nbi_host, nbi_port))
    con.request(
        "DELETE", 
        "/api/v1/mw/" + policyService + "/policies/" + policyName, 
        None,
        headers
    )
    response = con.getresponse()
    print("%s %s" % (response.status, response.reason)) 
    if response.status not in (204, 404) :
        print ("Failed")
        con.close()
        return -1
    print ("Success")
    con.close()
    return 0

