/* c201.c **********************************************************************
** Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                              Úpravy: Kamil Jeřábek, září 2020
**                                                    Daniel Dolejška, září 2021
**                                                    Daniel Dolejška, září 2022
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu List.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ List:
**
**      List_Dispose ....... zrušení všech prvků seznamu,
**      List_Init .......... inicializace seznamu před prvním použitím,
**      List_InsertFirst ... vložení prvku na začátek seznamu,
**      List_First ......... nastavení aktivity na první prvek,
**      List_GetFirst ...... vrací hodnotu prvního prvku,
**      List_DeleteFirst ... zruší první prvek seznamu,
**      List_DeleteAfter ... ruší prvek za aktivním prvkem,
**      List_InsertAfter ... vloží nový prvek za aktivní prvek seznamu,
**      List_GetValue ...... vrací hodnotu aktivního prvku,
**      List_SetValue ...... přepíše obsah aktivního prvku novou hodnotou,
**      List_Next .......... posune aktivitu na další prvek seznamu,
**      List_IsActive ...... zjišťuje aktivitu seznamu.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam předá
** někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**/

#include "c201.h"

#include <stdio.h>  // printf
#include <stdlib.h> // malloc, free

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě. Nastaví error_flag na
 * logickou 1. Tato funkce bude volána z některých dále implementovaných
 * operací.
 */
void List_Error(void) {
  printf("*ERROR* The program has performed an illegal operation.\n");
  error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným
 * seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
 * že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu jednosměrně vázaného seznamu
 */
void List_Init(List *list) {
  list->activeElement = NULL;
  list->firstElement = NULL;
  list->currentLength = 0;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam list do stavu, v jakém se
 *nacházel po inicializaci. Veškerá paměť používaná prvky seznamu list bude
 *korektně uvolněna voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného
 *seznamu
 **/
void List_Dispose(List *list) {
  ListElementPtr temp;
  ListElementPtr temp_next;
  temp = malloc(sizeof(ListElementPtr));
  temp_next = malloc(sizeof(ListElementPtr));
  if (temp == NULL || temp_next == NULL) {
    List_Error();
  }
  temp = list->firstElement;
  free(list->firstElement);
  for (size_t i = 0; i < list->currentLength - 1; i++) {
    temp_next = temp;
    free(temp);
    temp = temp_next->nextElement;
    free(temp_next);
  }
}

/**
 * Vloží prvek s hodnotou data na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného
 * seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void List_InsertFirst(List *list, int data) {
  ListElementPtr new;
  ListElementPtr temp;
  new = malloc(sizeof(ListElementPtr));
  temp = malloc(sizeof(ListElementPtr));
  if (new == NULL || temp == NULL) {
    List_Error();
  }
  temp = list->firstElement;
  new->data = data;
  new->nextElement = temp;
  list->firstElement = new;
}

/**
 * Nastaví aktivitu seznamu list na jeho první prvek.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného
 * seznamu
 */
void List_First(List *list) { list->activeElement = list->firstElement; }

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného
 * seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void List_GetFirst(List *list, int *dataPtr) {
  if (List_IsActive(list)) {
    dataPtr = (int *)list->firstElement->data;
  } else
    List_Error();
}

/**
 * Zruší první prvek seznamu list a uvolní jím používanou paměť.
 * Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného
 * seznamu
 */
void List_DeleteFirst(List *list) {
  if (List_IsActive(list)) {
    if (list->firstElement == list->activeElement) {
      list->firstElement->data = 0;
      free(list->firstElement);
      list->currentLength--;
      return;
    }
    ListElementPtr temp;
    temp = malloc(sizeof(ListElementPtr));
    temp = list->firstElement;
    free(list->firstElement);
    list->firstElement = temp;
    free(temp);
    list->currentLength--;
  }
}

/**
 * Zruší prvek seznamu list za aktivním prvkem a uvolní jím používanou paměť.
 * Pokud není seznam list aktivní nebo pokud je aktivní poslední prvek seznamu
 * list, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného
 * seznamu
 */
void List_DeleteAfter(List *list) {
  if (List_IsActive(list) && list->activeElement->nextElement != NULL) {
    list->activeElement->nextElement->data = 0;
    free(list->activeElement->nextElement);
    list->currentLength--;
  }
}

/**
 * Vloží prvek s hodnotou data za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje!
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * zavolá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného
 * seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void List_InsertAfter(List *list, int data) {
  if (List_IsActive(list)) {
    ListElementPtr new_element;
    new_element = malloc(sizeof(ListElementPtr));
    if (new_element == NULL) {
      List_Error();
    }
    new_element->nextElement = list->activeElement->nextElement;
    list->activeElement->nextElement = new_element;
    new_element->data = data;
    list->currentLength++;
  }
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam není aktivní, zavolá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného
 * seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void List_GetValue(List *list, int *dataPtr) {
  if (List_IsActive(list)) {
    dataPtr = (int *)list->activeElement->data;
  } else
    List_Error();
}

/**
 * Přepíše data aktivního prvku seznamu list hodnotou data.
 * Pokud seznam list není aktivní, nedělá nic!
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného
 * seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void List_SetValue(List *list, int data) {
  if (List_IsActive(list)) {
    list->activeElement->data = data;
    return 1;
  }
  return 0;
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
 * Pokud není předaný seznam list aktivní, nedělá funkce nic.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného
 * seznamu
 */
void List_Next(List *list) {
  if (List_IsActive(list)) {
    list->activeElement = list->activeElement->nextElement;
  }
  return 0;
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Tuto funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného
 * seznamu
 */
int List_IsActive(List *list) {
  if (list->activeElement != NULL) {
    return 1;
  }
  return 0;
}

/* Konec c201.c */
