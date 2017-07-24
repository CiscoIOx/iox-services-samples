FROM devhub-docker.cisco.com/iox-docker/base-x86_64
RUN opkg update
RUN opkg install python
RUN opkg install python-pip
RUN pip install oauth
RUN pip install websocket-client

ADD payload.json /usr/share/motionapp/resources/payload.json
ADD main.py /usr/share/motionapp/main.py
ADD oauth.py /usr/share/motionapp/oauth.py
ADD scripts/motion.sh /usr/share/motionapp/motion.sh
