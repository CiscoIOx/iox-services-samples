FROM devhub-docker.cisco.com/iox-docker/base-x86_64
RUN opkg update
RUN opkg install python
RUN opkg install python-pip
RUN pip install oauth
RUN pip install websocket-client

ADD resources/GPS_Config.json /usr/share/gpsapp/resources/GPS_Config.json
ADD oauth.py /usr/share/gpsapp/oauth.py
ADD gps_app.py /usr/share/gpsapp/gps_app.py
ADD gps_api.py /usr/share/gpsapp/gps_api.py
ADD scripts/gpsapp.sh /usr/share/gpsapp/gpsapp.sh
