#include <iostream>   // Biblioteka do obsługi strumieni wejścia/wyjścia
#include <windows.h>  // Biblioteka do obsługi systemu Windows
#include <queue>      // Biblioteka do obsługi kolejek
#include <ctime>      // Biblioteka do obsługi czasu

using namespace std;

const int k = 5; // Liczba palaczy
const int l = 3; // Liczba ubijaczy
const int m = 2; // Liczba pudełek zapałek

HANDLE ubijaczeSem[l]; // Semafory dla ubijaczy
HANDLE zapalkiSem[m];  // Semafory dla pudełek zapałek
HANDLE ekranSem;        // Semafory dla wyświetlacza

queue<int> ubijaczeQueue[l]; // Kolejki dla ubijaczy
queue<int> zapalkiQueue[m];  // Kolejki dla pudełek zapałek

// Funkcja do wyświetlania komunikatów
void WyswietlKomunikat(const char* komunikat, int palaczId, bool czeka = false) {
    WaitForSingleObject(ekranSem, INFINITE); // Czekaj na dostęp do wyświetlacza
    if (czeka)
        cout << "Palacz " << palaczId << ": CZEKAM - " << komunikat << endl; // Wyświetl komunikat z informacją o oczekiwaniu
    else
        cout << "Palacz " << palaczId << ": " << komunikat << endl; // Wyświetl standardowy komunikat
    ReleaseSemaphore(ekranSem, 1, NULL); // Zwolnij dostęp do wyświetlacza
}

// Funkcja wykonywana przez wątki palaczy
DWORD WINAPI PalaczThread(LPVOID lpParam) {
    int palaczId = *(int*)lpParam; // Pobierz identyfikator palacza

    srand(time(0)); // Inicjalizacja generatora liczb losowych

    while (true) {
        // Prośba o ubijacza
        WyswietlKomunikat("żądanie ubijacza", palaczId); // Wyświetl komunikat żądania ubijacza
        WyswietlKomunikat("czekam na ubijacza", palaczId, true); // Dodatkowy komunikat oczekiwania
        WaitForSingleObject(ubijaczeSem[palaczId % l], INFINITE); // Czekaj na dostęp do ubijacza
        WyswietlKomunikat("ubijanie", palaczId); // Wyświetl komunikat o ubijaniu
        Sleep((rand() % 3000) + 1000); // Losowy czas ubijania (od 1 do 3 sekund)
        WyswietlKomunikat("zwrot ubijacza", palaczId); // Wyświetl komunikat o zwrocie ubijacza
        ReleaseSemaphore(ubijaczeSem[palaczId % l], 1, NULL); // Zwolnij dostęp do ubijacza

        // Prośba o pudełko zapałek
        WyswietlKomunikat("żądanie pudełka zapałek", palaczId); // Wyświetl komunikat żądania pudełka zapałek
        WyswietlKomunikat("czekam na pudełko zapałek", palaczId, true); // Dodatkowy komunikat oczekiwania
        WaitForSingleObject(zapalkiSem[0], INFINITE); // Czekaj na dostęp do pudełka zapałek
        WyswietlKomunikat("zapalenie fajki", palaczId); // Wyświetl komunikat o zapaleniu fajki
        Sleep((rand() % 3000) + 1000); // Losowy czas zapalania (od 1 do 3 sekund)
        WyswietlKomunikat("zwrot pudełka zapałek", palaczId); // Wyświetl komunikat o zwrocie pudełka zapałek
        ReleaseSemaphore(zapalkiSem[0], 1, NULL); // Zwolnij dostęp do pudełka zapałek

        WyswietlKomunikat("palenie fajki", palaczId); // Wyświetl komunikat o paleniu fajki
        Sleep((rand() % 3000) + 1000); // Losowy czas palenia (od 1 do 3 sekund)

        WyswietlKomunikat("czekam na następny cykl", palaczId, true); // Dodatkowy komunikat oczekiwania
        Sleep((rand() % 3000) + 1000); // Losowy czas przed kolejnym cyklem
    }

    return 0; // Zakończ wątek
}

int main() {
    for (int i = 0; i < l; ++i) {
        ubijaczeSem[i] = CreateSemaphore(NULL, 1, 1, NULL); // Utwórz semafory dla ubijaczy
    }
    for (int i = 0; i < m; ++i) {
        zapalkiSem[i] = CreateSemaphore(NULL, 1, 1, NULL); // Utwórz semafory dla pudełek zapałek
    }
    ekranSem = CreateSemaphore(NULL, 1, 1, NULL); // Utwórz semafor dla wyświetlacza

    HANDLE threads[k]; // Tablica uchwytów wątków
    int palaczeIds[k]; // Tablica identyfikatorów palaczy
    for (int i = 0; i < k; ++i) {
        palaczeIds[i] = i + 1; // Przypisz identyfikatory palaczy
        threads[i] = CreateThread(NULL, 0, PalaczThread, &palaczeIds[i], 0, NULL); // Utwórz wątki palaczy
    }

    WaitForMultipleObjects(k, threads, TRUE, INFINITE); // Oczekuj na zakończenie wątków

    for (int i = 0; i < l; ++i) {
        CloseHandle(ubijaczeSem[i]); // Zamknij uchwyty semaforów dla ubijaczy
    }
    for (int i = 0; i < m; ++i) {
        CloseHandle(zapalkiSem[i]); // Zamknij uchwyty semaforów dla pudełek zapałek
    }
    CloseHandle(ekranSem); // Zamknij uchwyt semafora dla wyświetlacza

    return 0; // Zakończ działanie programu
}
