#/bin/bash
# 2016-09-08 - R777
#
# This little client prg will target the Arduino Leonardo WWW server
# Prerequisites:
#    0. of course, Arduino Leonardo (because that's it what I have)
#    1. proper board selected in Arduino IDE
#    2. EtherShield v3 hardware mounted on top of the board
#    3. Code loaded from File-> EXAMPLES -> Ethernet -> Webserver
#    4. Code modified to reflect proper static IP and MAC
#    5. A running local network (either a cable or infra)
#   (X. no Micro$oft running, albeit W10 now includes bash)

# Most likely you want to use Leonardo console for debugging:
# Tools -> Serial port (it is reasonable to raise the speed to 115k)

target_ip="192.168.2.166";
# change the target ip to suit your own network!
# 
agent_base="Wget_seqno=";

i=$((0 + 1)) ; # to check the arithmetics early
while true
do 
#    clear; # uncomment for sleep times over 1 sec
    echo "=====${i}";
#    echo;
    cua="${agent_base}${i}"; # Custom User Agent
    wget -U ${cua} -qO - ${target_ip};
#     sleep 0.1;
    i=$((i + 1));
done

#
