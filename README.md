
##UdpBroadcaster 问题
linux下向255.255.255.255 发送udp广播受限，输入以下命令能够解决(eth0为网卡地址)
route add -host 255.255.255.255 dev eth0

Windows下安装了vbox后，udp广播出现问题，从host向外发送udp广播消息失败
解决办法：在windows网卡中禁用VBOX虚拟网卡：VirtualBox Host-Only Ethernet Adapter