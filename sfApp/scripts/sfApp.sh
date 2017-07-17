#!/bin/sh

if [ -f /data/.env ]
then
	source /data/.env
fi

LOG="/data/logs/sfApp.log"
touch $LOG

start_app() {
	cd /usr/share/sfApp
	python sfApp.py  >> $LOG 2>&1
}

stop_app() {
	pidof python | xargs kill -15
}

_term() {
  echo "Caught SIGTERM signal!"
  kill -TERM "$pid" 2>/dev/null
}                              
                               
trap _term SIGTERM

case "$1" in
start)
        echo -n "Starting app sfApp" >> $LOG
        start_app
        echo "done." >> $LOG
        ;;

stop)
        echo -n "Stopping app sfApp" >> $LOG
        stop_app
        : exit 0
        echo "done." >> $LOG
        ;;

force-reload|restart)
        echo "Reconfiguring sfApp" >> $LOG
		stop_app
		start_app
        echo "done." >> $LOG
        ;;

*)
        echo "Usage: /etc/init.d/sfApp.sh {start|stop}" 
        exit 1
        ;;
esac

exit 0

