This program add user definied dead zones to all controller axis. It is a feeder for vJoy.
To compile this you need vJoy SDK.
To use this you have to install vJoy virtual controller. And copy SDK\lib\amd64\vJoyInterface.dll to you windows\System32 folder.
https://sourceforge.net/projects/vjoystick/

# Po co i dlaczego

Próbowałem zagrać w Ace Combat 7 z użyciem joysticka Logitech Extreme 3D Pro (X3D) i okazało się, że po zdefiniowaniu kontrolera w pliku `.ini`, samolot ciągle skręca w jedną stronę, nawet gdy ręka spoczywa na drążku.

Ponieważ kalibracja wbudowana w system Windows jest bardzo uboga, a sama gra nie oferuje żadnych zaawansowanych ustawień kalibracji joysticka, postanowiłem stworzyć to narzędzie.

Program odczytuje surowe dane z joysticka, filtruje dane analogowe, stosując **martwą strefę (dead zone)** zgodnie z ustawieniami zdefiniowanymi na początku pliku. Następnie przepisuje przeliczone dane osi oraz status przycisków do wirtualnego joysticka vJoy. Dzięki temu lekkie odchyłki od środka nie powodują niechcianego ruchu samolotu.

# Kompilacja

Potrzebne narzędzia (wersje, których używałem):
* **Visual Studio Community Edition 2026** (VS)
* **vJoy SDK** – biblioteki niezbędne do komunikacji z wirtualnym joystickiem (np. vJoy218SDK-291116.zip).
* **vJoySetup.exe** – program instalacyjny wirtualnego joysticka.
* **Git** (opcjonalnie) – system kontroli wersji, umożliwiający sklonowanie repozytorium i zapisywanie historii zmian.

Zakładam, że masz zainstalowane powyższe narzędzia i wiesz, gdzie wypakowano SDK. U mnie kompilacja działała na świeżej instalacji VS z domyślnymi opcjami dla C++.

### Pobieranie źródeł

Są dwie proste opcje, aby rozpocząć:

A) **Przez Git Bash:** Utwórz katalog, wejdź do niego, sklonuj repozytorium (`git clone https://github.com/jarekgol/uzdatniacz-joysticka.git`). Następnie Eksploratorem Windows wejdź głębiej i kliknij plik `joy2winmm.slnx` – to powinno otworzyć projekt w Visual Studio.
B) **Manualnie:** Utwórz nowy projekt konsolowy C++ w VS, a następnie podmień treść głównego pliku na zawartość `joy2winmm\joy2winmm.cpp`.

### Konfiguracja kompilatora (Jeśli wymagana)

Ustawienie ścieżek do SDK vJoy w VS: Otwórz **Projekt -> Właściwości** (lub `Alt+F7`).

* **Katalogi VC++:**
    * **Katalogi plików nagłówkowych:** Dodaj ścieżkę do plików `.h` (np. `[Twój katalog]\SDK\inc`).
    * **Katalogi bibliotek:** Dodaj ścieżkę do plików `.lib` (np. `[Twój katalog]\SDK\lib\amd64`).
* **Konsolidator -> Dane wejściowe:**
    * **Dodatkowe zależności:** Dodaj nazwy plików: `vJoyInterface.lib`.

Teraz projekt powinien się kompilować.

# Uruchomienie

1.  Upewnij się, że oryginalny joystick działa. Użyj systemowego narzędzia `joy.cpl` lub przejdź do "Kontrolery gier". Zobacz, co się zmienia po użyciu joysticka, zapamiętaj wygląd okna lub zostaw je otwarte.
2.  Uruchom program **`vjoyconf`** (Configure vJoy) i skonfiguruj wirtualny joystick tak, aby odpowiadał oryginałowi (możesz usunąć lub dodać osie, ale pamiętaj o zaimplementowaniu nowych osi w kodzie źródłowym).
    * Dla X3D: Osi X, Y, Rz, Suwak (Slider), 12 przycisków (buttons), oraz Mini-joystick (POV hat) ustaw na **Continous 1**.
3.  Sprawdź w `joy.cpl`, czy wirtualny joystick jest widoczny i czy jego konfiguracja jest poprawna. Przed uruchomieniem programu nie będzie on reagował.
4.  Uruchom program `joy2winmm.exe`.
5.  Poruszaj joystickiem – wirtualny kontroler powinien zacząć reagować.
6.  Przeprowadź standardową kalibrację Windows.
7.  W oknie konsoli będą widoczne dane z joysticka oraz informacje diagnostyczne o statusie martwej strefy. Jeśli w konsoli ciągle przybywa nowych linii, zwiększ szerokość terminala (np. Pasek tytułu prawym, Settings, Startup, Launch size, Columns -> zwiększ do 130).

# Zamykanie

Program można zamknąć poprzez kombinację klawiszy **`CTRL+C`** lub przez zamknięcie okna terminala. Program nie zapisuje żadnych ustawień na dysk i jest odporny na błędy.

# Zmiany i Adaptacja

