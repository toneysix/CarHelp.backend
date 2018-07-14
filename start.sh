#! /bin/bash

while :
do
    	usleep 1
	/mydev/backend/carhelp.backend/bin/Release/CarHelpserver 1 || break
done
