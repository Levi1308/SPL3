# Emergency Service Platform 

## Overview
This project implements an "Emergency Service" platform that allows users to subscribe to emergency channels and receive relevant updates. The platform facilitates communication between clients and a centralized server using the STOMP (Simple-Text-Oriented Messaging Protocol).

The project consists of two main components:
- **Server (Java):** Provides STOMP server services and supports two modes of operation: Thread-Per-Client (TPC) and Reactor.
- **Client (C++):** Allows users to interact with the system by subscribing to channels, reporting emergencies, and receiving updates.

---

## Table of Contents
- [Features](#features)
- [Technologies](#technologies)
- [Installation](#installation)
- [Usage](#usage)
- [STOMP Protocol Overview](#stomp-protocol-overview)
- [Commands](#commands)
- [Testing](#testing)
- [Bonus - Git Usage](#bonus---git-usage)

---

## Features
- Subscribe to emergency channels (fire, medical, police, natural disasters)
- Report emergencies and broadcast messages to subscribers
- Two server operation modes:
  - **Thread-Per-Client (TPC):** A dedicated thread for each client
  - **Reactor:** Event-driven architecture for efficient client handling
- Secure communication with username-password authentication
- Real-time emergency event dissemination

---

## Technologies
- **Server:** Java (Maven build tool)
- **Client:** C++ (Makefile for build)
- **Protocol:** STOMP 1.2
- **Networking:** TCP/IP

---

## Installation
### Server Setup (Java)
1. Clone the repository:
   ```bash
   git clone <repository-url>
   ```
2. Navigate to the server directory:
   ```bash
   cd server
   ```
3. Compile the server:
   ```bash
   mvn compile
   ```
4. Run the server:
   ```bash
   mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="<port> <tpc/reactor>"
   ```
   Example:
   ```bash
   mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="7777 tpc"
   ```

### Client Setup (C++)
1. Navigate to the client directory:
   ```bash
   cd client
   ```
2. Compile the client:
   ```bash
   make
   ```
3. Run the client:
   ```bash
   ./bin/StompEMIClient
   ```

---

## Usage
### Server
- The server listens for client connections and distributes messages to the appropriate channels.
- Use either TPC or Reactor mode by specifying it in the startup arguments.

### Client
- The client connects to the server and allows users to:
  - **Login:** Connect to the server with username and password
  - **Join Channels:** Subscribe to emergency topics
  - **Report Emergencies:** Send emergency reports
  - **Receive Updates:** Listen for new emergency events
  - **Logout:** Gracefully disconnect from the server

---

## STOMP Protocol Overview
STOMP is a lightweight messaging protocol that uses frames for communication.
- **Frame Structure:**
  ```
  <COMMAND>
  <header1>:<value1>
  <header2>:<value2>
  
  <Body>
  ^@
  ```
- **Client Frames:**
  - CONNECT, SEND, SUBSCRIBE, UNSUBSCRIBE, DISCONNECT
- **Server Frames:**
  - CONNECTED, MESSAGE, RECEIPT, ERROR

---

## Commands
- **Login:** `login {host:port} {username} {password}`
- **Join Channel:** `join {channel_name}`
- **Exit Channel:** `exit {channel_name}`
- **Report:** `report {file}`
- **Summarize Reports:** `summary {channel_name} {user} {file}`
- **Logout:** `logout`

---

## Testing
- **Server Testing:** A pre-compiled `.jar` (StompServer.jar) is provided in `/server/target`.
- **Client Testing:** Use the provided EchoClient as a reference.

To run the pre-compiled server:
```bash
java -jar StompServer.jar <port> <reactor/tpc>
```

---

## Bonus - Git Usage
Earn bonus points by demonstrating proper Git usage:
- **3 points:** Create a Git repository and commit regularly.
- **2 points:** Use clear and descriptive commit messages.

### Resources:
- [Learn Git](https://learngitbranching.js.org/)
- [Good vs Bad Commit Messages](https://dev.to/sheraz4194/good-commit-vs-bad-commit-best-practices-for-git-1plc)

---



