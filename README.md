This program add user definied dead zones to all controller axis. It is a feeder for vJoy.
To compile this you need vJoy SDK.
To use this you have to install vJoy virtual controller. And copy SDK\lib\amd64\vJoyInterface.dll to you windows\System32 folder.
https://sourceforge.net/projects/vjoystick/

# Po co i dlaczego

Próbowa³em zagraæ w Ace Combat 7 z u¿yciem joysticka Logitech Extreme 3D Pro (X3D) i okaza³o siê, ¿e po zdefiniowaniu kontrolera w pliku `.ini`, samolot ci¹gle skrêca w jedn¹ stronê, nawet gdy rêka spoczywa na dr¹¿ku.

Poniewa¿ kalibracja wbudowana w system Windows jest bardzo uboga, a sama gra nie oferuje ¿adnych zaawansowanych ustawieñ kalibracji joysticka, postanowi³em stworzyæ to narzêdzie.

Program odczytuje surowe dane z joysticka, filtruje dane analogowe, stosuj¹c **martw¹ strefê (dead zone)** zgodnie z ustawieniami zdefiniowanymi na pocz¹tku pliku. Nastêpnie przepisuje przeliczone dane osi oraz status przycisków do wirtualnego joysticka vJoy. Dziêki temu lekkie odchy³ki od œrodka nie powoduj¹ niechcianego ruchu samolotu.

# Kompilacja

Potrzebne narzêdzia (wersje, których u¿ywa³em):
* **Visual Studio Community Edition 2026** (VS)
* **vJoy SDK** – biblioteki niezbêdne do komunikacji z wirtualnym joystickiem (np. vJoy218SDK-291116.zip).
* **vJoySetup.exe** – program instalacyjny wirtualnego joysticka.
* **Git** (opcjonalnie) – system kontroli wersji, umo¿liwiaj¹cy sklonowanie repozytorium i zapisywanie historii zmian.

Zak³adam, ¿e masz zainstalowane powy¿sze narzêdzia i wiesz, gdzie wypakowano SDK. U mnie kompilacja dzia³a³a na œwie¿ej instalacji VS z domyœlnymi opcjami dla C++.

### Pobieranie Ÿróde³

S¹ dwie proste opcje, aby rozpocz¹æ:

A) **Przez Git Bash:** Utwórz katalog, wejdŸ do niego, sklonuj repozytorium (`git clone https://github.com/jarekgol/uzdatniacz-joysticka.git`). Nastêpnie Eksploratorem Windows wejdŸ g³êbiej i kliknij plik `joy2winmm.slnx` – to powinno otworzyæ projekt w Visual Studio.
B) **Manualnie:** Utwórz nowy projekt konsolowy C++ w VS, a nastêpnie podmieñ treœæ g³ównego pliku na zawartoœæ `joy2winmm\joy2winmm.cpp`.

### Konfiguracja kompilatora (Jeœli wymagana)

Ustawienie œcie¿ek do SDK vJoy w VS: Otwórz **Projekt -> W³aœciwoœci** (lub `Alt+F7`).

* **Katalogi VC++:**
    * **Katalogi plików nag³ówkowych:** Dodaj œcie¿kê do plików `.h` (np. `[Twój katalog]\SDK\inc`).
    * **Katalogi bibliotek:** Dodaj œcie¿kê do plików `.lib` (np. `[Twój katalog]\SDK\lib\amd64`).
* **Konsolidator -> Dane wejœciowe:**
    * **Dodatkowe zale¿noœci:** Dodaj nazwy plików: `vJoyInterface.lib`.

Teraz projekt powinien siê kompilowaæ.

# Uruchomienie

1.  Upewnij siê, ¿e oryginalny joystick dzia³a. U¿yj systemowego narzêdzia `joy.cpl` lub przejdŸ do "Kontrolery gier". Zobacz, co siê zmienia po u¿yciu joysticka, zapamiêtaj wygl¹d okna lub zostaw je otwarte.
2.  Uruchom program **`vjoyconf`** (Configure vJoy) i skonfiguruj wirtualny joystick tak, aby odpowiada³ orygina³owi (mo¿esz usun¹æ lub dodaæ osie, ale pamiêtaj o zaimplementowaniu nowych osi w kodzie Ÿród³owym).
    * Dla X3D: Osi X, Y, Rz, Suwak (Slider), 12 przycisków (buttons), oraz Mini-joystick (POV hat) ustaw na **Continous 1**.
3.  SprawdŸ w `joy.cpl`, czy wirtualny joystick jest widoczny i czy jego konfiguracja jest poprawna. Przed uruchomieniem programu nie bêdzie on reagowa³.
4.  Uruchom program `joy2winmm.exe`.
5.  Poruszaj joystickiem – wirtualny kontroler powinien zacz¹æ reagowaæ.
6.  PrzeprowadŸ standardow¹ kalibracjê Windows.
7.  W oknie konsoli bêd¹ widoczne dane z joysticka oraz informacje diagnostyczne o statusie martwej strefy. Jeœli w konsoli ci¹gle przybywa nowych linii, zwiêksz szerokoœæ terminala (np. Pasek tytu³u prawym, Settings, Startup, Launch size, Columns -> zwiêksz do 130).

# Zamykanie

Program mo¿na zamkn¹æ poprzez kombinacjê klawiszy **`CTRL+C`** lub przez zamkniêcie okna terminala. Program nie zapisuje ¿adnych ustawieñ na dysk i jest odporny na b³êdy.

# Zmiany i Adaptacja

Program ma domyœlnie wpisane wartoœci dla mojego joysticka. Poniewa¿ oœ Rz mia³a problem z dryfem tylko w jedn¹ stronê, jest filtrowana asymetrycznie (tylko dla wartoœci poni¿ej po³owy). Osie X i Y s¹ liczone symetrycznie (z obu stron martwej strefy). Suwak/przepustnica, przyciski i mini-joystick rozgl¹dania siê s¹ przepisywane bezpoœrednio.

* Jeœli chcesz zmieniæ ustawienia stref, zmieñ wartoœci **`const long`** na górze pliku `joy2winmm.cpp`.
* Jeœli adaptujesz to do innego kontrolera z inn¹ iloœci¹ osi, musisz samodzielnie zaimplementowaæ ich obs³ugê. W katalogu SDK znajduje siê PDF z opisem biblioteki.

Spory udzia³ w powstaniu kodu mia³o darmowe Gemini, wiêc polecam do wprowadzania zmian, zw³aszcza jeœli jesteœ bardziej graczem ni¿ programist¹.

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