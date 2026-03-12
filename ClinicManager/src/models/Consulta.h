#pragma once
#include <string>

struct Consulta {
    std::string idConsulta;
    std::string cedulaPaciente;
    std::string fecha;        // "YYYY-MM-DD"
    std::string medicoTratante;
    std::string diagnostico;
    int gravedad = 1;         // 1-5
    double costo = 0.0;
    std::string notas;

    bool operator<(const Consulta& other) const {
        return fecha < other.fecha;
    }
    bool operator==(const Consulta& other) const {
        return idConsulta == other.idConsulta;
    }
};
