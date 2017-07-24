FROM devhub-docker.cisco.com/iox-docker/base-x86_64
RUN opkg update
RUN opkg install python
RUN opkg install python-pip
RUN pip install oauth
RUN pip install websocket-client

ADD resources/Config.json /usr/share/motionapp/resources/Config.json
ADD oauth.py /usr/share/motionapp/oauth.py
ADD motion_app.py /usr/share/motionapp/motion_app.py
ADD motion_api.py /usr/share/motionapp/motion_api.py
ADD scripts/motionapp.sh /usr/share/motionapp/motionapp.sh
