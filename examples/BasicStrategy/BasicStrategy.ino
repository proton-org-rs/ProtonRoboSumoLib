#include "ProtonSumo.h"
// Potrebno je dodati ProtonSumo.h u libraries folder Arduina
// Potrebno je instalirati IRremote biblioteku

// Ovde se definisu imena taktika kako bi se registrovale u setup-u
void attack();

void setup()
{
    Sumo.begin();

    // mogu se isključiti senzori ako nisu potrebni, npr. ako se zna da protivnik neće biti ispred robota ili da rub staze nije problem:
    // Sumo.enableUltrasonic(false);
    // Sumo.enableEdgeSensor(false);
    Sumo.registerTactic(1, attack);

    // Ukoliko je potrebno da se pregleda nesto na Serial monitoru, obavezno otkometarsati liniju ispod
    // Serial.begin(9600) // Odnosno neki drugi baud rate umesto 9600
}

// U loop se mogu dodati jos neke funkcije radi debugovanja
void loop()
{
    Sumo.run();
    // Serial.println(Sumo.isRunning()) // ili sta god da je potrebno proveravati
}

// Example strategija, moze se modifikovati, ignorisati ili obrisati
void attack()
{
    if (Sumo.edgeDetected())
    {
        Sumo.escapeEdge();
        return;
    }

    if (Sumo.opponentDetected())
        Sumo.forward(100);
    else
        Sumo.search();
}

// Druge strategije pisati ispod i obavezno ih registrovati u setup()