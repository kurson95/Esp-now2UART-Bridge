# 📡 `BRCMD` Command Documentation (ESP8266 Firmware)(WIP)

The firmware supports a set of commands sent over UART in the format:

```
BRCMD+COMMAND=ARG1>ARG2
```

- Commands with the `BRCMD+` prefix are executed **locally**
- Commands without the prefix (e.g. `SETGPIO_X=...`) are executed **remotely** by the receiver
- The `>` symbol is used to pass an optional second argument (e.g. MAC address, timeout)

---

## 📋 Command Overview

| Command            | Description                                  |
|--------------------|----------------------------------------------|
| `BRCMD`            | Main command used to invoke subcommands      |
| `ADDRECV`          | Stores receiver's MAC address in flash       |
| `SHOWMAC`          | Displays device's MAC address                |
| `REBOOT`           | Restarts the device                          |
| `REMRECV`          | Removes stored receiver MAC address          |
| `SHOWRECV`         | Shows stored receiver MAC address            |
| `SEND`             | Sends a message to the receiver              |
| `SETBR`            | Sets UART baud rate                          |
| `SETGPIO_X`        | Controls GPIO pins locally or remotely       |
| `READAN`           | Reads analog value from pin A0               |
| `ENC`              | Enables/disables transmission encryption     |

---

## 🧩 Command Details

### 🔹 `BRCMD`

**Description:**  
Main command that triggers other commands.

**Syntax:**
```
BRCMD+COMMAND=ARG1>ARG2
```

**Arguments:**
- none – displays a list of available commands
- `COMMAND` – executes the given command locally

---

### 🔹 `ADDRECV`

**Description:**  
Stores the receiver's MAC address in flash memory.

**Syntax:**
```
BRCMD+ADDRECV=00:00:0A:BB:28:FC
```

**Arguments:**
- `mac_address` – format: `FF:FF:FF:FF:FF:FF`

---

### 🔹 `SHOWMAC`

**Description:**  
Displays the MAC address of the current device.

**Syntax:**
```
BRCMD+SHOWMAC
```

**Example response:**
```
OK
MAC:00:00:0A:BB:28:FC
```

---

### 🔹 `REBOOT`

**Description:**  
Restarts the controller.

**Syntax:**
```
BRCMD+REBOOT
```

---

### 🔹 `REMRECV`

**Description:**  
Deletes the saved receiver MAC address (fallback to broadcast).

**Syntax:**
```
BRCMD+REMRECV
```

---

### 🔹 `SHOWRECV`

**Description:**  
Displays the saved MAC address of the receiver.

**Syntax:**
```
BRCMD+SHOWRECV
```

---

### 🔹 `SEND`

**Description:**  
Sends a text message to the receiver or controls auto-send mode.

**Syntax:**
```
BRCMD+SEND=Hello World!
BRCMD+SEND=Hello World!>00:00:0A:BB:28:FC
BRCMD+SEND=0     // disable auto-sending
BRCMD+SEND=1     // enable auto-sending
```

**Arguments:**
- `text` – message content
- `MAC` – optional, target MAC address
- `0/1` – auto-send mode (default: 1)

---

### 🔹 `SETBR`

**Description:**  
Sets the UART baud rate.

**Syntax:**
```
BRCMD+SETBR=115200
```

**Allowed values:**
```
300, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 230400, 250000
```

---

### 🔹 `SETGPIO_X`

**Description:**  
Sets the state of GPIO pin locally (`BRCMD+...`) or remotely (`SETGPIO_X=...`).

**Syntax:**
```
BRCMD+SETGPIO_4=1
BRCMD+SETGPIO_6=128
BRCMD+SETGPIO_4=1>1000
SETGPIO_4=1
SETGPIO_4=1>1000
```

**Arguments:**
- `X` – GPIO number
- `State` – 0 (LOW), 1 (HIGH), or 0–255 (PWM)
- `TimeOut` – (optional) time in ms after which the pin resets to LOW

---

### 🔹 `READAN`

**Description:**  
Reads analog value from pin A0 (range: 0–1023).

**Syntax:**
```
BRCMD+READAN       // local
READAN             // remote
```

---

### 🔹 `ENC`

**Description:**  
Enables or disables transmission encryption.

**Syntax:**
```
BRCMD+ENC=1   // enable
BRCMD+ENC=0   // disable
```

**Default:** 0 (disabled)

---

## 📌 Notes

- Commands must end with a newline character (`\n`)
- Most commands return `OK` upon success
- `BRCMD+` prefix = local, no prefix = remote
- `>` is used to pass optional second arguments
- GPIO related functions are only available on ESP8266
---