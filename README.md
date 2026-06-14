# Proton Sumo Biblioteka – Uputstvo za učesnike

## Uvod

Proton Sumo biblioteka je razvijena za Robo Sumo takmičenje kako bi učesnicima omogućila da se fokusiraju isključivo na **strategiju i taktiku robota**, dok biblioteka upravlja svim hardverskim detaljima.

Biblioteka automatski kontroliše:

* DC motore (DRV8833 driver)
* IR prijemnik (izbor taktike, START/STOP)
* Ultrazvučni senzor (HC-SR04)
* Leve i desne IR senzore za detekciju ivice arene

---

## Osnovni koncept

Učesnici ne programiraju low-level funkcije robota, već definišu **do 8 taktika**, koje robot izvršava nakon izbora preko IR daljinskog upravljača.

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
Sumo.opponentDetected();
Sumo.opponentDetected(15);
```

```cpp
if (Sumo.opponentDetected()) {
    Sumo.forward(100);
}
```

---

## 6. Senzori ivice arene

Robot koristi dva IR senzora za detekciju ivice borilišta.

| Senzor                 | Pin |
| ---------------------- | --- |
| Levi IR Line Follower  | A0  |
| Desni IR Line Follower | A1  |

---

### `Sumo.edgeDetected()`

Vraća `true` ukoliko je bilo koji od senzora detektovao ivicu arene.

```cpp
if (Sumo.edgeDetected()) {
    Sumo.escapeEdge();
}
```

---

### `Sumo.leftEdgeDetected()`

Vraća `true` ukoliko je levi senzor detektovao ivicu.

```cpp
if (Sumo.leftEdgeDetected()) {
    // leva strana robota je na ivici
}
```

---

### `Sumo.rightEdgeDetected()`

Vraća `true` ukoliko je desni senzor detektovao ivicu.

```cpp
if (Sumo.rightEdgeDetected()) {
    // desna strana robota je na ivici
}
```

---

## 7. Pomoćne funkcije

### `Sumo.search()`

Automatski režim traženja protivnika.

Podrazumevano ponašanje je rotacija u mestu radi pretrage arene.

```cpp
Sumo.search();
```

---

### `Sumo.escapeEdge()`

Automatska reakcija na detekciju ivice arene.

Biblioteka koristi informacije sa oba senzora kako bi izabrala odgovarajući manevar:

| Detekcija    | Reakcija                           |
| ------------ | ---------------------------------- |
| Levi senzor  | Kretanje unazad i skretanje udesno |
| Desni senzor | Kretanje unazad i skretanje ulevo  |
| Oba senzora  | Povlačenje unazad od ivice         |

Primer korišćenja:

```cpp
if (Sumo.edgeDetected()) {
    Sumo.escapeEdge();
    return;
}
```

---

## 8. Upravljanje senzorima

Biblioteka omogućava isključivanje senzora koji nisu povezani na robot.

---

### `Sumo.enableUltrasonic(bool state)`

Uključuje ili isključuje ultrazvučni senzor.

```cpp
Sumo.enableUltrasonic(false);
```

Ako je isključen:

* `distance()` vraća `-1`
* `opponentDetected()` vraća `false`

---

### `Sumo.enableEdgeSensor(bool state)`

Uključuje ili isključuje senzore za detekciju ivice arene.

```cpp
Sumo.enableEdgeSensor(false);
```

Ako su isključeni:

* `edgeDetected()` uvek vraća `false`
* `leftEdgeDetected()` uvek vraća `false`
* `rightEdgeDetected()` uvek vraća `false`

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

**NIKAKO NE REGISTROVATI TAKTIKE NA 0x09 I 0x0A.**

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

* Izbegavati korišćenje `delay()` funkcije unutar taktika
* Uvek proveravati `edgeDetected()` pre logike za napad
* Koristiti `escapeEdge()` nakon detekcije ivice
* Koristiti `leftEdgeDetected()` i `rightEdgeDetected()` samo kada je potrebno implementirati naprednije taktike
* Testirati taktike sa različitim konfiguracijama senzora i protivnika
* U slučaju da određeni senzor nije povezan, koristiti odgovarajuću funkciju za njegovo isključivanje
