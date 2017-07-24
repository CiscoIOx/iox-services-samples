# use python2
# pip install websocket-client
# usage: webclient.py <websocket-url> <access-token>

import sys
import httplib
import ssl
import os
import websocket
import oauth
from datetime import datetime
import logging
from logging.handlers import RotatingFileHandler

# create a log file
LOG_FILENAME = '/data/logs/motion.log'

# create formatter and add it to the handler
formatter = logging.Formatter('%(levelname)s - %(message)s')
# create logger with 'gps'
logger = logging.getLogger("gps")
# set the logger level
logger.setLevel(logging.INFO)
# Use Rotatingfilehanlder and set max size
handler = RotatingFileHandler(LOG_FILENAME, maxBytes=1024)
handler.setFormatter(formatter)
logger.addHandler(handler)

nbi_label = "nbi"
nbi_host = os.environ[nbi_label+"_IP_ADDRESS"]
nbi_port = os.environ[nbi_label+"_TCP_9999_PORT"]

def on_message(ws, message):
   logger.info(str(datetime.now())+" Received: "+message)

def on_error(ws, error):
    logger.info(error)

def on_close(ws):
    logger.info("websocket closed")

def on_open(ws):
    logger.info("websocket opened successfully")

con = httplib.HTTPSConnection("%s:%s" % (nbi_host, nbi_port))
con.request(
    method = "GET",
    url = "/api/v1/mw/motion/gyroscope",
    headers = {
        "Authentication": "Bearer "+oauth.access_token
    }
)
response = con.getresponse()
logger.info("%s %s" % (response.status, response.reason))
logger.info(response.read())
con.close()

conn = httplib.HTTPSConnection("%s:%s" % (nbi_host, nbi_port))
conn.request(
    method = "GET",
    url = "/api/v1/mw/motion/accelerometer",
    headers = {
        "Authentication": "Bearer "+oauth.access_token
    }
)
response = conn.getresponse()
logger.info("%s %s" % (response.status, response.reason))
logger.info(response.read())
conn.close()


websocket.enableTrace(True)

url = "wss://%s:%s/api/v1/mw/topics/**" % (nbi_host, nbi_port)
ws = websocket.create_connection(
    url,
    header = ["Authorization: Bearer " + oauth.access_token],
    sslopt= {"cert_reqs": ssl.CERT_NONE}
)
while True:
    logger.info("RECVD: %s" % ws.recv())
