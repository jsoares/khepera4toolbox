#!/bin/sh

rm -rf data
mkdir data

echo "\n1) Start Swistrack and open the swistrack/config/config.swistrack file."
echo "   Make sure that the mask points to  swistrack/config/arena_mask.bmp and"
echo "   that the TSAI calibration points to swistrack/config/calibration_points.xml."
echo "   You can start to track using Swistrack (Run button).\n"
echo -n "   Press [ENTER] when you are done."
read enter

echo "\n2) All robots should accept incoming SSH connections from this computer"
echo "   without the need to input a password (use k3-ssh-keysetup)."
echo -n "   Which robots do you have (e.g. 202 204 206 214): "
read robots

echo "\n3) Do you want this script to send the relativepositionboard_full_calibration"
echo -n "   executable to your robots? [y/n] "
read answer
case $answer in
    y|yes) 
        for r in $robots; do
            echo "   Uploading file onto robot $r"
            scp ../relativepositionboard_full_calibration root@192.168.141.$r:~
        done
        ;;
    *) echo "   We can then safely assume that those executables are on the robots ($robots).";;
esac

echo -n "\n4) On which host is Swistrack running (localhost)? "
read host
if [ -z $host ]; then
    host="localhost"
    echo "   Swistrack runs on $host then."
fi

echo -n "\n5) On which port is Swistrack sending data (3001)? "
read port
if [ -z $port ]; then
    port="3001"
    echo "   Swistrack uses port $port then."
fi

echo -n "\n6) Starting the tracking script...\t"
c="netcat $host $port | perl ./swistrack/swistrack-filter.pl $robots > data/positions.txt";
xterm -T "Swistrack" -e "$c; echo \"[ENTER] to close\"; read" &
echo "[ OK ]"

echo -n "\n6) Starting the rssi observers...\t"
id=0
for r in $robots; do
    rport=`dc -e "$id 4000 + p"`
    c="echo \"Listening on port $rport\"; netcat -l -u $rport | perl robot/robot-filter.pl > data/rssi_robot_$id.txt";
    xterm -T "RSSI observer for robot $r" -e "$c; echo \"[ENTER] to close\"; read" &
    id=`dc -e "$id 1 + p"`
done
echo "[ OK ]"

pc=`ifconfig  | grep 'inet addr:192.168.141.' | cut -d. -f4 | awk '{ print $1}'`
echo -n "\n7) Starting the calibration script on the robots (local IP address: 192.168.141.$pc)...\t"
id=0
for r in $robots; do
    sleep 1
    c="ssh -t root@192.168.141.$r ./relativepositionboard_full_calibration --id=$id --pc=$pc"
    xterm -T "Robot $r" -e "echo \"Press [CTRL + C] to stop the program.\"; $c; echo \"[ENTER] to close\"; read" &
    id=`dc -e "$id 1 + p"`
done
echo "[ OK ]"



