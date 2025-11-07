#ifndef UTSTYR_INFO
#define UTSTYR_INFO

#include <string>

using std::string;

// assign utstyr informasjon
struct utstyr_info {
    string admin_navn = "Utvikling";
    string admin_passord = "UBrukerDrift";
    int er_admin;
    int id;
    string navn;
    string beskrivelse;
    string tilstand;
    string lånt_av;
    string dato_utlånt;
};
#endif