// få både input og output funksjonene
#include <iostream>
// få sqlite3
#include <sqlite3.h>
// bruk headeren for å få utstyr info
#include "utstyr_info.h"

#include "crow.h"

// mindre kopi lim inn
using std::cerr;
using std::endl;


// funksjon for assistering av preparasjoner
int prep_hjelper(sqlite3 *db, sqlite3_stmt *&pSmt, const char *zSQL) {
    // lag preparasjon
    int db_prep = sqlite3_prepare_v2 (
        db,
        zSQL,
        -1, // kjør til null skjer
        &pSmt, // koble til databasen
        NULL
    );

    // håndtere sqlite feil for preperasjon
    if (db_prep != SQLITE_OK) {
        cerr << "Feil ved kjøring av preparasjon: " << sqlite3_errmsg(db);
        sqlite3_finalize(pSmt);
        pSmt = nullptr;
        return 1;
    }

    return 0;
}


// funksjon til å håndtere start utstyr preparasjon
int start_utstyr_prep(sqlite3 *db, sqlite3_stmt *&pSmt) {
    return prep_hjelper(db, pSmt, "CREATE TABLE IF NOT EXISTS Utstyr (id INTEGER PRIMARY KEY, navn TEXT, beskrivelse TEXT, tilstand TEXT, lånt_av TEXT, dato_utlånt TEXT)");
}

// funksjon til å håndtere start konto preparasjon
int start_konto_prep(sqlite3 *db, sqlite3_stmt *&pSmt) {
    return prep_hjelper(db, pSmt, "CREATE TABLE IF NOT EXISTS Konto (navn TEXT, passord TEXT, er_admin INTEGER, UNIQUE (navn, passord))");
}

// funksjon til å håndtere laging preparasjon
int legge_til_prep(sqlite3 *db, sqlite3_stmt *&pSmt) {
    return prep_hjelper(db, pSmt, "INSERT INTO Utstyr (navn, beskrivelse, tilstand, lånt_av, dato_utlånt) VALUES (?, ?, ?, ?, ?)");
}

// funksjon til å håndtere endre preparasjon
int endre_til_prep(sqlite3 *db, sqlite3_stmt *&pSmt) {
    return prep_hjelper(db, pSmt, "UPDATE Utstyr SET navn = ?, beskrivelse = ?, tilstand = ?, lånt_av = ?, dato_utlånt = ? WHERE id = ?");
}

// funksjon til å håndtere admin logg inn insert preparasjon
int logge_inn_insert_prep(sqlite3 *db, sqlite3_stmt *&pSmt) {
    return prep_hjelper(db, pSmt, "INSERT INTO Konto (navn, passord, er_admin) VALUES (?, ?, ?)");
}


