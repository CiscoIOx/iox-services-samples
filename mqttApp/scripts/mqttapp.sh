#!/bin/sh

if [ -f /data/.env ]
then
	source /data/.env
fi

start_app() {
	cd /usr/share/mqttapp
	python mqtt_app.py
}

_term() {
  echo "Caught SIGTERM signal!"
  kill -TERM "$pid" 2>/dev/null
}                              
                               
trap _term SIGTERM

case "$1" in
start)
        echo -n "Starting app mqttapp"
        start_app
        echo "done."
        ;;

stop)
        echo -n "Stopping app mqttapp"
        pidof python | xargs kill -15
        : exit 0
        echo "done."
        ;;

force-reload|restart)
        echo "Reconfiguring phapp"
        echo "done."
        ;;

*)
        echo "Usage: /etc/init.d/mqttapp {start|stop}"
        exit 1
        ;;
esac

exit 0
