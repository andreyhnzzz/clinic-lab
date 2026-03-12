#pragma once
#include <QVector>
#include <QString>
#include "../models/Paciente.h"
#include "../models/Consulta.h"
#include "../models/Diagnostico.h"

class DataGenerator {
public:
    static QVector<Paciente> generatePacientes(int count);
    static QVector<Consulta> generateConsultas(const QVector<Paciente>& pacientes, int count);
    static QVector<Diagnostico> generateDiagnosticos();

    static QString generateCedula();
    static QString randomCostaRicanName();
    static QString randomCanton();
    static QString randomTipoSangre();
    static QString randomTelefono();
    static QString randomDate(int yearFrom = 2020, int yearTo = 2024);
    static QString randomMedico();
};
