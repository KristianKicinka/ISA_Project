# ISA Projekt
## Tunelovanie dátových prenosov cez DNS dotazy
#
Autor : Kristián Kičinka (xkicin02)<br>
Dátum vytvorenia : 7.11.2022
#
## Build
Pred prvým spustením programu je nutné prejsť do adresára, kde sú cieľové skripty uložené a 
za pomoci príkazu make preložiť skripty.

Preloženie oboch modulov (sender a receiver)
```bash
make 
```
Preloženie modulu sender
```bash
make sender
```
Preloženie modulu receiver
```bash
make receiver
```

## Spustenie programu

Pri spustení modulu sender využívame nasledujúci príkaz

```bash
./dns_sender [-u UPSTREAM_DNS_IP] {BASE_HOST} {DST_FILEPATH} [SRC_FILEPATH]
```
Po spustení sa spustí odosielanie dát zo súboru

Pri spustení modulu receiver využívame nasledujúci príkaz

```bash
./dns_receiver {BASE_HOST} {DST_DIRPATH}
```
Po spustení sa spustí receiver, ktorý zachytáva prichádzajúcu komunikáciu.

## Príklad použitia
Príklad spustenia DNS senderu, ktorý odosiela dáta na DNS server s IP adresou ```127.0.0.1```, base host je ```example.com```.
Dáta sa uložia na serveri do súboru data.txt a ako vstupný súbor dát senderu je zvolaný súbor ```data.txt```:

```bash
./dns_sender -u 127.0.0.1 example.com data.txt ./data.txt
```

Príklad spustenia DNS receiveru, ako base host je nastavený ```example.com``` a root zložka je ```/data```
```bash
./dns_receiver example.com ./data 
```

## Popis programu

### Modul DNS Receiver
Po spustení modulu sa vytvorí a nastaví soket, bindne sa na port 53 a zachytáva DNS komunikáciu. 
Po zachytení paketu dochádza k spracovaniu paketu. Získa sa base host a porovná sa s basehostom zadaným ako parameter modulu. Ak sa base host zhoduje, následne sa pristupuje k dekódovaniu dát z paketu. Ak ide o init paket, dáta obsahujú cestu k súboru do ktorého majú byť dáta uložené. Pri data pakete paket nesie samotné dáta, ktoré sa následne uložia do súboru.

### Modul DNS Sender
Po spustení modulu sa vatvorí a nastaví soket, následne sa vytvorí a inicializuje DNS hlavička. Hlavička sa vloží do DNS paketu reprezentovaného bufferom. Ako prvý sa posiela init paket, obsahujúci cestu k súboru. Po odoslaní akéhokoľvek paketu sa čaká na response od receivera. Ak dorazila odpoveď na init prechádza sa k čítaniu dát zo vstupu. Pred tým ako sa dáta posielajú je nutné ich rozdeliť, zakódovať a vytvoriť DNS query. To platí aj v prípade init paketu. Ak je vstupný súbor odoslaný a spracovaný sender vysiela end packet, ktorý signalizuje recieveru, že prenos dát je dokončený. Po odoslaní end paketu sender ukončuje svoju činnosť.

## Zoznam odovzdávaných súborov
- Sender
  - dns_sender.h
  - dns_sender.c
  - dns_sender_events.h
  - dns_sender_events.c
- Reciever
  - dns_receiver.h
  - dns_receiver.c
  - dns_receiver_events.h
  - dns_receiver_events.c
- base32.h
- base32.c
- connection.h
- connection.c
- error.h
- error.c
- program_arguments.h
- program_arguments.c
- README.md
- Makefile



