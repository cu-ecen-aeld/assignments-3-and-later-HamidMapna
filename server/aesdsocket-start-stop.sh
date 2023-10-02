#! /bin/sh
#DAEMON_OPTS = "-d"
case "$1" in
    start)
        echo "starting aesd_server program"
        start-stop-daemon -S -n aesdsocket -a /usr/bin/aesdsocket
        ;;
    stop)
        echo "stopping aesd_server program"
        start-stop-daemon -K -n aesdsocket 
        ;;
    *)
        echo "Usage: $0 {start|stop}"
    exit 1
esac

exit 0    

