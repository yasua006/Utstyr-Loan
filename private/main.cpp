// få både input og output funksjonene
#include <iostream>
// få string funksjonen
#include <string>
// få sqlite3
#include <sqlite3.h>
// bruk headeren for å få utstyr info
#include "utstyr_info.h"

// mindre kopi lim inn
using std::cin;
using std::cout;
using std::clog;
using std::cerr;
using std::endl;
using std::string;

string id_str = "Identifiser utstyret med id: ";


// funksjon for assistering av preparasjoner
void prep_hjelper(sqlite3 *db, sqlite3_stmt *&pSmt, const char *zSQL) {
    // lag preparasjon
    int db_prep = sqlite3_prepare_v2 (
        db,
        zSQL,
        -1, // kjør til null skjer
        &pSmt, // koble til databasen
        NULL
    );

    // håndtere sqlite feil på prep
    if (db_prep != SQLITE_OK) {
        cerr << "Feil ved kjøring av preparasjon: " << sqlite3_errmsg(db);
        sqlite3_close_v2(db);
        return;
    }
}


// funksjon til å håndtere start preparasjon
void start_prep(sqlite3 *db, sqlite3_stmt *&pSmt) {
    prep_hjelper(db, pSmt, "CREATE TABLE IF NOT EXISTS Utstyr (id INTEGER PRIMARY KEY, navn TEXT, beskrivelse TEXT, tilstand TEXT, lånt_av TEXT, dato_utlånt TEXT)");
}


// funksjon til å håndtere henting
void hent(int &id, sqlite3 *db, sqlite3_stmt *&pSmt) {
    cout << id_str;
    cin >> id;

    prep_hjelper(db, pSmt, "SELECT * FROM Utstyr WHERE id = ?");

    int id_for_hent = sqlite3_bind_int(
        pSmt,
        1,
        id
    );

    // håndtere ingen hent resultat og kjør henting av databasen
    if (sqlite3_step(pSmt) != SQLITE_ROW) {
        cout << "Ingen utstyr fant med id: " << id << endl;
        sqlite3_finalize(pSmt);
        return;
    }

    cout << endl << "Har er hva vi fant: " << endl << endl;

    auto navn = sqlite3_column_text(pSmt, 1);
    auto beskrivelse = sqlite3_column_text(pSmt, 2);
    auto tilstand = sqlite3_column_text(pSmt, 3);
    auto lånt_av = sqlite3_column_text(pSmt, 4);
    auto dato_utlånt = sqlite3_column_text(pSmt, 5);

    // logg resultatet
    cout << "Utstyrs navn: " << navn << endl;
    cout << "Beskrivelse: " << beskrivelse << endl;
    cout << "Utstyrs tilstand: " << tilstand << endl;
    cout << "Lånt av: " << lånt_av << endl;
    cout << "Dato utlånt: " << dato_utlånt << endl << endl;

    // fjern ferdig gjort henting av databasen
    sqlite3_finalize(pSmt);
}


// funksjon til å håndtere endring preparasjon
void endre_prep(sqlite3 *db, sqlite3_stmt *&pSmt) {
    prep_hjelper(db, pSmt, "UPDATE Utstyr SET navn = ?, beskrivelse = ?, tilstand = ?, lånt_av = ?, dato_utlånt = ? WHERE id = ?");
}


// funksjon til å håndtere laging preparasjon
void legge_til_prep(sqlite3 *db, sqlite3_stmt *&pSmt) {
    prep_hjelper(db, pSmt, "INSERT INTO Utstyr (navn, beskrivelse, tilstand, lånt_av, dato_utlånt) VALUES (?, ?, ?, ?, ?)");
}


// funksjon til å håndtere fjerning preparasjon
void slett_prep(sqlite3 *db, sqlite3_stmt *&pSmt) {
    prep_hjelper(db, pSmt, "DELETE FROM Utstyr WHERE id = ?");
}


