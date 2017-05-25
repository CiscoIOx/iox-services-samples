#!/bin/sh

export PATH=$PATH:/usr/share/cservice/gps/bin
export LD_LIBRARY_PATH=/usr/share/cservice/gps/libs:$LD_LIBRARY_PATH

if [ -f /data/.env ]
then
	source /data/.env
fi

start_gps() {

	if [ ! -d /data/cservice/gps ]
	then
	    mkdir -p /data/cservice/gps
	fi

	cp /usr/share/cservice/gps/config/service.raml /data/cservice/gps/service.raml

	cp /usr/share/cservice/gps/config/GPS_dataset.txt /data/cservice/gps/
	cp /usr/share/cservice/gps/config/GPS_schema.txt /data/cservice/gps/

	cd /data/cservice/gps;

	gps_handler > /dev/null 2>&1 &
	pid=$!                        
	echo -n "Wating for the process $pid"
	wait $pid
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
        echo "Usage: /etc/init.d/gpsservice {start|stop}"
        exit 1
        ;;
esac

exit 0
