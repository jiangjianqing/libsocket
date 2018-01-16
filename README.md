
##UdpBroadcaster 问题
linux下向255.255.255.255 发送udp广播受限，输入以下命令能够解决(eth0为网卡地址)
route add -host 255.255.255.255 dev eth0
