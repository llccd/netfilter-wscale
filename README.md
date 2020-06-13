# xt_wscale

A kernel module/iptables extension for matching wscale in TCP option  

Usage
=====

```
iptables -t mangle -A POSTROUTING -p tcp --dport 443 --tcp-flags SYN,RST SYN -m wscale --lt 10 -j CONNMARK --set-mark 1
```
This will set ctmark to 1 for connections with wscale less than 10.
