#!/bin/bash

CAMERA_HOST="turrdet-camera.local"
BACKUP_IP="10.54.31.20"
MAX_TEST=20
CURRENT_TEST=0

resolve_host() {
	avahi-resolve -4 --name $1 | sed 's/\|/ /' | awk '{print $2}'
}

ping_test() {
	ping -c 1 $1 -W 1 > /dev/null && echo 1 || echo 0
}

while [ 1 ] ; do
		RESOLVE_TEST=$(resolve_host $CAMERA_HOST)
		PINGRESULT=$(ping_test $RESOLVE_TEST)
		if [ $PINGRESULT -eq 1 ]; then
			echo "$RESOLVE_TEST"
			break
		fi
		
		# If passed max tests try the backup ip
		if [ $CURRENT_TEST -gt $MAX_TEST ]; then
			PINGRESULT="$(ping_test $BACKUP_IP)"
			if [ $PINGRESULT -ne 1 ]; then
				echo "NA"
				break
			fi
		fi
		
		CURRENT_TEST=$((CURRENT_TEST + 1))
		# Wait a second
		sleep 1
done
