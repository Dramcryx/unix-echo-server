# unix-echo-server
Хотелось бы ещё дописать обработку параметров командной строки и чтобы не локалхост, позже сделаю, пока так.
**Сборка:**
1. север:
```
g++ server.cpp -pthread -o server
```
2. клиенты:
```
g++ tcp-client.cpp -o tcp
g++ udp-client.coo -o udp
```