// funksjon til å håndtere legge kjøring
int legge_bind_step(sqlite3 *db, sqlite3_stmt *&pSmt, const utstyr_info& dataen) {
    // bind navn
    sqlite3_bind_text(
    pSmt,
    1,
    dataen.navn.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // bind beskrivelse
    sqlite3_bind_text(
    pSmt,
    2,
    dataen.beskrivelse.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // bind tilstand
    sqlite3_bind_text(
    pSmt,
    3,
    dataen.tilstand.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // bind lånt av
    sqlite3_bind_text(
    pSmt,
    4,
    dataen.lånt_av.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // bind dato utlånt
    sqlite3_bind_text(
    pSmt,
    5,
    dataen.dato_utlånt.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // kjøre endringen
    int step_resultat = sqlite3_step(pSmt);

    // håndtere kjøre feil for legging
    if (step_resultat != SQLITE_DONE) {
        cerr << "Feil ved kjøring av legge:" << sqlite3_errmsg(db) << endl;
        // fjern ferdig kjøring av legge
        sqlite3_finalize(pSmt);
        pSmt = nullptr;
        return 1;
    }

    // fjern ferdig kjøring av legge
    sqlite3_finalize(pSmt);
    pSmt = nullptr;
    return 0;
}


// funksjon til å håndtere endre kjøring
int endre_til_bind_step(sqlite3 *db, sqlite3_stmt *&pSmt, const utstyr_info& dataen) {
    // håndtere ugyldig id-er
    if (dataen.id <= 0) {
        cerr << "Ugyldig id!" << endl;
        return 1;
    }

    // bind navn
    sqlite3_bind_text(
    pSmt,
    1,
    dataen.navn.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // bind beskrivelse
    sqlite3_bind_text(
    pSmt,
    2,
    dataen.beskrivelse.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // bind tilstand
    sqlite3_bind_text(
    pSmt,
    3,
    dataen.tilstand.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // bind lånt av
    sqlite3_bind_text(
    pSmt,
    4,
    dataen.lånt_av.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // bind dato utlånt
    sqlite3_bind_text(
    pSmt,
    5,
    dataen.dato_utlånt.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // bind id
    sqlite3_bind_int(pSmt, 6, dataen.id);

    // kjøre endringen
    int step_resultat = sqlite3_step(pSmt);

    // håndtere kjøre feil for endring
    if (step_resultat != SQLITE_DONE) {
        cerr << "Feil ved kjøring av endre:" << sqlite3_errmsg(db) << endl;
        // fjern ferdig kjøring av endre
        sqlite3_finalize(pSmt);
        pSmt = nullptr;
        return 1;
    }

    // fjern ferdig kjøring av endre
    sqlite3_finalize(pSmt);
    pSmt = nullptr;
    return 0;
}

// funksjon til å håndtere logg inn kjøring
int logge_inn_bind_step(sqlite3 *db, sqlite3_stmt *&pSmt, const utstyr_info& dataen) {
    // bind navn
    sqlite3_bind_text(
    pSmt,
    1,
    dataen.admin_navn.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // bind passord
    sqlite3_bind_text(
    pSmt,
    2,
    dataen.admin_passord.c_str(),
    -1,
    SQLITE_TRANSIENT
    );

    // bind er admin
    sqlite3_bind_int(pSmt, 3, dataen.er_admin);

    // kjøre insert
    int step_resultat = sqlite3_step(pSmt);

    // håndtere kjøre feil for insert
    if (step_resultat != SQLITE_DONE) {
        cerr << "Feil ved kjøring av logg inn:" << sqlite3_errmsg(db) << endl;
        // fjern ferdig kjøring av logg inn
        sqlite3_finalize(pSmt);
        pSmt = nullptr;
        return 1;
    }

    // fjern ferdig kjøring av logg inn
    sqlite3_finalize(pSmt);
    pSmt = nullptr;
    return 0;
}


// funksjon til å håndtere henting av utstyr navn for oversikt
int hent_navner(sqlite3 *db, sqlite3_stmt *&pSmt, crow::mustache::context &info) {
    prep_hjelper(db, pSmt, "SELECT id, navn FROM Utstyr");

    try {
        int index = 0; // brukt for increment

        // løkke til å gå gjennom alle rad med navn
        while (sqlite3_step(pSmt) == SQLITE_ROW) {
            crow::mustache::context rad;
            // vis resultatet
            rad["id"] = sqlite3_column_int(pSmt, 0);
            // bytt bool til string
            rad["navn"] = reinterpret_cast<const char*>(sqlite3_column_text(pSmt, 1));

            // increment med 1
            info["utstyr"][index++] = std::move(rad);
        }

        // fjern ferdig gjort henting av databasens navn radene
        sqlite3_finalize(pSmt);
        pSmt = nullptr;
        return 0;
    } catch (int err) {
        cerr << "Feil ved henting av navner:" << err << endl;
        // fjern ferdig gjort henting av databasens navn radene
        sqlite3_finalize(pSmt);
        pSmt = nullptr;
        return 1;
    }
}


// funksjon til å håndtere henting av alt utstyr for detalj
int hent_utstyr(sqlite3 *db, sqlite3_stmt *&pSmt, crow::mustache::context &info, int &id) {
    // håndtere ugyldig id-er
    if (id <= 0) {
        cerr << "Ugyldig id!" << endl;
        return 1;
    }

    prep_hjelper(db, pSmt, "SELECT * FROM Utstyr WHERE id = ?");

    sqlite3_bind_int(pSmt, 1, id);

    try {
        // løkke til å gå gjennom alle rad, utenom id-ene
        if (sqlite3_step(pSmt) == SQLITE_ROW) {
            // vis resultatet, men bytt bool til string
            info["navn"] = reinterpret_cast<const char*>(sqlite3_column_text(pSmt, 1));
            info["beskrivelse"] = reinterpret_cast<const char*>(sqlite3_column_text(pSmt, 2));
            info["tilstand"] = reinterpret_cast<const char*>(sqlite3_column_text(pSmt, 3));
            info["lånt av"] = reinterpret_cast<const char*>(sqlite3_column_text(pSmt, 4));
            info["dato utlånt"] = reinterpret_cast<const char*>(sqlite3_column_text(pSmt, 5));
        } else {
            cerr << "Ingen rader fant!" << endl;
            throw (info);
        }

        // fjern ferdig gjort henting av databasens radene
        sqlite3_finalize(pSmt);
        pSmt = nullptr;
        return 0;
    } catch (int err) {
        cerr << "Feil ved henting av utstyret:" << err << endl;
        // fjern ferdig gjort henting av databasens radene
        sqlite3_finalize(pSmt);
        pSmt = nullptr;
        return 1;
    }
}


int main() {
    // lag instance av utstyr informasjonen
    utstyr_info dataen;

    // init id-en
    dataen.id = 0;

    // init pointers
    sqlite3 *db = nullptr;
    sqlite3_stmt *pSmt = nullptr;

    // lag en ny kobling til databasen
    int åpen = sqlite3_open_v2 (
        "utstyr.db",
        &db, // koble til databasens pointer
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
        NULL // ingen VFS modul
    );

    // håndtere sqlite feil på open
    if (åpen != SQLITE_OK) {
        cerr << "Feil ved åpning: " << sqlite3_errmsg(db);
        sqlite3_close_v2(db);
        return 1;
    }

    // start utstyr preparasjon
    start_utstyr_prep(db, pSmt);
    // kjør start utstyr preperasjon
    int start_utstyr_step = sqlite3_step(pSmt);


    // håndtere feil ved kjøring av start step
    if (start_utstyr_step != SQLITE_DONE || start_utstyr_step == 1) {
        cerr << "Feil ved kjøring av start utstyr: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(pSmt);
        pSmt = nullptr;
        return 1;
    }

    // fjern ferdig gjort start utstyr preparasjon
    sqlite3_finalize(pSmt);
    pSmt = nullptr;

    // start konto preparasjon
    start_konto_prep(db, pSmt);
    // kjør start konto preperasjon
    int start_konto_step = sqlite3_step(pSmt);

    // håndtere feil ved kjøring av start step
    if (start_konto_step != SQLITE_DONE || start_konto_step == 1) {
        cerr << "Feil ved kjøring av start konto: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(pSmt);
        pSmt = nullptr;
        return 1;
    }

    // fjern ferdig gjort start konto preparasjon
    sqlite3_finalize(pSmt);
    pSmt = nullptr;

    // lag appen
    crow::SimpleApp app;

    crow::mustache::set_global_base("public");

    CROW_ROUTE(app, "/").methods("GET"_method)([](){
        auto side = crow::mustache::load("index.html");
        return side.render();
    });

    // detaljside for hvert utstyr
    CROW_ROUTE(app, "/detalj-side/<int>").methods("GET"_method)([&](int id){
        crow::mustache::context info;

        // håndtere feil ved henting av utstyret
        if (hent_utstyr(db, pSmt, info, id) == 1) {
            cerr << "400: Henting" << endl;
        }

        auto side = crow::mustache::load("detalj-side.html");
        return side.render(info);
    });

    // oversikt over alt utstyr
    CROW_ROUTE(app, "/oversikt").methods("GET"_method)([&](){
        crow::mustache::context info;

        // håndtere feil ved henting av navner
        if (hent_navner(db, pSmt, info) == 1) {
            cerr << "400: Henting" << endl;
        }

        auto side = crow::mustache::load("oversikt.html");
        return side.render(info);
    });

    CROW_ROUTE(app, "/skjema/legge").methods("GET"_method)([](){
        auto side = crow::mustache::load("skjema/legge.html");
        return side.render();
    });

    CROW_ROUTE(app, "/skjema/endre").methods("GET"_method)([](){
        auto side = crow::mustache::load("skjema/endre.html");
        return side.render();
    });

    CROW_ROUTE(app, "/skjema/logg-inn").methods("GET"_method)([](){
        auto side = crow::mustache::load("skjema/logg-inn.html");
        return side.render();
    });

    // POST request for registrering av utstyr (admin tilgang)
    CROW_ROUTE(app, "/skjema/legge/send").methods("POST"_method)([&](const crow::request& req){
        // ikke en admin, uautorisert tilgang
        if (!dataen.er_admin) {
            return crow::response(403);
        }

        auto body_resultat = req.get_body_params();

        try {
            auto navn = body_resultat.get("navn");
            auto beskrivelse = body_resultat.get("beskrivelse");
            auto tilstand = body_resultat.get("tilstand");
            auto lånt_av = body_resultat.get("lont-av"); // UTF-8 grunner
            auto dato_utlånt = body_resultat.get("dato-utlont");

            // håndtere query strings som finnes ikke
            if (!navn || !beskrivelse || !tilstand || !lånt_av || !dato_utlånt) {
                return crow::response(400, "Kunne ikke legge radene!");
            }

            dataen.navn = navn;
            dataen.beskrivelse = beskrivelse;
            dataen.tilstand = tilstand;
            dataen.lånt_av = lånt_av;
            dataen.dato_utlånt = dato_utlånt;

            legge_til_prep(db, pSmt);
            legge_bind_step(db, pSmt, dataen);

            return crow::response(200, "Legge til: OK!");
        } catch (int err) {
            return crow::response(400, std::to_string(err));
        }
    });

    // PUT request for redigering av utstyr (admin tilgang)
    CROW_ROUTE(app, "/skjema/endre/send").methods("POST"_method)([&](const crow::request &req){
        // ikke en admin, uautorisert tilgang
        if (!dataen.er_admin) {
            return crow::response(403);
        }

        auto body_resultat = req.get_body_params();

        try {
            auto id_str = body_resultat.get("id");
            auto navn = body_resultat.get("navn");
            auto beskrivelse = body_resultat.get("beskrivelse");
            auto tilstand = body_resultat.get("tilstand");
            auto lånt_av = body_resultat.get("lont-av");
            auto dato_utlånt = body_resultat.get("dato-utlont");

            // håndtere query strings som finnes ikke
            if (!id_str || !navn || !beskrivelse || !tilstand || !lånt_av || !dato_utlånt) {
                return crow::response(400, "Kunne ikke endre radene!");
            }

            dataen.id = std::stoi(id_str);
            dataen.navn = navn;
            dataen.beskrivelse = beskrivelse;
            dataen.tilstand = tilstand;
            dataen.lånt_av = lånt_av;
            dataen.dato_utlånt = dato_utlånt;

            endre_til_prep(db, pSmt);
            endre_til_bind_step(db, pSmt, dataen);

            return crow::response(200, "Endre til: OK!");
        } catch (int err) {
            return crow::response(400, std::to_string(err));
        }
    });

    // PUT request for å logge inn (sjekk om admin tilgang)
    CROW_ROUTE(app, "/skjema/logg-inn/send").methods("POST"_method)([&](const crow::request &req){
        auto body_resultat = req.get_body_params();

        try {
            auto navn = body_resultat.get("navn");
            auto passord = body_resultat.get("passord");

            // håndtere query strings som finnes ikke
            if (!navn || !passord) {
                return crow::response(400, "Kunne ikke logge inn!");
            }

            // sjekk om brukeren skrev riktig admin navn og passord
            if (navn == dataen.admin_navn && passord == dataen.admin_passord) {
                dataen.admin_navn = navn;
                dataen.admin_passord = passord;
                dataen.er_admin = 1;
            // ikke en admin
            } else {
                return crow::response(403);
            }

            logge_inn_insert_prep(db, pSmt);
            logge_inn_bind_step(db, pSmt, dataen);

            return crow::response(200, "Logg inn: OK!");
        } catch (int err) {
            return crow::response(400, std::to_string(err));
        }
    });

    app.port(18080).run();

    // slutt programmet
    sqlite3_close_v2(db);
    return 0;
}