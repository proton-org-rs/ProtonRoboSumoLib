# Proton Sumo Biblioteka – Uputstvo za učesnike

## Uvod

Proton Sumo biblioteka je razvijena za Robo Sumo takmičenje kako bi učesnicima omogućila da se fokusiraju isključivo na **strategiju i taktiku robota**, dok biblioteka upravlja svim hardverskim detaljima.

Biblioteka automatski kontroliše:

* DC motore (DRV8833 driver)
* IR prijemnik (izbor taktike, START/STOP)
* Ultrazvučni senzor (HC-SR04)
* IR senzor za ivicu arene

---

## Osnovni koncept

Učesnici ne programiraju lowlevel funkcije robota, već definišu **do 8 taktika**, koje robot izvršava nakon izbora preko IR daljinskog upravljača.

Tok rada:

1. IR daljinskim se bira taktika (1–8)
2. Šalje se START signal
3. Robot izvršava izabranu taktiku
4. STOP signal zaustavlja robota

---

## Struktura programa

Svaki program mora sadržati:

```cpp
#include <ProtonSumo.h>

void setup() {
    Sumo.begin();

    Sumo.registerTactic(1, tactic1);
}

void loop() {
    Sumo.run();
}
```

---

## 1. Inicijalizacija

### `Sumo.begin()`

Inicijalizuje:

* motore
* senzore
* IR prijemnik
* interne promenljive biblioteke

Poziva se jednom u `setup()`.

```cpp
Sumo.begin();
```

---

## 2. Registracija taktika

### `Sumo.registerTactic(slot, function)`

Povezuje taktiku sa brojem 1–8.

| Parametar | Opis                             |
| --------- | -------------------------------- |
| slot      | broj taktike (1–8)               |
| function  | funkcija koja definiše ponašanje |

```cpp
Sumo.registerTactic(1, attack);
Sumo.registerTactic(2, searchMode);
```

---

## 3. Pokretanje sistema

### `Sumo.run()`

Glavna funkcija biblioteke koja mora biti pozivana u svakoj iteraciji `loop()`.

Ona automatski:

* čita IR komande
* bira taktiku
* pokreće taktiku kada stigne START
* zaustavlja robota na STOP

```cpp
void loop() {
    Sumo.run();
}
```

---

## 4. Upravljanje motorima

### `Sumo.setMotors(leftDir, leftPower, rightDir, rightPower)`

Direktna kontrola motora.

| Parametar | Značenje         |
| --------- | ---------------- |
| 'F'       | forward (napred) |
| 'B'       | backward (nazad) |
| 0–100     | snaga motora (%) |

```cpp
Sumo.setMotors('F', 100, 'F', 100);
```
// NAPOMENA: trenutno je u biblioteci minimalna brzina motora 55% od maksimalne brzine motora (ukoliko nije zadat speed 0 ili veci od 56+, speed ce biti 55)
---

### `Sumo.forward(speed)`

Kretanje napred.

```cpp
Sumo.forward(100);
```

---

### `Sumo.backward(speed)`

Kretanje unazad.

```cpp
Sumo.backward(80);
```

---

### `Sumo.turnLeft(speed)`

Rotacija ulevo.

```cpp
Sumo.turnLeft(60);
```

---

### `Sumo.turnRight(speed)`

Rotacija udesno.

```cpp
Sumo.turnRight(60);
```

---

### `Sumo.stop()`

Momentano zaustavlja robota.

```cpp
Sumo.stop();
```

---

## 5. Ultrazvučni senzor

### `Sumo.distance()`

Vraća udaljenost u centimetrima.

* vraća `-1` ako senzor nije aktivan ili nema očitavanja

```cpp
float d = Sumo.distance();
```

---

### `Sumo.opponentDetected(threshold)`

Proverava da li je protivnik u blizini.

Podrazumevano: **30 cm**

```cpp
Sumo.opponentDetected();        // default 30 cm
Sumo.opponentDetected(15);      // custom prag
```

```cpp
if (Sumo.opponentDetected()) {
    Sumo.forward(100);
}
```

---

## 6. Senzor ivice arene

### `Sumo.edgeDetected()`

Vraća `true` ako je detektovana ivica (crna linija).

```cpp
if (Sumo.edgeDetected()) {
    Sumo.escapeEdge();
}
```

---

## 7. Pomoćne funkcije

### `Sumo.search()`

Automatski režim traženja protivnika (rotacija u mestu).

```cpp
Sumo.search();
```

---

### `Sumo.escapeEdge()`

Automatska reakcija kada robot dođe do ivice:

* ide unazad
* rotira se
* nastavlja borbu

```cpp
Sumo.escapeEdge();
```

---

## 8. Upravljanje senzorima

Biblioteka omogućava isključivanje senzora ako nisu fizički povezani.

---

### `Sumo.enableUltrasonic(bool state)`

Uključuje/isključuje ultrazvučni senzor.

```cpp
Sumo.enableUltrasonic(false);
```

Ako je isključen:

* `distance()` vraća `-1`
* `opponentDetected()` vraća `false`

---

### `Sumo.enableEdgeSensor(bool state)`

Uključuje/isključuje IR senzor za ivicu.

```cpp
Sumo.enableEdgeSensor(false);
```

Ako je isključen:

* `edgeDetected()` uvek vraća `false`

---

## 9. Status robota

### `Sumo.isRunning()`

Vraća da li je meč aktivan (START primljen).

```cpp
if (Sumo.isRunning()) {
    // taktika radi
}
```

---

## 10. IR komandni sistem

Biblioteka koristi sledeće IR kodove:

| Komanda   | Kod  |
| --------- | ---- |
| Taktika 1 | 0x01 |
| Taktika 2 | 0x02 |
| Taktika 3 | 0x03 |
| Taktika 4 | 0x04 |
| Taktika 5 | 0x05 |
| Taktika 6 | 0x06 |
| Taktika 7 | 0x07 |
| Taktika 8 | 0x08 |
| START     | 0x09 |
| STOP      | 0x0A |

NIKAKO NE REGISTROVATI TAKTIKE NA 0X09 I 0X0A!!!
---

## 11. Primer kompletne taktike

```cpp
void attack()
{
    if (Sumo.edgeDetected()) {
        Sumo.escapeEdge();
        return;
    }

    if (Sumo.opponentDetected()) {
        Sumo.forward(100);
    } else {
        Sumo.search();
    }
}
```

---

## 12. Preporuke za učesnike

* Izbegavati `delay()` funkcije u taktici
* Uvek proveravati `edgeDetected()` pre napada
* Koristiti `escapeEdge()` obavezno
* Testirati taktike sa različitim konfiguracijama senzora

---