Program ma domyślnie wpisane wartości dla mojego joysticka. Ponieważ oś Rz miała problem z dryfem tylko w jedną stronę, jest filtrowana asymetrycznie (tylko dla wartości poniżej połowy). Osie X i Y są liczone symetrycznie (z obu stron martwej strefy). Suwak/przepustnica, przyciski i mini-joystick rozglądania się są przepisywane bezpośrednio.

* Jeśli chcesz zmienić ustawienia stref, zmień wartości **`const long`** na górze pliku `joy2winmm.cpp`.
* Jeśli adaptujesz to do innego kontrolera z inną ilością osi, musisz samodzielnie zaimplementować ich obsługę. W katalogu SDK znajduje się PDF z opisem biblioteki.

Spory udział w powstaniu kodu miało darmowe Gemini, więc polecam do wprowadzania zmian, zwłaszcza jeśli jesteś bardziej graczem niż programistą.

---
---

## ???? English Version (Machine Translation Disclaimer)

***
**?? DISCLAIMER:** This is a machine translation of the Polish documentation. Please refer to the original Polish version for absolute accuracy.
***

```markdown
This program add user definied dead zones to all controller axis. It is a feeder for vJoy.
To compile this you need vJoy SDK.
To use this you have to install vJoy virtual controller. And copy SDK\lib\amd64\vJoyInterface.dll to you windows\System32 folder.
[https://sourceforge.net/projects/vjoystick/](https://sourceforge.net/projects/vjoystick/)

# Purpose and Rationale

I was trying to play Ace Combat 7 using the Logitech Extreme 3D Pro (X3D) joystick, and it turned out that after defining the controller in the `.ini` file, the aircraft constantly drifted to one side even when my hand was resting on the stick.

Since the calibration built into Windows is very poor, and the game itself does not offer any advanced joystick calibration settings, I decided to create this tool.

The program reads raw data from the joystick, filters the analog data by applying a **dead zone** according to the settings defined at the top of the file. It then translates the calculated axis data and button status to the virtual vJoy joystick. This ensures that slight deviations from the center do not cause unwanted aircraft movement.

# Compilation

Required tools (versions I used):
* **Visual Studio Community Edition 2026** (VS)
* **vJoy SDK** – libraries necessary for communication with the virtual joystick (e.g., vJoy218SDK-291116.zip).
* **vJoySetup.exe** – virtual joystick installation program.
* **Git** (optional) – version control system, allowing you to clone the repository and save the history of changes.

I assume you have the above tools installed and know where the SDK was extracted. Compilation worked for me on a fresh VS installation with default options for C++.

### Getting the Source

There are two simple options to get started:

A) **Via Git Bash:** Create a directory, enter it, clone the repository (`git clone https://github.com/jarekgol/uzdatniacz-joysticka.git`). Then use Windows Explorer to go deeper and click the `joy2winmm.slnx` file – this should open the project in Visual Studio.
B) **Manually:** Create a new C++ console project in VS, then replace the content of the main file with the content of `joy2winmm\joy2winmm.cpp`.

### Compiler Configuration (If Required)

To set the vJoy SDK paths in VS: Open **Project -> Properties** (or `Alt+F7`).

* **VC++ Directories:**
    * **Include Directories:** Add the path to the `.h` files (e.g., `[Your Directory]\SDK\inc`).
    * **Library Directories:** Add the path to the `.lib` files (e.g., `[Your Directory]\SDK\lib\amd64`).
* **Linker -> Input:**
    * **Additional Dependencies:** Add the file names: `vJoyInterface.lib`.

The project should now compile.

# Running

1.  Make sure your original joystick is working. Use the system tool `joy.cpl` or navigate to "Game Controllers". Observe what changes when you use the joystick, remember the window's appearance, or leave it open.
2.  Launch **`vjoyconf`** (Configure vJoy) and set up the virtual joystick to match the original (you can remove or add axes, but remember to implement new axes in the source code).
    * For X3D: Set X, Y, Rz, Slider, 12 buttons, and the Mini-joystick (POV hat) to **Continous 1**.
3.  Check `joy.cpl` to ensure the virtual joystick is visible and configured correctly. It will not respond before running the program.
4.  Run the `joy2winmm.exe` program.
5.  Move the joystick – the virtual controller should start responding.
6.  Perform the standard Windows calibration.
7.  The console window will display joystick data and diagnostic information about the dead zone status. If the console keeps receiving new lines, increase the terminal width (e.g., Title Bar Right-click, Settings, Startup, Launch size, Columns -> increase to 130).

# Closing

The program can be closed by pressing **`CTRL+C`** or by closing the terminal window. The program does not save any settings to disk and is robust.

# Changes and Adaptation

The program has values hardcoded for my specific joystick. Since the Rz axis only had a drift issue on one side, it is filtered asymmetrically (only for values below half). The X and Y axes are calculated symmetrically (from both sides of the dead zone). The throttle/slider, buttons, and mini-joystick (POV) are passed through directly.

* If you want to change the dead zone settings, modify the **`const long`** values at the top of the `joy2winmm.cpp` file.
* If you are adapting this for a different controller with a different number of axes, you must implement the handling for them yourself. A PDF describing the library is located in the SDK directory.

The free Gemini AI had a significant contribution to the code, so I recommend it for making changes, especially if you are more of a gamer than a programmer.