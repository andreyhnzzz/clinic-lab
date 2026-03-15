#include "DataGenerator.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <set>

static std::mt19937& rng() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

static int randInt(int lo, int hi) {
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(rng());
}

static const QVector<QString> NOMBRES_MASCULINOS = {
    "Carlos", "Luis", "José", "Juan", "Andrés", "Diego", "Ricardo",
    "Miguel", "Alejandro", "Felipe", "Sebastián", "Eduardo", "Mauricio",
    "Josué", "Daniel", "Rafael", "Esteban", "Rodrigo", "Gabriel", "Óscar"
};

static const QVector<QString> NOMBRES_FEMENINOS = {
    "María", "Ana", "Laura", "Valeria", "Sofía", "Gabriela", "Andrea",
    "Patricia", "Carmen", "Daniela", "Marcela", "Isabel", "Paola",
    "Adriana", "Natalia", "Claudia", "Lorena", "Alejandra", "Diana", "Karina"
};

static const QVector<QString> APELLIDOS = {
    "Rodríguez", "González", "Hernández", "López", "Martínez", "Pérez",
    "Jiménez", "Ramírez", "Sánchez", "Castro", "Mora", "Vargas",
    "Brenes", "Solano", "Rojas", "Arias", "Chaves", "Méndez",
    "Alvarado", "Salas", "Quesada", "Trejos", "Ugalde", "Monge",
    "Elizondo", "Chinchilla", "Barrantes", "Badilla", "Fonseca", "Murillo"
};

static const QVector<QString> CANTONES = {
    "San José", "Alajuela", "Heredia", "Cartago", "Liberia",
    "Nicoya", "Puntarenas", "Limón", "Desamparados", "Tibás",
    "Moravia", "Escazú", "Santa Ana", "Curridabat", "La Unión"
};

static const QVector<QString> TIPOS_SANGRE = {
    "A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"
};

static const QVector<QString> MEDICOS = {
    "Dr. Carlos Solano", "Dra. Ana Jiménez", "Dr. Luis Ramírez",
    "Dra. Patricia Mora", "Dr. Eduardo Vargas", "Dra. Sofía Brenes",
    "Dr. Andrés Quesada", "Dra. Marcela Rojas", "Dr. Felipe Castro",
    "Dra. Isabel Chaves", "Dr. Rodrigo Arias", "Dra. Laura González"
};

static const QVector<QString> DIAGNOSTICOS_COMUNES = {
    "Hipertensión arterial", "Diabetes mellitus tipo 2", "Gastritis crónica",
    "Gripe estacional", "Faringitis aguda", "Lumbalgia",
    "Migraña", "Bronquitis aguda", "Hipotiroidismo", "Artritis",
    "Infección urinaria", "Anemia ferropénica", "Obesidad",
    "Ansiedad generalizada", "Depresión moderada", "Apendicitis aguda",
    "Colecistitis", "Hernia inguinal", "Esguince de tobillo", "Fractura de muñeca"
};

static const QVector<QString> NOTAS_CONSULTA = {
    "Paciente refiere mejoría con tratamiento previo.",
    "Se indica reposo relativo por 3 días.",
    "Se solicitan exámenes de laboratorio de control.",
    "Paciente con buen estado general.",
    "Se ajusta dosis de medicamento.",
    "Seguimiento en 2 semanas.",
    "Se refiere a especialista.",
    "Sin complicaciones aparentes.",
    "Paciente indica cumplimiento del tratamiento.",
    "Se indica hidratación oral abundante."
};

QString DataGenerator::generateCedula() {
    int provincia = randInt(1, 9);
    int numero1 = randInt(100, 9999);
    int numero2 = randInt(100, 9999);
    return QString("%1-%2-%3")
        .arg(provincia)
        .arg(numero1, 4, 10, QChar('0'))
        .arg(numero2, 4, 10, QChar('0'));
}

QString DataGenerator::randomCostaRicanName() {
    bool masculino = (randInt(0, 1) == 0);
    const auto& nombres = masculino ? NOMBRES_MASCULINOS : NOMBRES_FEMENINOS;
    QString nombre = nombres[randInt(0, nombres.size() - 1)];
    QString apellido1 = APELLIDOS[randInt(0, APELLIDOS.size() - 1)];
    QString apellido2 = APELLIDOS[randInt(0, APELLIDOS.size() - 1)];
    return nombre + " " + apellido1 + " " + apellido2;
}

QString DataGenerator::randomCanton() {
    return CANTONES[randInt(0, CANTONES.size() - 1)];
}

QString DataGenerator::randomTipoSangre() {
    return TIPOS_SANGRE[randInt(0, TIPOS_SANGRE.size() - 1)];
}

QString DataGenerator::randomTelefono() {
    // Costa Rican phone numbers start with 6, 7, 8
    static const int prefixes[] = {6, 7, 8};
    int prefix = prefixes[randInt(0, 2)];
    int suffix = randInt(0, 9999999);
    return QString("%1%2").arg(prefix).arg(suffix, 7, 10, QChar('0'));
}

