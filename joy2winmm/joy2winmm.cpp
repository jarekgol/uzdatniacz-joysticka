#include <windows.h>
#include <mmsystem.h> // WinMM API do odczytu
#include <iostream>
#include <thread>
#include <string>
#include <algorithm>
#include <fcntl.h>
#include <io.h>
#include <iomanip> // NOWY: Do formatowania cout
// !!! NAGŁÓWKI VJOY (Wymagają konfiguracji ścieżek w VS) !!!
#include "public.h"
#include "vJoyInterface.h"


#pragma comment(lib, "winmm.lib")
// #pragma comment(lib, "vJoyInterface.lib") // Ta dyrektywa jest opcjonalna, jeśli konfiguracja konsolidatora jest poprawna

// === STAŁE KONFIGURACYJNE ===
const UINT VJOY_DEVICE_ID = 1;  // ID wirtualnego joysticka (zawsze 1)
const long AXIS_CENTER = 32767;

// GRANICE DEADZONE DLA OSI RZ (Asymetryczna strefa martwa dla RZ: 22189 - 32768)
const long RZ_DEADZONE_MIN = 22189;
const long RZ_DEADZONE_MAX = 32768;

// NOWE GRANICE DEADZONE DLA OSI X (Deadzone dla X: 28671 - 39129)
const long X_DEADZONE_MIN = 28671;
const long X_DEADZONE_MAX = 39129;

// GRANICE DEADZONE DLA OSI Y (Używamy tych samych co dla X, dopóki nie zostaną podane inne)
const long Y_DEADZONE_MIN = 28605;
const long Y_DEADZONE_MAX = 35599;
// =============================

// Globalna flaga do kontrolowania pętli
volatile bool g_shouldExit = false;

// Funkcja przechwytująca sygnały konsoli (CTRL+C, zamknięcie okna)
BOOL WINAPI ConsoleHandler(DWORD dwType)
{
    // Obsługujemy sygnał CTRL+C (CTRL_C_EVENT) i zamknięcie okna (CTRL_CLOSE_EVENT)
    if (dwType == CTRL_C_EVENT || dwType == CTRL_CLOSE_EVENT)
    {
        g_shouldExit = true;
        // Wracamy TRUE, aby zasygnalizować, że obsłużyliśmy sygnał
        return TRUE;
    }
    // Wracamy FALSE, aby system Windows mógł obsłużyć inne zdarzenia
    return FALSE;
}


