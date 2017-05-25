FROM devhub-docker.cisco.com/iox-docker/base-x86_64
RUN opkg update
RUN opkg install python
RUN opkg install python-pip
RUN pip install oauth
RUN pip install websocket-client

ADD resources/mqtt_policy.json /usr/share/mqttapp/resources/mqtt_policy.json
ADD oauth.py /usr/share/mqttapp/oauth.py
ADD mqtt_app.py /usr/share/mqttapp/mqtt_app.py
ADD mqtt_api.py /usr/share/mqttapp/mqtt_api.py
ADD scripts/mqttapp.sh /usr/share/mqttapp/mqttapp.sh
