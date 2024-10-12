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
  QosPacketListPtr curr_qos;
  int located = 0;

  DLL_First(packetLists);
  while (packetLists->activeElement != NULL) {
    curr_qos = (QosPacketListPtr)packetLists->activeElement->data;
    if (curr_qos->priority == packet->priority) {
      located = 1;
      break;
    }
    DLL_Next(packetLists);
  }

  if (located) {

    if (curr_qos->list->currentLength < MAX_PACKET_COUNT) {
      DLL_InsertLast(curr_qos->list, (long)packet);
    } else {
      DLL_First(curr_qos->list);
      for (int i = 1; i <= curr_qos->list->currentLength; i++) {
        if (i % 2) {
          if (curr_qos->list->activeElement->nextElement != NULL) {
            DLL_Next(curr_qos->list);
          }
        } else if (curr_qos->list->activeElement->nextElement != NULL) {
          DLL_Next(curr_qos->list);
          DLL_DeleteBefore(curr_qos->list);
        }
      }
    }

  } else {
    DLList *new_list;
    QosPacketList *new_packet_list;
    new_list = malloc(sizeof(DLList));
    new_packet_list = malloc(sizeof(QosPacketList));
    if (new_list == NULL || new_packet_list == NULL) {
      printf("Allocation failed!");
      return;
    }
    DLL_Init(new_list);

    new_packet_list->priority = packet->priority;
    new_packet_list->list = new_list;
    DLL_InsertLast(packetLists, (long)new_packet_list);

    curr_qos = (QosPacketListPtr)packetLists->lastElement->data;
    DLL_InsertLast(curr_qos->list, (long)packet);
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
  int swapp = 1;
  long swapp_place;
  QosPacketListPtr active;
  QosPacketListPtr active_next;
  DLL_First(packetLists);

  while (swapp) {
    swapp = 0;
    int i = 1;

    while (i <= packetLists->currentLength) {

      if (packetLists->activeElement->nextElement == NULL) {
        DLL_First(packetLists);
      }
      active = (QosPacketListPtr)packetLists->activeElement->data;
      active_next =
          (QosPacketListPtr)packetLists->activeElement->nextElement->data;
      if ((active == NULL) || (active_next == NULL)) {
        break;
      }

      if (active->priority < active_next->priority) {
        swapp_place = packetLists->activeElement->nextElement->data;
        packetLists->activeElement->nextElement->data =
            packetLists->activeElement->data;
        packetLists->activeElement->data = swapp_place;
        swapp = 1;
      }

      DLL_Next(packetLists);
      i++;
    }
  }

  DLL_First(packetLists);
  QosPacketListPtr curr_qos;
  curr_qos = (QosPacketListPtr)packetLists->activeElement->data;
  DLL_First(curr_qos->list);
  int packets_out = 0;

  while (packets_out != maxPacketCount) {
    if (curr_qos->list->activeElement == NULL) {
      DLL_Next(packetLists);
      if (packetLists->activeElement == NULL) {
        break;
      }
      curr_qos = (QosPacketListPtr)packetLists->activeElement->data;
    }

    if (curr_qos->list->currentLength == 1) {
      DLL_InsertLast(outputPacketList, curr_qos->list->activeElement->data);
      packets_out++;
      DLL_DeleteFirst(curr_qos->list);
      continue;
    }

    DLL_InsertLast(outputPacketList, curr_qos->list->activeElement->data);
    packets_out++;
    DLL_Next(curr_qos->list);
    DLL_DeleteBefore(curr_qos->list);
    if ((curr_qos->list->activeElement == curr_qos->list->firstElement) &&
        (curr_qos->list->currentLength == 1)) {
      DLL_InsertLast(outputPacketList, curr_qos->list->activeElement->data);
      packets_out++;
      DLL_DeleteFirst(curr_qos->list);
    }
  }
}
