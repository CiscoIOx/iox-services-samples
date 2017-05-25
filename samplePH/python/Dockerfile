FROM devhub-docker.cisco.com/iox-docker/base-x86_64
RUN opkg update
RUN opkg install python
RUN opkg install python-pip
RUN pip install oauth
RUN pip install websocket-client
ADD scripts/oauth.py /usr/share/gps/oauth.py
ADD scripts/nbi_subscribe.py /usr/share/gps/nbi_subscribe.py
ADD scripts/gpsapp.sh /usr/share/gps/gpsapp.sh
