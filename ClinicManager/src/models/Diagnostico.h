#pragma once
#include <string>

struct Diagnostico {
    std::string codigo;       // ICD-10 style code like "I21.0"
    std::string nombre;
    std::string categoria;    // Medical area (e.g. "Medicina Interna")
    std::string subcategoria; // Specialty (e.g. "Cardiología")
    std::string descripcion;

    bool operator==(const Diagnostico& other) const {
        return codigo == other.codigo;
    }
};
