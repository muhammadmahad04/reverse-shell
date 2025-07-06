# 🛠️ Reverse Shell in C

A fully functional reverse shell written in C, built for educational and ethical hacking purposes. Includes persistence via Windows Registry and an optional keylogger.

---

## 📁 Files

- `client.c` – Connects back to the attacker, runs shell commands
- `server.cpp` – Listens for connections, sends commands
- `keylogger.h` – Keylogger thread support (optional)

---

## 🔧 Features

- Reverse TCP shell using raw sockets
- Registry persistence with Windows API
- Stealth console hiding
- Keylogging via threads
- Remote command execution with `_popen()`

---

## 💻 Usage

### 🧑‍💻 Compile Server (on Kali/Linux)

```bash
g++ server.cpp -o server
./server
