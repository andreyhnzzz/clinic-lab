#pragma once
#include <string>

struct Consulta {
    std::string idConsulta;
    std::string cedulaPaciente;
    std::string fecha;        // "YYYY-MM-DD"
    std::string medicoTratante;
    std::string diagnostico;
    std::string codigoDiagnostico;  // ICD-10 code from diagnosis tree
    std::string areaDiagnostico;    // Medical area
    std::string especialidadDiagnostico; // Specialty
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
