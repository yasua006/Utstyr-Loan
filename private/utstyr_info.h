#ifndef UTSTYR_INFO
#define UTSTYR_INFO

#include <string>

using std::string;

// assign utstyr informasjon
struct utstyr_info {
    string bruker_navn;
    string bruker_passord;
    string admin_navn;
    string admin_passord;
    int id;
    string navn;
    string beskrivelse;
    string tilstand;
    string lånt_av;
    string dato_utlånt;
};
#endif