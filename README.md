# ISA Projekt
## Tunelovanie dátových prenosov cez DNS dotazy

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