// --- GLOWNA FUNKCJA LOGIKI ---
int main()
{
    // Ustawienia konsoli (zachowujemy wcout, aby uniknąć problemów z asercją)
    (void)_setmode(_fileno(stdout), _O_U16TEXT);
        
    std::wcout << L"--- VJoy Feeder (WinMM -> vJoy Deadzone) ---" << std::endl;

    // --- 1. Inicjalizacja WinMM (Odczyt Fizycznego Joysticka) ---
    if (joyGetNumDevs() == 0) {
        std::wcout << L"Błąd: Nie znaleziono żadnego fizycznego joysticka WinMM." << std::endl;
        return 1;
    }

    // --- 2. Inicjalizacja vJoy (Wirtualnego Joysticka) ---
    if (!vJoyEnabled()) {
        std::wcout << L"Błąd: Sterownik vJoy nie jest aktywny. Uruchom 'Configure vJoy'." << std::endl;
        return 1;
    }

    VjdStat status = GetVJDStatus(VJOY_DEVICE_ID);
    if (status != VJD_STAT_FREE) {
        std::wcout << L"Błąd: Wirtualne Urządzenie " << VJOY_DEVICE_ID << L" nie jest wolne. Status: " << status << std::endl;
        RelinquishVJD(VJOY_DEVICE_ID); // Probujemy zwolnic na wszelki wypadek
        return 1;
    }

    if (!AcquireVJD(VJOY_DEVICE_ID)) {
        std::wcout << L"Błąd: Nie udało się przejąć wirtualnego urządzenia vJoy ID " << VJOY_DEVICE_ID << std::endl;
        return 1;
    }

    std::wcout << L"Acquire vJoy ID " << VJOY_DEVICE_ID << L" OK." << std::endl;
    std::wcout << L"Asymetryczna Deadzone RZ ustawiona na zakres: " << RZ_DEADZONE_MIN << L" - " << RZ_DEADZONE_MAX << L"." << std::endl;
    std::wcout << L"Deadzone X ustawiona na zakres: " << X_DEADZONE_MIN << L" - " << X_DEADZONE_MAX << L"." << std::endl; // NOWY komunikat

    // --- 3. Główna pętla odczytu, filtracji i wypychania ---
    JOYINFOEX joyInfo;
    joyInfo.dwSize = sizeof(JOYINFOEX);
    joyInfo.dwFlags = JOY_RETURNALL | JOY_RETURNPOV | JOY_RETURNBUTTONS;

    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE))
    {
        std::wcout << L"Ostrzeżenie: Nie udało się ustawić handlera CTRL+C. Program wyjdzie tylko przez zamknięcie okna." << std::endl;
    }
    std::wcout << L"\nGotowy. Ruszaj joystickiem i sprawdzaj wartości. Naciśnij ESC, aby wyjść." << std::endl;

    while (!g_shouldExit)
    {
       

        MMRESULT mmResult = joyGetPosEx(JOYSTICKID1, &joyInfo);

        if (mmResult == JOYERR_NOERROR)
        {
            // === 3.1. FILTROWANIE I SKALOWANIE OSI RZ (Asymetryczna Deadzone ze skalowaniem) ===
            long raw_rz = joyInfo.dwRpos; // Oś RZ w WinMM
            long filtered_rz = raw_rz; // Domyślnie kopiuj surową wartość
            BOOL isRZDeadBandActive = FALSE; // Flaga dla diagnostyki
            
            // Stałe dla skalowania dolnego zakresu RZ: 
            const long RZ_INPUT_MAX_LOWER = RZ_DEADZONE_MIN; // 22189
            const long VJOY_OUTPUT_CENTER = AXIS_CENTER;    // 32767

            // 1. Sprawdzenie i wymuszenie Strefy Martwej RZ
            if (raw_rz >= RZ_DEADZONE_MIN && raw_rz <= RZ_DEADZONE_MAX)
            {
                filtered_rz = AXIS_CENTER; // Wymuś wartość centralną (32767)
                isRZDeadBandActive = TRUE;
            }
            // 2. Skalowanie Dolnej Połówki RZ (raw_rz < RZ_DEADZONE_MIN)
            else if (raw_rz < RZ_DEADZONE_MIN)
            {
                // Przeskaluj zakres 0..RZ_DEADZONE_MIN (22189) do 0..AXIS_CENTER (32767)
                // Wzór: filtered_rz = raw_rz * (32767 / 22189)
                filtered_rz = (long)(((double)raw_rz / (double)RZ_INPUT_MAX_LOWER) * (double)VJOY_OUTPUT_CENTER);

                if (filtered_rz > AXIS_CENTER) {
                    filtered_rz = AXIS_CENTER;
                }
            }
            // 3. Wartości powyżej Deadzone RZ (raw_rz > RZ_DEADZONE_MAX)
            // Zostają skopiowane domyślnie.

            // Wypychanie przefiltrowanej RZ do vJoy
            SetAxis(filtered_rz, VJOY_DEVICE_ID, HID_USAGE_RZ);


            // === 3.2. FILTROWANIE I SKALOWANIE OSI X (Deadzone ze skalowaniem) ===
            long raw_x = joyInfo.dwXpos; // Oś X w WinMM
            long filtered_x = raw_x; // Domyślnie kopiuj surową wartość
            BOOL isXDeadBandActive = FALSE; // Flaga dla diagnostyki

            const long X_INPUT_MIN = X_DEADZONE_MIN; // 28671
            const long X_INPUT_MAX = X_DEADZONE_MAX; // 39129
            const long VJOY_OUTPUT_MAX = 65535; // Maksymalna wartość wyjściowa vJoy

            // 1. Sprawdzenie Strefy Martwej X
            if (raw_x >= X_INPUT_MIN && raw_x <= X_INPUT_MAX)
            {
                filtered_x = VJOY_OUTPUT_CENTER; // Wymuś wartość centralną (32767)
                isXDeadBandActive = TRUE;
            }
            // 2. Skalowanie Dolnej Połówki X (raw_x < X_INPUT_MIN)
            else if (raw_x < X_INPUT_MIN)
            {
                // Przeskaluj zakres 0..28671 do 0..32767
                const long X_INPUT_RANGE_LOWER = X_INPUT_MIN; // 28671
                const long X_OUTPUT_RANGE_LOWER = VJOY_OUTPUT_CENTER; // 32767

                filtered_x = (long)(((double)raw_x / (double)X_INPUT_RANGE_LOWER) * (double)X_OUTPUT_RANGE_LOWER);

                if (filtered_x > VJOY_OUTPUT_CENTER) {
                    filtered_x = VJOY_OUTPUT_CENTER;
                }
            }
            // 3. Skalowanie Górnej Połówki X (raw_x > X_INPUT_MAX)
            else if (raw_x > X_INPUT_MAX)
            {
                // Wejściowy zakres do skalowania: 39129..65535 (Rozmiar: 26406)
                // Wyjściowy zakres: 32767..65535 (Rozmiar: 32768)
                const long X_SHIFT_MIN = X_INPUT_MAX; // 39129
                const long X_INPUT_RANGE_UPPER = VJOY_OUTPUT_MAX - X_SHIFT_MIN; // 26406
                const long X_OUTPUT_RANGE_UPPER = VJOY_OUTPUT_MAX - VJOY_OUTPUT_CENTER; // 32768

                // Przesuń zakres wejściowy, aby zaczął się od 0
                long shifted_x = raw_x - X_SHIFT_MIN;

                // Skaluj i dodaj środek: filtered_x = 32767 + shifted_x * (32768 / 26406)
                filtered_x = VJOY_OUTPUT_CENTER + (long)(((double)shifted_x / (double)X_INPUT_RANGE_UPPER) * (double)X_OUTPUT_RANGE_UPPER);

                if (filtered_x > VJOY_OUTPUT_MAX) {
                    filtered_x = VJOY_OUTPUT_MAX;
                }
            }
            // Wypychanie przefiltrowanej X do vJoy
            SetAxis(filtered_x, VJOY_DEVICE_ID, HID_USAGE_X); // Filtrowana Oś X


            // === 3.3. FILTROWANIE I SKALOWANIE OSI Y (Symetryczna Deadzone - Używa Y_DEADZONE_MIN/MAX) ===
            long raw_y = joyInfo.dwYpos;
            long filtered_y = raw_y;
            BOOL isYDeadBandActive = FALSE;

            if (raw_y >= Y_DEADZONE_MIN && raw_y <= Y_DEADZONE_MAX)
            {
                filtered_y = AXIS_CENTER;
                isYDeadBandActive = TRUE;
            }
            else if (raw_y < Y_DEADZONE_MIN) // Skalowanie Dolnej Połówki
            {
                const long Y_INPUT_RANGE_LOWER = Y_DEADZONE_MIN;
                filtered_y = (long)(((double)raw_y / (double)Y_INPUT_RANGE_LOWER) * (double)AXIS_CENTER);
                if (filtered_y > AXIS_CENTER) { filtered_y = AXIS_CENTER; }
            }
            else if (raw_y > Y_DEADZONE_MAX) // Skalowanie Górnej Połówki
            {
                const long Y_SHIFT_MIN = Y_DEADZONE_MAX;
                const long Y_INPUT_RANGE_UPPER = VJOY_OUTPUT_MAX - Y_SHIFT_MIN;
                const long Y_OUTPUT_RANGE_UPPER = VJOY_OUTPUT_MAX - AXIS_CENTER; // Poprawna definicja dla osi Y
                long shifted_y = raw_y - Y_SHIFT_MIN;

                // POPRAWKA: Użycie Y_OUTPUT_RANGE_UPPER
                filtered_y = AXIS_CENTER + (long)(((double)shifted_y / (double)Y_INPUT_RANGE_UPPER) * (double)Y_OUTPUT_RANGE_UPPER);
                if (filtered_y > VJOY_OUTPUT_MAX) { filtered_y = VJOY_OUTPUT_MAX; }
            }

            SetAxis(filtered_y, VJOY_DEVICE_ID, HID_USAGE_Y);

            // KOREKTA MAPOWANIA: Mapowanie Suwaka fizycznego (Zpos) do wirtualnego SLIDER 1
            SetAxis(joyInfo.dwZpos, VJOY_DEVICE_ID, HID_USAGE_SL0);


            // === 3.3. Kopiowanie Przycisków (do 12) ===
            for (int i = 0; i < 12; i++) {
                BOOL isPressed = (joyInfo.dwButtons & (1 << i)) != 0;
                SetBtn(isPressed, VJOY_DEVICE_ID, i + 1);
            }


            // === 3.4. Kopiowanie POV (Tryb Continuous: Kąt WinMM na Kąt vJoy) ===
            long raw_pov = joyInfo.dwPOV; // Kąt WinMM (0-35900) lub 0xFFFF

            if (raw_pov == 0xFFFF)
            {
                SetContPov(-1, VJOY_DEVICE_ID, 1); // NEUTRALNY (-1)
            }
            else
            {
                SetContPov((DWORD)raw_pov, VJOY_DEVICE_ID, 1);
            }


            // === 3.5. Wypisanie wartości (Pełna Diagnostyka - Naprawione Formatowanie) ===
            // Używamy \r (Carriage Return) i manipulatorów iomanip do stałej szerokości pól
            std::wcout << L"\r| RZ Filtrowana: " << std::setw(5) << filtered_rz
                << L" | DZ: " << std::left << (isRZDeadBandActive ? L"T" : L"N")
                << L" | X: " << std::right << std::setw(5) << joyInfo.dwXpos
                << L" | XF: " << std::right << std::setw(5) << filtered_x
                << L" | DX: " << std::left << (isXDeadBandActive ? L"T" : L"N")
                << L" | Y: " << std::setw(5) << joyInfo.dwYpos
                << L" | YF: " << std::setw(5) << filtered_y
                << L" | Zpos (Suwak): " << std::setw(5) << joyInfo.dwZpos
                << L" | POV: " << std::setw(5) << joyInfo.dwPOV
                << L" | P1: " << std::left << ((joyInfo.dwButtons & 0x0001) ? L"T" : L"N");

            std::wcout.flush();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    // --- 4. Czyszczenie zasobów ---
    RelinquishVJD(VJOY_DEVICE_ID); // Zwolnienie wirtualnego joysticka
    std::wcout << L"\nProgram zakończony i zwolniono vJoy." << std::endl;

    return 0;
}