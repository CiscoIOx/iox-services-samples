# use python2
# pip install websocket_client
# pip install oauth
# usage: webclient.py <websocket-url> <access-token>

import sys
import ssl
import os
import websocket
import oauth
from datetime import datetime

nbi_label = "nbi"
nbi_host = os.environ[nbi_label+"_IP_ADDRESS"]
nbi_port = os.environ[nbi_label+"_TCP_9999_PORT"]

websocket.enableTrace(True)
topic = "system.gps.parsed.event"
url = "wss://%s:%s/api/v1/mw/topics/%s" % (nbi_host, nbi_port, topic)
ws = websocket.create_connection(
    url,
    header = ["Authorization: Bearer " + oauth.access_token],
    sslopt= {"cert_reqs": ssl.CERT_NONE}
)

print "Websocket connected"

while True:
    print "RECVD: %s" % ws.recv()

