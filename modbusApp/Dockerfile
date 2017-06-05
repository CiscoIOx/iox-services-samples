FROM devhub-docker.cisco.com/iox-docker/base-x86_64
RUN opkg update
RUN opkg install python
RUN opkg install python-pip
RUN pip install oauth
RUN pip install websocket-client

ADD resources/data_schema1.json /usr/share/phapp/resources/data_schema1.json
ADD resources/data_schema2.json /usr/share/phapp/resources/data_schema2.json
ADD resources/data_schema3.json /usr/share/phapp/resources/data_schema3.json
ADD resources/device1.json /usr/share/phapp/resources/device1.json
ADD resources/device.json /usr/share/phapp/resources/device.json
ADD resources/device_type1.json /usr/share/phapp/resources/device_type1.json
ADD resources/device_type.json /usr/share/phapp/resources/device_type.json
ADD oauth.py /usr/share/phapp/oauth.py
ADD ph_app.py /usr/share/phapp/ph_app.py
ADD ph_dm_config.py /usr/share/phapp/ph_dm_config.py
ADD scripts/phapp.sh /usr/share/phapp/phapp.sh
