import os
import base64
import urlparse
import httplib
import json
import urllib

client_id = os.environ["OAUTH_CLIENT_ID"]
client_secret = os.environ["OAUTH_CLIENT_SECRET"]
token_url = os.environ["OAUTH_TOKEN_URL"]

#logger.info("client_id: "+client_id)
#logger.info("client_secret: "+client_secret)
#logger.info("token_url: "+token_url)

tokens = urlparse.urlparse(token_url)
if tokens.scheme == 'https':
    con = httplib.HTTPSConnection(tokens.netloc)
else:
    con = httplib.HTTPSConnection(tokens.netloc)

con.request(
    method = "POST", 
    url = tokens.path, 
    headers = {
        "Content-Type": "application/x-www-form-urlencoded",
        "Authorization": "Basic " + base64.b64encode("%s:%s" % (client_id, client_secret))
    },
    body= urllib.urlencode({'grant_type': 'client_credentials'})
)
response = con.getresponse()
if response.status / 100 != 2:
    raise Exception("oauth token url returned %s" % response.status)

access_token = json.loads(response.read())['access_token']
con.close()
