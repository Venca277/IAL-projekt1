/*
 *  Předmět: Algoritmy (IAL) - FIT VUT v Brně
 *  Rozšíření pro příklad c206.c (Dvousměrně vázaný lineární seznam)
 *  Vytvořil: Daniel Dolejška, září 2024
 */

#include "c206-ext.h"

bool error_flag;
bool solved;

/**
 * Tato metoda simuluje příjem síťových paketů s určenou úrovní priority.
 * Přijaté pakety jsou zařazeny do odpovídajících front dle jejich priorit.
 * "Fronty" jsou v tomto cvičení reprezentovány dvousměrně vázanými seznamy
 * - ty totiž umožňují snazší úpravy pro již zařazené položky.
 *
 * Parametr `packetLists` obsahuje jednotlivé seznamy paketů
 * (`QosPacketListPtr`). Pokud fronta s odpovídající prioritou neexistuje, tato
 * metoda ji alokuje a inicializuje. Za jejich korektní uvolnení odpovídá
 * volající.
 *
 * V případě, že by po zařazení paketu do seznamu počet prvků v cílovém seznamu
 * překročil stanovený MAX_PACKET_COUNT, dojde nejdříve k promazání položek
 * seznamu. V takovémto případě bude každá druhá položka ze seznamu zahozena
 * nehledě na její vlastní prioritu ovšem v pořadí přijetí.
 *
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného
 * seznamu
 * @param packet Ukazatel na strukturu přijatého paketu
 */
void receive_packet(DLList *packetLists, PacketPtr packet) {

  QosPacketListPtr curr_qos; // aktuální list packetů
  DLL_First(packetLists);    // posunutí ukazatele na první list packetů
  int located = 0; // pomocná proměnná pro nalezení správné priority

  //----------------------------------------------------------------
  // Hledání správného listu podle priority
  //----------------------------------------------------------------

  while (packetLists->activeElement != NULL) {
    curr_qos = (QosPacketListPtr)packetLists->activeElement->data;

    // pokud se rovná byl nalezen list packetů
    if (curr_qos->priority == packet->priority) {
      located = 1;
      break;
    }
    DLL_Next(packetLists);
  }

  //---------------------------------------------------------------
  // Tvorba nového listu pro prioritu packetu (pokud neexistuje)
  //---------------------------------------------------------------

  if (!located) {
    DLList *new_list = malloc(sizeof(DLList));
    QosPacketList *new_packet_list = malloc(sizeof(QosPacketList));
    // kontrola alokace
    if (new_list == NULL || new_packet_list == NULL) {
      printf("Allocation failed!");
      return;
    }
    DLL_Init(new_list);
    // inicializace nového listu pro paket
    new_packet_list->priority = packet->priority;
    new_packet_list->list = new_list;
    DLL_InsertLast(packetLists, (long)new_packet_list); // vložení k ostatním
    curr_qos = (QosPacketListPtr)packetLists->lastElement->data; // nový list
  }

  //--------------------------------------------------------------
  // Vkládání packetů do jeho listu
  //--------------------------------------------------------------

  if (curr_qos->list->currentLength < MAX_PACKET_COUNT) {
    DLL_InsertLast(curr_qos->list, (long)packet); // vkládá do MAX_PACKET_COUNT
  } else { // jinak od začátku zahodí každý druhý packet
    DLL_First(curr_qos->list);
    while (curr_qos->list->activeElement != NULL) {
      DLL_DeleteAfter(curr_qos->list);
      DLL_Next(curr_qos->list);
    }
    DLL_InsertLast(curr_qos->list, (long)packet); // vloží packet co se nevešel
  }
}

/**
 * Tato metoda simuluje výběr síťových paketů k odeslání. Výběr respektuje
 * relativní priority paketů mezi sebou, kde pakety s nejvyšší prioritou
 * jsou vždy odeslány nejdříve. Odesílání dále respektuje pořadí, ve kterém
 * byly pakety přijaty metodou `receive_packet`.
 *
 * Odeslané pakety jsou ze zdrojového seznamu při odeslání odstraněny.
 *
 * Parametr `packetLists` obsahuje ukazatele na jednotlivé seznamy paketů
 * (`QosPacketListPtr`). Parametr `outputPacketList` obsahuje ukazatele na
 * odeslané pakety (`PacketPtr`).
 *
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného
 * seznamu
 * @param outputPacketList Ukazatel na seznam paketů k odeslání
 * @param maxPacketCount Maximální počet paketů k odeslání
 */
void send_packets(DLList *packetLists, DLList *outputPacketList,
                  int maxPacketCount) {

  long swapp_place;       // pomocná proměnná pro prohození dvou priorit
  DLL_First(packetLists); // aktvní prvek na první prvek (první list packetů)
  QosPacketListPtr active;      // první list packetů
  QosPacketListPtr active_next; // další list packetů (hned vedle)

  //-----------------------------------------------------------------------------
  // Řazení packet listů
  //-----------------------------------------------------------------------------

  int swapp = 1;
  //řadící algoritmus(bubble sort), bude řadit pro listy s více než jednou
  // prioritou
  while (swapp && (packetLists->currentLength > 1)) {
    swapp = 0;
    int i = 1;

    while (i <= packetLists->currentLength) {

      // zajištění cyklicity listu(aby bylo vždy s čím porovnávat)
      if (packetLists->activeElement->nextElement == NULL) {
        DLL_First(packetLists);
      }

      // inicializace prvků pro porovnání
      active = (QosPacketListPtr)packetLists->activeElement->data;
      active_next =
          (QosPacketListPtr)packetLists->activeElement->nextElement->data;

      // prohození ukazatelů(longů) na list packetů, řazení je sestupné
      if (active->priority < active_next->priority) {
        swapp_place = packetLists->activeElement->nextElement->data;
        packetLists->activeElement->nextElement->data =
            packetLists->activeElement->data;
        packetLists->activeElement->data = swapp_place;
        swapp = 1; // byl proveden prohoz
      }

      DLL_Next(packetLists);
      i++;
    }
  }

  //-----------------------------------------------------------------------------
  // Odesílaní packetů
  //-----------------------------------------------------------------------------

  // příprava odesílání v seřazeném listu packetů, nejvyšší priorita
  DLL_First(packetLists);
  QosPacketListPtr curr_qos =
      (QosPacketListPtr)packetLists->activeElement
          ->data; // nahrání prvního listu packetů s nejvyšší prioritou
  DLL_First(curr_qos->list); // první jde ven nejstarší packet
  int packets_out = 0;       //čítač odeslaných packetů

  // odesílá packety dokud nebyl odeslán požadovaný počet
  while (packets_out != maxPacketCount) {

    // kontrola prázdnosti listu a přepnutí na další prioritu
    if (curr_qos->list->activeElement == NULL) {
      DLL_Next(packetLists);
      // není-li dostupný další list packetů, odesílání se ukončí
      if (packetLists->activeElement == NULL) {
        break;
      }
      // přepnutí na další list packetů
      curr_qos = (QosPacketListPtr)packetLists->activeElement->data;
      DLL_First(curr_qos->list);
    }

    // odesílání packetů do outputPacketList a inkrementace čítače packetů
    DLL_InsertLast(outputPacketList, curr_qos->list->activeElement->data);
    packets_out++;
    // kontrola posledního packetu v listu
    if (curr_qos->list->currentLength == 1) {
      DLL_DeleteFirst(curr_qos->list);
      continue;
    }
    // zahození packetu a posunutí ukazatele na další packet
    DLL_Next(curr_qos->list);
    DLL_DeleteBefore(curr_qos->list);
  }
}
