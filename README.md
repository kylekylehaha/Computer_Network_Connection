# Computer_Network_Connection
NCKU CS Computer_Network_Connection

## LAB1 
create socket

HOW to use??
step1: open server
step2: open client to send messange to server

```
	server: ./exe protocol(tcp or udp) recv IP portnuober
	client: ./exe protocol(tcp or udp) send IP portnumber filename
```

## LAB2
create multicast and multithread socket

*	multicast:
	*	內建IP : 127.0.0.1
	*	portnumber is 5678
	*	multicastIP is 226.1.1.1
	*	how to use:
		*	先開client 端等候 在開server端傳送資料到group


```
		server: ./exe 要傳送的檔名
		client: ./exe
```

*	multithread:
	*	兩端用tcp連線
	*	內建IP 127.0.0.1
	*	portnumber=5678
	*	how to use:
		*	先開server端等待client 建立連線
		*	server端不會關閉,除非自己去關閉 因為要一直開著等待連線

```
		server:./exe 要傳送的檔名
		client:./exe
```	

