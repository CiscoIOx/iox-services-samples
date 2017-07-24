#!/bin/sh

if [ -f /data/.env ]
then
	source /data/.env
fi

start_app() {
	cd /usr/share/motionapp
	python motion_app.py 2> /data/logs/motionapp.log
}

_term() {
  echo "Caught SIGTERM signal!"
  kill -TERM "$pid" 2>/dev/null
}                              
                               
trap _term SIGTERM

case "$1" in
start)
        echo -n "Starting app motionapp"
        start_app
        echo "done."
        ;;

stop)
        echo -n "Stopping app motionapp"
        pidof python | xargs kill -15
        : exit 0
        echo "done."
        ;;

force-reload|restart)
        echo "Reconfiguring motionapp"
        echo "done."
        ;;

*)
        echo "Usage: /etc/init.d/motionapp {start|stop}"
        exit 1
        ;;
esac

exit 0
