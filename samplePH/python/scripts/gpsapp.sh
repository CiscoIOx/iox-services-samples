#!/bin/sh

if [ -f /data/.env ]
then
	source /data/.env
fi

start_gps() {

	cd /usr/share/gps
	python nbi_subscribe.py
}

_term() {
  echo "Caught SIGTERM signal!"
  kill -TERM "$pid" 2>/dev/null
}                              
                               
trap _term SIGTERM

case "$1" in
start)
        echo -n "Starting gps App...."
        start_gps
        echo "done."
        ;;

stop)
        echo -n "Stopping gps App..."
        pidof gps_handler | xargs kill -15
        : exit 0
        echo "done."
        ;;

force-reload|restart)
        echo "Reconfiguring gpsApp"
        echo "done."
        ;;

*)
        echo "Usage: /etc/init.d/gpsapp {start|stop}"
        exit 1
        ;;
esac

exit 0
