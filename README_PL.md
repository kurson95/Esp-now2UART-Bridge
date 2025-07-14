# 📡 Dokumentacja komend `BRCMD` (ESP8266 Firmware)(WIP)

Firmware obsługuje zestaw komend przesyłanych przez UART w formacie:

```
BRCMD+COMMAND=ARG1>ARG2
```

- Komendy z przedrostkiem `BRCMD+` są wykonywane **lokalnie**
- Komendy bez `BRCMD+` (np. `SETGPIO_X=...`) są wykonywane **zdalnie** na odbiorniku
- `>` oddziela argument opcjonalny (np. adres MAC, timeout)

---

## 📋 Spis komend

| Komenda            | Opis                                       |
|--------------------|--------------------------------------------|
| `BRCMD`            | Główna komenda uruchamiająca podkomendy    |
| `ADDRECV`          | Zapisuje adres MAC odbiornika              |
| `SHOWMAC`          | Wyświetla MAC bieżącego urządzenia         |
| `REBOOT`           | Restartuje urządzenie                      |
| `REMRECV`          | Usuwa zapisany adres MAC odbiornika        |
| `SHOWRECV`         | Pokazuje zapisany adres odbiornika         |
| `SEND`             | Wysyła wiadomość do odbiornika             |
| `SETBR`            | Ustawia prędkość UART                      |
| `SETGPIO_X`        | Steruje GPIO lokalnie lub zdalnie          |
| `READAN`           | Odczyt pinu analogowego A0                 |
| `ENC`              | Włącza/wyłącza szyfrowanie transmisji      |

---

## 🧩 Szczegóły komend

### 🔹 `BRCMD`

**Opis:**  
Główna komenda wywołująca inne komendy.

**Składnia:**
```
BRCMD+COMMAND=ARG1>ARG2
```

**Argumenty:**
- brak – wyświetla listę dostępnych komend
- `COMMAND` – wykonuje wybraną komendę lokalnie

---

### 🔹 `ADDRECV`

**Opis:**  
Zapisuje adres MAC odbiornika w pamięci flash.

**Składnia:**
```
BRCMD+ADDRECV=00:00:0A:BB:28:FC
```

**Argumenty:**
- `adres_mac` – adres w formacie `FF:FF:FF:FF:FF:FF`

---

### 🔹 `SHOWMAC`

**Opis:**  
Wyświetla adres MAC aktualnego urządzenia.

**Składnia:**
```
BRCMD+SHOWMAC
```

**Przykład odpowiedzi:**
```
OK
MAC:00:00:0A:BB:28:FC
```

---

### 🔹 `REBOOT`

**Opis:**  
Restartuje kontroler.

**Składnia:**
```
BRCMD+REBOOT
```

---

### 🔹 `REMRECV`

**Opis:**  
Usuwa zapisany adres MAC odbiornika (ustawia broadcast).

**Składnia:**
```
BRCMD+REMRECV
```

---

### 🔹 `SHOWRECV`

**Opis:**  
Wyświetla zapisany adres odbiornika.

**Składnia:**
```
BRCMD+SHOWRECV
```

---

### 🔹 `SEND`

**Opis:**  
Wysyła wiadomość tekstową do odbiornika lub steruje trybem autowysyłania.

**Składnia:**
```
BRCMD+SEND=Hello World!
BRCMD+SEND=Hello World!>00:00:0A:BB:28:FC
BRCMD+SEND=0     // wyłącza autowysyłanie
BRCMD+SEND=1     // włącza autowysyłanie
```

**Argumenty:**
- `tekst` – treść wiadomości
- `MAC` – opcjonalnie, docelowy adres MAC
- `0/1` – tryb autowysyłania (domyślnie: 1)

---

### 🔹 `SETBR`

**Opis:**  
Ustawia baud rate dla portu UART.

**Składnia:**
```
BRCMD+SETBR=115200
```

**Dozwolone wartości:**
```
300, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 230400, 250000
```

---

### 🔹 `SETGPIO_X`

**Opis:**  
Ustawia stan pinu GPIO lokalnie (`BRCMD+...`) lub zdalnie (`SETGPIO_X=...`).

**Składnia:**
```
BRCMD+SETGPIO_4=1
BRCMD+SETGPIO_6=128
BRCMD+SETGPIO_4=1>1000
SETGPIO_4=1
SETGPIO_4=1>1000
```

**Argumenty:**
- `X` – numer GPIO
- `State` – 0 (LOW), 1 (HIGH) lub 0–255 (PWM)
- `TimeOut` – (opcjonalny) czas w ms po którym pin wraca do LOW

---

### 🔹 `READAN`

**Opis:**  
Odczytuje wartość analogową z pinu A0 w zakresie 0–1023.

**Składnia:**
```
BRCMD+READAN       // lokalnie
READAN             // zdalnie
```

---

### 🔹 `ENC`

**Opis:**  
Włącza lub wyłącza szyfrowanie transmisji.

**Składnia:**
```
BRCMD+ENC=1   // włącza
BRCMD+ENC=0   // wyłącza
```

**Domyślnie:** 0 (wyłączone)

---

## 📌 Uwagi końcowe

- Komendy muszą być zakończone znakiem nowej linii (`\n`)
- Odpowiedzią na poprawną komendę jest zazwyczaj `OK`
- Komendy z `BRCMD+` wykonują się lokalnie, bez `BRCMD+` — zdalnie
- Separator `>` służy do przekazywania drugiego argumentu
- Funcje związane z GPIO działają tylko na ESP8266

---