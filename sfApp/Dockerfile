FROM devhub-docker.cisco.com/iox-docker/base-x86_64
RUN opkg update
RUN opkg install python
RUN opkg install python-pip
RUN pip install oauth

ADD resources/sf_mytopic_schema.json /usr/share/sfApp/resources/sf_mytopic_schema.json
ADD resources/sf_mytopic_schema_no_usb.json /usr/share/sfApp/resources/sf_mytopic_schema_no_usb.json
ADD oauth.py /usr/share/sfApp/oauth.py
ADD sfApp.py /usr/share/sfApp/sfApp.py
ADD sf_config.py /usr/share/sfApp/sf_config.py
ADD scripts/sfApp.sh /usr/share/sfApp/sfApp.sh

