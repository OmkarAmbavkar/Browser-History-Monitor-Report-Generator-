# Browser History Monitor & Report Generator (C++)

This project is a Windows background application written in C++ that monitors browser activity and automatically generates a browsing history report during system shutdown.

The application runs silently in the background and performs all operations without user interaction.

---

## Features

- Runs as a background Windows application
- Detects system shutdown events
- Extracts Google Chrome browsing history
- Stores URLs and timestamps using SQLite
- Generates a PDF report from browsing data
- Sends the PDF report automatically via email
- Exits cleanly without blocking shutdown

---

## How It Works

1. The application starts and runs silently in the background.
2. It listens for Windows shutdown events using system messages.
3. When shutdown is detected:
   - The Chrome history database is safely copied
   - Browsing data is extracted using SQLite
   - A PDF report is generated
   - The report is sent via SMTP email
4. The application terminates immediately after completion.

---

## Technologies Used

- C++
- Windows API
- SQLite
- libharu (PDF generation)
- libcurl (SMTP email)

---

## Project Structure

obzen_task4/
├── src/
│ └── main.cpp
├── third_party/
│ ├── sqlite3.c
│ ├── sqlite3.h
├── data/
│ ├── history.txt
│ └── history.pdf
├── obzen.exe
├── hpdf.dll
├── libpng16.dll
├── zlib1.dll
├── libcurl.dll
└── README.md

---

## Email Configuration

- Email is sent using SMTP.
- Credentials are configured locally using an app password.
- For security reasons, real email credentials are not included in this repository.

---

## Use Cases

- System-level automation demonstration
- Windows shutdown event handling
- Background application design
- PDF generation and email automation in C++

---

## Testing

- Tested on Windows 10 and Windows 11
- Shutdown tested using system sign-out and restart
- Email delivery with PDF attachment verified locally

---

## Limitations

- Tested mainly with Google Chrome browser
- Requires SMTP access for email delivery
- A brief application closing notification may appear during shutdown

---

## Build Notes

- Built using Visual Studio Build Tools (MSVC)
- Required runtime DLLs must be present alongside the executable

---

## Author

Omkar Ambavkar

