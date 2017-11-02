FROM gliderlabs/alpine:3.3
RUN apk add --update \
    python \
    py-pip \
  && rm -rf /var/cache/apk/*
RUN pip install --upgrade pip
RUN pip install websocket-client
RUN pip install oauth
RUN pip install requests==2.10.0

ADD resources/GPS_Config.json /usr/share/gpsapp/resources/GPS_Config.json
ADD oauth.py /usr/share/gpsapp/oauth.py
ADD gps_app.py /usr/share/gpsapp/gps_app.py
ADD gps_api.py /usr/share/gpsapp/gps_api.py
ADD scripts/gpsapp.sh /usr/share/gpsapp/gpsapp.sh
