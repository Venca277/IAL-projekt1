/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
** uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error(void) {
  printf("*ERROR* The program has performed an illegal operation.\n");
  error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init(DLList *list) {
  // nastaví ukazatele na NULL, seznam je prázdný
  list->activeElement = NULL;
  list->firstElement = NULL;
  list->lastElement = NULL;
  list->currentLength = 0;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose(DLList *list) {
  DLLElementPtr curr;
  // projde postupně všechny prvky
  while (list->firstElement != NULL) {
    curr = list->firstElement;              // uloží element pro uvolnění
    list->firstElement = curr->nextElement; // posune prvek
    list->lastElement = curr->nextElement;  // posune prvek
    list->currentLength--;                  // dekrementuje počet prvků
    free(curr);                             // uvolní prvek
  }
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst(DLList *list, long data) {
  // alokace nového elementu
  DLLElementPtr new_element = (DLLElementPtr)malloc(sizeof(struct DLLElement));
  if (new_element == NULL) { // kontrola alokace
    DLL_Error();
    return;
  }
  // společná část
  list->currentLength++;    // inkrementace počtu prvků
  new_element->data = data; // nahrajeme nová data

  if (list->firstElement == NULL) {      // pokud je list prázdný
    new_element->nextElement = NULL;     // není další element
    new_element->previousElement = NULL; // není ani předchozí
    list->firstElement = new_element;    // nastavujeme na první
    list->lastElement = new_element;     // i na poslední
    return;
  } else {
    DLLElementPtr temp = list->firstElement; // pomocná proměnná pro přepojení
    new_element->nextElement = temp; // další element bude původní první
    new_element->previousElement = NULL; // předchozí element je NULL
    temp->previousElement = new_element; // napojení na new_element
    list->firstElement = new_element;    // nastavení prvního na new_element
    return;
  }
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k
 * DLL_InsertFirst). V případě, že není dostatek paměti pro nový prvek při
 * operaci malloc, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast(DLList *list, long data) {
  DLLElementPtr new_element = (DLLElementPtr)malloc(sizeof(struct DLLElement));
  // kontrola alokace prvku
  if (new_element == NULL) {
    DLL_Error();
    return;
  }
  list->currentLength++;    // inkrementace prvků pole
  new_element->data = data; // naplníme daty

  if (list->lastElement == NULL) {
    new_element->nextElement = NULL;     // nastaví další ukazatel na NULL
    new_element->previousElement = NULL; // nastaví předchozí ukazatel na NULL
    list->lastElement = new_element;     // nastaví první
    list->firstElement = new_element;    // nastaví poslední
    return;
  } else {
    DLLElementPtr temp = list->lastElement; // pomocná proměnná pro přemístění

    new_element->nextElement = NULL; // za novým prvkem nic už není, je poslední
    new_element->previousElement = temp; // předchozí byl původně poslední
    temp->nextElement = new_element;     // připojí nový ke starému
    list->lastElement = new_element;     // nastaví poslední na nový
    return;
  }
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First(DLList *list) {
  list->activeElement = list->firstElement; /*nastaví první na aktívní*/
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last(DLList *list) {
  list->activeElement = list->lastElement; /*nastaví poslední na aktívní*/
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst(DLList *list, long *dataPtr) {
  if (!(list->firstElement == NULL)) {   // pokud není první prázdný
    *dataPtr = list->firstElement->data; // vrátí hodnotu přes ukazatel
  } else
    DLL_Error(); // jinak vrátí error
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu
 * list. Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast(DLList *list, long *dataPtr) {
  if (!(list->firstElement == NULL)) {  // pokud není poslední prázdný
    *dataPtr = list->lastElement->data; // vrátí hodnotu přes ukazatel
  } else
    DLL_Error(); // jinak vrátí error
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst(DLList *list) {
  // kontrola práznosti listu
  if (list->firstElement != NULL && list->currentLength > 0) {
    if (list->firstElement == list->activeElement) // ztráta aktivity
      list->activeElement = NULL;
    // pomocná proměnná pro uvolnění, obsahuje první prvek
    DLLElementPtr temp = list->firstElement;
    list->currentLength--;
    if (list->firstElement->nextElement == NULL) { // pokud je prvek sám v listu
      list->firstElement = NULL;                   // reset prvního
      list->lastElement = NULL;                    // reset posledního
      free(temp);                                  // uvolnění
      return;
    }
    list->firstElement = list->firstElement->nextElement; // přepojení prvku
    list->firstElement->previousElement = NULL; // nastavení ukazatele
    if (list->firstElement->nextElement == NULL) {
      list->lastElement = list->firstElement; // pokud byl prvek poslední
    }
    free(temp); // uvolnení
  }
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast(DLList *list) {
  // kontrola jestli je list prázndý
  if (list->firstElement != NULL && list->currentLength > 0) {
    if (list->firstElement == list->activeElement)
      list->activeElement = NULL;           // ztráta aktivity listu
    DLLElementPtr temp = list->lastElement; // dočasná proměnná (last_elem)

    list->currentLength--;
    if (list->lastElement == list->firstElement) {
      list->lastElement = NULL;  // nastavení posledního
      list->firstElement = NULL; // nastavení prvního
      free(temp);                // uvolnění
      return;
    }
    list->lastElement = list->lastElement->previousElement;
    list->lastElement->nextElement = NULL; // přepojení ukazatele na NULL
    if (list->lastElement->previousElement == NULL) {
      list->firstElement = list->lastElement; // pokud byl prvek sám
    }
    free(temp); // uvolnění
  }
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter(DLList *list) {
  if (!(list->activeElement == NULL ||
        list->activeElement->nextElement == NULL)) {
    DLLElementPtr temp = list->activeElement->nextElement; // pomocná proměnná

    list->currentLength--; // dekrementace počtu prvků
    // pokud je prvek za aktivním prvkem poslední
    if (temp->nextElement == NULL) {
      list->activeElement->nextElement = NULL; // další element je NULL
      list->lastElement = list->activeElement; // aktní bude teď poslední
      free(temp);                              // uvolníme prvek
      return;
    }
    list->activeElement->nextElement =
        temp->nextElement; // přepojení ukazatelů za rušený prvek
    temp->nextElement->previousElement =
        list->activeElement; // přepojení za rušeným prvekem
    free(temp);              // uvolnění
  }
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore(DLList *list) {
  if (!(list->activeElement == NULL ||
        list->activeElement->previousElement == NULL)) {
    DLLElementPtr temp = list->activeElement->previousElement;
    // pomocná proměnná
    list->currentLength--; // dekrementace prvků v listu

    if (temp->previousElement == NULL) { // pokud byl prvek před první
      list->activeElement->previousElement = NULL; // nastaví ukazetel na null
      list->firstElement = list->activeElement;    // aktivní je teď první
      free(temp);
      return;
    }
    list->activeElement->previousElement =
        temp->previousElement; // přepojení ukazatele na prvek před
    temp->previousElement->nextElement =
        list->activeElement; // přepojení uakzetele na aktivní prvek
    free(temp);
  }
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter(DLList *list, long data) {
  if (!(list->activeElement == NULL)) {
    DLLElementPtr new_element;
    new_element = (DLLElementPtr)malloc(sizeof(struct DLLElement));
    if (new_element == NULL) {
      DLL_Error();
      return;
    }

    // společná čast nastavení
    list->currentLength++;    // inkrementace počtu prvků
    new_element->data = data; // naplní daty
    new_element->previousElement =
        list->activeElement; // napojení předchozího na aktivní

    if (list->activeElement == list->lastElement) { // pokud byl v listu sám
      new_element->nextElement = NULL; // za novým prvkem nic není
      list->lastElement = new_element; // bude poslední
      list->activeElement->nextElement =
          new_element; // aktivní bude ukazovat na nový
      return;
    }
    list->activeElement->nextElement->previousElement =
        new_element; // přepojení ukazatelů za novým
    new_element->nextElement =
        list->activeElement->nextElement; // přepojení ukazatele na další
    list->activeElement->nextElement =
        new_element; // aktivní bude ukazovat na nový
  }
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore(DLList *list, long data) {
  if (!(list->activeElement == NULL)) {
    DLLElementPtr new_element;
    new_element = (DLLElementPtr)malloc(sizeof(struct DLLElement));
    if (new_element == NULL) {
      DLL_Error();
      return;
    }

    // společné nastavení
    list->currentLength++;    // inkrementace počtu prvků
    new_element->data = data; // naplníme daty
    new_element->nextElement =
        list->activeElement; // další element bude ten nový

    if (list->activeElement == list->firstElement) { // pokud je tam sám
      new_element->previousElement = NULL;           // před novým nic není
      list->activeElement->previousElement =
          new_element;                  // pro aktivní bude přechozí nový
      list->firstElement = new_element; // nastavý nový na první
      return;
    }
    list->activeElement->previousElement->nextElement =
        new_element; // přepojení ukazatele předchozího na nový
    new_element->previousElement =
        list->activeElement->previousElement; // přepojení nového na předchozí
    list->activeElement->previousElement =
        new_element; // přepojení aktivního na nový
  }
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue(DLList *list, long *dataPtr) {
  // kontrola aktivity listu
  if (!(list->activeElement == NULL)) {
    *dataPtr = list->activeElement->data; // vrátí data skrz pointer
  } else
    DLL_Error(); // jinak list error
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného
 * seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue(DLList *list, long data) {
  // kontrola aktivity listu
  if (list->activeElement != NULL) {
    list->activeElement->data = data; // přepíše data
  }
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane
 * neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného
 * seznamu
 */
void DLL_Next(DLList *list) {
  // kontrola aktivity listu
  if (list->activeElement != NULL) {
    list->activeElement = list->activeElement->nextElement; // posune aktivitu
  }
}

/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného
 * seznamu
 */
void DLL_Previous(DLList *list) {
  // kontrola aktivity listu
  if (list->activeElement != NULL) {
    list->activeElement = list->activeElement->previousElement; // aktivita zpět
  }
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného
 * seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
bool DLL_IsActive(DLList *list) {
  return !(list->activeElement == NULL); // vrací jestli je list aktivní
}

/* Konec c206.c */