QString DataGenerator::randomDate(int yearFrom, int yearTo) {
    int year = randInt(yearFrom, yearTo);
    int month = randInt(1, 12);
    int maxDay = 28;
    if (month == 1 || month == 3 || month == 5 || month == 7 ||
        month == 8 || month == 10 || month == 12) maxDay = 31;
    else if (month != 2) maxDay = 30;
    else if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) maxDay = 29;
    int day = randInt(1, maxDay);
    return QString("%1-%2-%3")
        .arg(year)
        .arg(month, 2, 10, QChar('0'))
        .arg(day, 2, 10, QChar('0'));
}

QString DataGenerator::randomMedico() {
    return MEDICOS[randInt(0, MEDICOS.size() - 1)];
}

QVector<Paciente> DataGenerator::generatePacientes(int count) {
    QVector<Paciente> pacientes;
    pacientes.reserve(count);
    std::set<std::string> usedCedulas;
    for (int i = 0; i < count; ++i) {
        Paciente p;
        // Guarantee unique cedulas
        std::string ced;
        do {
            ced = generateCedula().toStdString();
        } while (usedCedulas.count(ced));
        usedCedulas.insert(ced);
        p.cedula = ced;
        p.nombre = randomCostaRicanName().toStdString();
        p.edad = randInt(1, 90);
        p.fechaRegistro = randomDate().toStdString();
        p.prioridad = randInt(1, 3);
        p.tipoSangre = randomTipoSangre().toStdString();
        p.diagnostico = DIAGNOSTICOS_COMUNES[randInt(0, DIAGNOSTICOS_COMUNES.size()-1)].toStdString();
        p.telefono = randomTelefono().toStdString();
        p.canton = randomCanton().toStdString();
        pacientes.push_back(p);
    }
    return pacientes;
}

QVector<Consulta> DataGenerator::generateConsultas(const QVector<Paciente>& pacientes, int count) {
    QVector<Consulta> consultas;
    consultas.reserve(count);
    if (pacientes.isEmpty()) return consultas;

    for (int i = 0; i < count; ++i) {
        Consulta c;
        c.idConsulta = QString("CONS-%1").arg(i + 1, 6, 10, QChar('0')).toStdString();
        const Paciente& p = pacientes[randInt(0, pacientes.size() - 1)];
        c.cedulaPaciente = p.cedula;
        c.fecha = randomDate(2020, 2024).toStdString();
        c.medicoTratante = randomMedico().toStdString();
        c.diagnostico = DIAGNOSTICOS_COMUNES[randInt(0, DIAGNOSTICOS_COMUNES.size()-1)].toStdString();
        c.gravedad = randInt(1, 5);
        c.costo = 15000.0 + randInt(0, 1000) * 500.0;  // Costa Rican colones range
        c.notas = NOTAS_CONSULTA[randInt(0, NOTAS_CONSULTA.size()-1)].toStdString();
        consultas.push_back(c);
    }
    // Sort by date for binary search
    std::sort(consultas.begin(), consultas.end(),
              [](const Consulta& a, const Consulta& b){ return a.fecha < b.fecha; });
    return consultas;
}

QVector<Diagnostico> DataGenerator::generateDiagnosticos() {
    QVector<Diagnostico> diags;
    auto addDiag = [&](const char* code, const char* name, const char* cat, const char* sub, const char* desc) {
        Diagnostico d;
        d.codigo = code; d.nombre = name; d.categoria = cat;
        d.subcategoria = sub; d.descripcion = desc;
        diags.push_back(d);
    };
    addDiag("I10","Hipertensión arterial","Medicina Interna","Cardiología","Presión arterial elevada crónica");
    addDiag("E11","Diabetes mellitus tipo 2","Medicina Interna","Endocrinología","Trastorno metabólico crónico");
    addDiag("K29.7","Gastritis","Medicina Interna","Gastroenterología","Inflamación de la mucosa gástrica");
    addDiag("J18.9","Neumonía","Pediatría","Infectología Pediátrica","Infección pulmonar bacteriana o viral");
    addDiag("K35.8","Apendicitis aguda","Cirugía","Cirugía General","Inflamación aguda del apéndice");
    addDiag("I21.0","Infarto de miocardio","Medicina Interna","Cardiología","Necrosis del músculo cardíaco");
    addDiag("G43.9","Migraña","Neurología","Neurología General","Cefalea episódica severa");
    addDiag("M51.1","Hernia de disco","Ortopedia","Columna Vertebral","Protrusión del disco intervertebral");
    addDiag("F32.1","Depresión moderada","Psiquiatría","Psiquiatría General","Trastorno del ánimo");
    addDiag("O80","Parto normal","Ginecología","Obstetricia","Parto vaginal sin complicaciones");
    return diags;
}
