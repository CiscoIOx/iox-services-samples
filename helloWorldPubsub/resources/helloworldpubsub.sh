#!/bin/sh

export PATH=$PATH:/usr/share/cservice/hello_pubsub/bin
export LD_LIBRARY_PATH=/usr/lib:/usr/share/cservice/hello_pubsub/libs

if [ -f /data/.env ]
then
	source /data/.env
fi

start_pubsub() {

	if [ ! -d /data/cservice/hello_pubsub ]
	then
	    mkdir -p /data/cservice/hello_pubsub
	fi

	if [ ! -d /data/cservice/hello_pubsub/storage ]
	then
	    mkdir -p /data/cservice/hello_pubsub/storage
	fi

	if [ -f /data/package_config.ini ]
	then
		cp /data/package_config.ini /data/cservice/hello_pubsub/package_config.ini
	fi

	cd /data/cservice/hello_pubsub;

	hello_world_pubsub > /dev/null 2>&1 &

	pid=$!                        
	echo -n "Wating for the process $pid"
	wait $pid
}

_term() {
  echo "Hello PubSub Caught SIGTERM signal!"
  kill -TERM "$pid" 2>/dev/null
}                              
                               
trap _term SIGTERM

case "$1" in
start)
        echo -n "Starting gps App...."
        start_pubsub
        echo "done."
        ;;

stop)
        echo -n "Stopping gps App..."
        pidof hello_world_pubsub | xargs kill -15
        : exit 0
        echo "done."
        ;;

force-reload|restart)
        echo "Reconfiguring pubsub app"
        echo "done."
        ;;

*)
        echo "Usage: /usr/share/cservice/hello_pubsub/bin/hello_pubsub_handler {start|stop}"
        exit 1
        ;;
esac

exit 0