// funksjon til å håndtere input-er og output-er (ingen spacing)
void inp_og_out(sqlite3 *db, string out_str, string &in_str, bool ignorer, bool få_linje) {
    // håndtere ugyldig måte – både
    if (ignorer && få_linje) {
        cerr << "Kan ikke være både getline() og cin.ignore()!" << endl;
        sqlite3_close_v2(db);
        return;
    // håndtere ugyldig måte – ingen
    } else if (!ignorer && !få_linje) {
        cerr << "Kan ikke være ingen måte!" << endl;
        sqlite3_close_v2(db);
        return;
    }

    // output
    cout << out_str;

    // input
    if (ignorer) {
        cin >> in_str;
        cin.ignore(); // unngår input "skip"

        // håndtere tom input
        if (in_str.empty()) {
            cout << "Feltet kan ikke være tom!" << endl;
            sqlite3_close_v2(db);
            return;
        }

        return;
    } else if (få_linje) {
        getline(cin, in_str); // få hele linjen

        // håndtere tom input
        if (in_str.empty()) {
            cout << "Feltet kan ikke være tom!" << endl;
            sqlite3_close_v2(db);
            return;
        }

        return;
    }
}


int main() {
    // lag instance av utstyr informasjonen
    utstyr_info dataen;

    sqlite3 *db;
    sqlite3_stmt *pSmt = nullptr; // unngå random krasjer

    // lag en ny kobling til databasen
    int åpen = sqlite3_open_v2 (
        "utstyr.db",
        &db, // koble til database filen
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
        NULL // ingen VFS modul
    );

    // håndtere sqlite feil på open
    if (åpen != SQLITE_OK) {
        cerr << "Feil ved åpning: " << sqlite3_errmsg(db);
        sqlite3_finalize(pSmt);
        return 1;
    }

    // start preparasjon
    start_prep(db, pSmt);
    // kjør start preperasjon
    int start_step = sqlite3_step(pSmt);

    // håndtere feil ved kjøring av start step
    if (start_step != SQLITE_DONE) {
        cerr << "Feil ved kjøring av start: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(pSmt);
        return 1;
    }

    // fjern ferdig gjort start preparasjon
    sqlite3_finalize(pSmt);

    inp_og_out(db, "Hent utstyr? (ja / nei) ", dataen.hent_samtykke, true, false);

    // håndtere hent samtykke
    if (dataen.hent_samtykke == "ja") {
        hent(dataen.id, db, pSmt);

        // slutt programmet
        sqlite3_close_v2(db);
        cout << "OK!";
        return 0;
    }

    inp_og_out(db, "Endre utstyr? (ja / nei) ", dataen.endre_samtykke, true, false);
    
    // håndtere endre samtykke
    if (dataen.endre_samtykke == "ja") {
        // utstyrs id for identifikasjon
        cout << id_str;
        cin >> dataen.id;
        cin.ignore();
        // endre utstyrs navn
        inp_og_out(db, "Endre utstyrs navn: ", dataen.navn, false, true);
        // endre beskrivelse
        inp_og_out(db, "Endre beskrivelse: ", dataen.beskrivelse, false, true);
        // endre tilstand
        inp_og_out(db, "Endre tilstand: ", dataen.tilstand, false, true);
        // endre lånt av
        inp_og_out(db, "Endre lånt av: ", dataen.lånt_av, false, true);
        // endre dato utlånt
        inp_og_out(db, "Endre dato utlånt: ", dataen.dato_utlånt, false, true);
        // utstyr endring
        endre_prep(db, pSmt);

        int endre_navn = sqlite3_bind_text(
            pSmt,
            1,
            dataen.navn.c_str(),
            -1, // kjør til null skjer
            SQLITE_TRANSIENT
        );

        int endre_beskrivelse = sqlite3_bind_text(
            pSmt,
            2,
            dataen.beskrivelse.c_str(),
            -1, // kjør til null skjer
            SQLITE_TRANSIENT
        );

        int endre_tilstand = sqlite3_bind_text(
            pSmt,
            3,
            dataen.tilstand.c_str(),
            -1, // kjør til null skjer
            SQLITE_TRANSIENT
        );

        int endre_lånt_av = sqlite3_bind_text(
            pSmt,
            4,
            dataen.lånt_av.c_str(),
            -1, // kjør til null skjer
            SQLITE_TRANSIENT
        );

        int endre_dato_utlånt = sqlite3_bind_text(
            pSmt,
            5,
            dataen.dato_utlånt.c_str(),
            -1, // kjør til null skjer
            SQLITE_TRANSIENT
        );

        int id_for_endre = sqlite3_bind_int(
            pSmt,
            6,
            dataen.id
        );

        // kjør endring av databasen
        int endre_step = sqlite3_step(pSmt);

        // håndtere feil ved kjøring av endre step
        if (endre_step != SQLITE_DONE) {
            cerr << "Feil ved kjøring av endre: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(pSmt);
            return 1;
        }

        // fjern ferdig lagde endring av databasen
        sqlite3_finalize(pSmt);
        // slutt programmet
        sqlite3_close_v2(db);
        cout << "OK!";
        return 0;
    }

    inp_og_out(db, "Lag utstyr? (ja / nei) ", dataen.lag_samtykke, true, false);
    
    // håndtere lag samtykke
    if (dataen.lag_samtykke == "ja") {
        // lag utstyrs navn
        inp_og_out(db, "Lag utstyrs navn: ", dataen.navn, false, true);
        // lag beskrivelse
        inp_og_out(db, "Lag beskrivelse: ", dataen.beskrivelse, false, true);
        // lag tilstand
        inp_og_out(db, "Lag tilstand: ", dataen.tilstand, false, true);
        // lag lånt av
        inp_og_out(db, "Lag lånt av: ", dataen.lånt_av, false, true);
        // lag dato utlånt
        inp_og_out(db, "Lag dato utlånt: ", dataen.dato_utlånt, false, true);
        // utstyr laging
        legge_til_prep(db, pSmt);

        int lag_navn = sqlite3_bind_text(
            pSmt,
            1,
            dataen.navn.c_str(),
            -1, // kjør til null skjer
            SQLITE_TRANSIENT
        );

        int lag_beskrivelse = sqlite3_bind_text(
            pSmt,
            2,
            dataen.beskrivelse.c_str(),
            -1, // kjør til null skjer
            SQLITE_TRANSIENT
        );

        int lag_tilstand = sqlite3_bind_text(
            pSmt,
            3,
            dataen.tilstand.c_str(),
            -1, // kjør til null skjer
            SQLITE_TRANSIENT
        );

        int lag_lånt_av = sqlite3_bind_text(
            pSmt,
            4,
            dataen.lånt_av.c_str(),
            -1, // kjør til null skjer
            SQLITE_TRANSIENT
        );

        int lag_dato_utlånt = sqlite3_bind_text(
            pSmt,
            5,
            dataen.dato_utlånt.c_str(),
            -1, // kjør til null skjer
            SQLITE_TRANSIENT
        );

        // kjør laging av databasen
        int lag_step = sqlite3_step(pSmt);

        // håndtere feil ved kjøring av lag step
        if (lag_step != SQLITE_DONE) {
            cerr << "Feil ved kjøring av lag: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(pSmt);
            return 1;
        }

        // fjern ferdig lagde laging av databasen
        sqlite3_finalize(pSmt);
        // slutt programmet
        sqlite3_close_v2(db);
        cout << "OK!";
        return 0;
    }
    
    inp_og_out(db, "Slett utstyr? (ja / nei) ", dataen.slett_samtykke, true, false);

    // håndtere slett samtykke
    if (dataen.slett_samtykke == "ja") {
        // slett utstyr
        cout << "Slett utstyret med id: ";
        cin >> dataen.id;
        // utstyr fjerning
        slett_prep(db, pSmt);

        int id_for_slett = sqlite3_bind_int(
            pSmt,
            1,
            dataen.id
        );

        // kjør fjerning av databasen
        int slett_step = sqlite3_step(pSmt);

        // håndtere feil ved kjøring av slett step
        if (slett_step != SQLITE_DONE) {
            cerr << "Feil ved kjøring av slett: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(pSmt);
            return 1;
        }

        // fjern ferdig gjort fjerning av databasen
        sqlite3_finalize(pSmt);
    }

    // slutt programmet
    sqlite3_close_v2(db);
    cout << "OK!";
    return 0;
}