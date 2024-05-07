# C-Chat

C-Chat is a lightweight terminal-based chat application built in C using TCP sockets. It provides a simple and minimalistic interface for users to engage in text-based communication over a network. This project is suitable for educational purposes or for anyone interested in exploring network programming and inter-process communication in C.

## Features

- **Lightweight Text Chat**: Simple and easy-to-use chat app.
- **Cross-Platform**: Works on any platform that supports C and sockets, including Linux, macOS, and Windows (with appropriate environment).
- **Open Source**: Free to use, modify, and distribute.

## Getting Started

### Prerequisites

Before you start using C-Chat, ensure you have a C compiler installed (like `gcc` or `clang`) and make sure your system supports sockets.

- **Linux/macOS**: Most come with a C compiler pre-installed. You can check by running `gcc --version` or `clang --version` in the terminal.
- **Windows**: You might need to install a C compiler like MinGW or use WSL (Windows Subsystem for Linux).

### Installation

1. **Clone the repository**

   ```bash
   git clone https://github.com/SrKlono/C-Chat.git
   cd C-Chat
   ```
2. **Compile the source code**

   ```bash
   gcc *.c -o C-Chat
   ```
3. **Run**

   ```bash
   ./C-Chat
   ```

### Usage

After starting C-Chat and choosing your nickname you can:

1. **Connect** - Choose this option to connect to a host. Enter the host's IP address and port.
2. **Host** - Select this to host a chat. You will be asked to specify the port on which the server will run.

## Contributing

Contributions to C-Chat are welcome! Whether it involves fixing bugs or adding new features, feel free to fork the repository and submit a pull request.
