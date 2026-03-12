#pragma once
#include <string>

struct Paciente {
    std::string cedula;
    std::string nombre;
    int edad = 0;
    std::string fechaRegistro;
    int prioridad = 3;   // 1=crítico, 2=urgente, 3=normal
    std::string tipoSangre;
    std::string diagnostico;
    std::string telefono;
    std::string canton;

    bool operator<(const Paciente& other) const {
        return prioridad < other.prioridad;
    }
    bool operator>(const Paciente& other) const {
        return prioridad > other.prioridad;
    }
    bool operator==(const Paciente& other) const {
        return cedula == other.cedula;
    }
};
