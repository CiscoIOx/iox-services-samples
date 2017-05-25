#!/bin/sh

export PATH=$PATH:/usr/share/cservice/rpc/bin
export LD_LIBRARY_PATH=/usr/share/cservice/rpc/libs:$LD_LIBRARY_PATH

if [ -f /data/.env ]
then
	source /data/.env
fi

start_rpc() {

	if [ ! -d /data/cservice/rpc ]
	then
	    mkdir -p /data/cservice/rpc
	fi


	cp /usr/share/cservice/rpc/config/RPC_schema1.txt /data/cservice/rpc/
	cp /usr/share/cservice/rpc/config/RPC_schema2.txt /data/cservice/rpc/
	cp /usr/share/cservice/rpc/config/RPC_schema3.txt /data/cservice/rpc/

	cd /data/cservice/rpc;

	rpc_handler > /dev/null 2>&1 &
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
        echo -n "Starting RPC App...."
        start_rpc
        echo "done."
        ;;

stop)
        echo -n "Stopping RPC App..."
        pidof rpc_handler | xargs kill -15
        : exit 0
        echo "done."
        ;;

force-reload|restart)
        echo "Reconfiguring rpcApp"
        echo "done."
        ;;

*)
        echo "Usage: /etc/init.d/rpcservice {start|stop}"
        exit 1
        ;;
esac

exit 0
