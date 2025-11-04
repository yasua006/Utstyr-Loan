#ifndef UTSTYR_INFO
#define UTSTYR_INFO

// assign utstyr informasjon
struct utstyr_info {
    std::string hent_samtykke;
    std::string endre_samtykke;
    std::string lag_samtykke;
    std::string slett_samtykke;
    int id;
    std::string navn;
    std::string beskrivelse;
    std::string tilstand;
    std::string lånt_av;
    std::string dato_utlånt;
};
#endif