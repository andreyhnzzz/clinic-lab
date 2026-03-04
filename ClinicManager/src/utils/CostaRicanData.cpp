#include "CostaRicanData.h"

namespace CostaRicanData
{

// ── Cédula generation ─────────────────────────────────────────────────────────
// Costa Rican national ID: province(1) + block(3) + individual(5) = 9 digits.

QString randomCedula(int seed)
{
    int province   = (qAbs(seed)        % 9) + 1;      // 1-9
    int block      = (qAbs(seed / 10)   % 999) + 1;    // 001-999
    int individual = (qAbs(seed * 7919) % 99999) + 1;  // 00001-99999
    return QString("%1%2%3")
        .arg(province)
        .arg(block,      3, 10, QLatin1Char('0'))
        .arg(individual, 5, 10, QLatin1Char('0'));
}

// ── Name pools ────────────────────────────────────────────────────────────────

const QStringList &firstNamesMale()
{
    static const QStringList names = {
        "Andrés", "Carlos", "Daniel", "Eduardo", "Felipe",
        "Gabriel", "Humberto", "Isaac", "Javier", "Kevin",
        "Luis", "Mario", "Nicolás", "Oscar", "Pablo",
        "Rafael", "Santiago", "Tomás", "Ulises", "Víctor",
        "Walter", "Xavier", "Yesid", "Zacarías", "Mauricio",
        "Rodrigo", "Alejandro", "Fernando", "Gustavo", "Hernán",
        "Joel", "Leonardo", "Marco", "Néstor", "Orlando",
        "Pedro", "Raúl", "Sergio", "Tadeo", "Uriel"
    };
    return names;
}

const QStringList &firstNamesFemale()
{
    static const QStringList names = {
        "Andrea", "Beatriz", "Camila", "Diana", "Elena",
        "Fernanda", "Gabriela", "Helena", "Isabel", "Jennifer",
        "Karen", "Laura", "María", "Natalia", "Olga",
        "Patricia", "Rebeca", "Sandra", "Tatiana", "Úrsula",
        "Valeria", "Wendy", "Ximena", "Yolanda", "Zelenia",
        "Adriana", "Brenda", "Carolina", "Daniela", "Estefanía",
        "Flor", "Gloria", "Hannah", "Irene", "Juliana",
        "Karina", "Lorena", "Melissa", "Nora", "Paola"
    };
    return names;
}

const QStringList &lastNames()
{
    static const QStringList names = {
        "Acosta", "Araya", "Barrantes", "Brenes", "Cascante",
        "Chacón", "Cordero", "Díaz", "Elizondo", "Fonseca",
        "García", "Hernández", "Jiménez", "Leiva", "López",
        "Madrigal", "Mora", "Montero", "Núñez", "Obando",
        "Pereira", "Quesada", "Ramírez", "Rodríguez", "Rojas",
        "Salazar", "Solano", "Solís", "Torres", "Umaña",
        "Vargas", "Vindas", "Zamora", "Zúñiga", "Arias",
        "Badilla", "Castro", "Durán", "Espinoza", "Flores",
        "González", "Gutiérrez", "Herrera", "Hidalgo", "Leal",
        "Méndez", "Miranda", "Murillo", "Navarro", "Ortega"
    };
    return names;
}

const QStringList &doctorNames()
{
    static const QStringList names = {
        "Dr. Alberto Mora",     "Dra. Ana Vargas",      "Dr. Bernardo Quesada",
        "Dra. Carmen Solís",    "Dr. Diego Araya",      "Dra. Elena Fonseca",
        "Dr. Francisco Rojas",  "Dra. Gloria Jiménez",  "Dr. Hugo Cascante",
        "Dra. Irene Madrigal",  "Dr. Javier Torres",    "Dra. Karen Brenes",
        "Dr. Leonel Chacón",    "Dra. Mariela Herrera", "Dr. Nicolás Rodríguez",
        "Dra. Olga Salazar",    "Dr. Pablo Espinoza",   "Dra. Rosa Méndez",
        "Dr. Samuel Gutiérrez", "Dra. Tatiana Arias"
    };
    return names;
}

// ── Blood types (approximate Costa Rican distribution) ───────────────────────

const QStringList &bloodTypes()
{
    // Weighted list: O+ most common (~38%), A+ (~31%), B+ (~9%), AB+ (~3%), negatives rarer
    static const QStringList types = {
        "O+",  "O+",  "O+",  "O+",  "O+",  "O+",  "O+",  "O+",
        "A+",  "A+",  "A+",  "A+",  "A+",  "A+",
        "B+",  "B+",
        "AB+",
        "O-",  "O-",
        "A-",
        "B-",
        "AB-"
    };
    return types;
}

// ── Common diagnoses ──────────────────────────────────────────────────────────

const QStringList &commonDiagnoses()
{
    static const QStringList diags = {
        "Hipertensión arterial",         "Diabetes mellitus tipo 2",
        "Infección respiratoria aguda",  "Gastroenteritis aguda",
        "Insuficiencia cardíaca",        "Fractura de radio",
        "Lumbalgia crónica",             "Cefalea tensional",
        "Rinitis alérgica",              "Bronquitis aguda",
        "Otitis media",                  "Faringitis estreptocócica",
        "Urticaria",                     "Dermatitis atópica",
        "Anemia ferropénica",            "Hipotiroidismo",
        "Asma bronquial",                "Enfermedad por reflujo",
        "Infección urinaria",            "Ansiedad generalizada",
        "Depresión mayor",               "Migraña",
        "Artrosis de rodilla",           "Gota",
        "Conjuntivitis viral",           "Hernia inguinal",
        "Cálculo renal",                 "Colitis",
        "Hepatitis A",                   "COVID-19"
    };
    return diags;
}

// ── Medical areas (N-ary tree Level::Area) ────────────────────────────────────

const QStringList &medicalAreas()
{
    static const QStringList areas = {
        "Medicina Interna",
        "Cirugía",
        "Pediatría",
        "Ginecología y Obstetricia",
        "Psiquiatría y Salud Mental",
        "Urgencias y Emergencias"
    };
    return areas;
}

// ── Specialties per area (Level::Specialty) ───────────────────────────────────

const QList<QStringList> &specialtiesByArea()
{
    static const QList<QStringList> specs = {
        // Medicina Interna
        { "Cardiología", "Endocrinología", "Gastroenterología",
          "Neumología",  "Nefrología",     "Reumatología" },
        // Cirugía
        { "Cirugía General", "Ortopedia y Traumatología",
          "Neurocirugía",    "Cirugía Plástica" },
        // Pediatría
        { "Neonatología", "Pediatría General",
          "Cardiopediatría", "Pediatría Oncológica" },
        // Ginecología y Obstetricia
        { "Ginecología", "Obstetricia", "Medicina Materno-Fetal" },
        // Psiquiatría y Salud Mental
        { "Psiquiatría General", "Psicología Clínica",
          "Adicciones", "Psiquiatría Infantil" },
        // Urgencias y Emergencias
        { "Urgencias Generales", "Urgencias Pediátricas",
          "Trauma", "Reanimación" }
    };
    return specs;
}

// ── Diagnoses per specialty (Level::Diagnosis) ────────────────────────────────

const QList<QStringList> &diagnosesBySpecialty()
{
    static const QList<QStringList> diags = {
        // Cardiología
        { "Insuficiencia cardíaca congestiva", "Infarto agudo de miocardio",
          "Fibrilación auricular", "Hipertensión arterial esencial" },
        // Endocrinología
        { "Diabetes mellitus tipo 1", "Diabetes mellitus tipo 2",
          "Hipotiroidismo", "Síndrome de Cushing" },
        // Gastroenterología
        { "Enfermedad por reflujo gastroesofágico", "Úlcera péptica",
          "Colitis ulcerosa", "Enfermedad de Crohn" },
        // Neumología
        { "Asma bronquial", "EPOC", "Neumonía adquirida",
          "Tromboembolia pulmonar" },
        // Nefrología
        { "Enfermedad renal crónica", "Cálculo renal",
          "Glomerulonefritis", "Infección urinaria alta" },
        // Reumatología
        { "Artritis reumatoide", "Lupus eritematoso", "Gota", "Artrosis" },
        // Cirugía General
        { "Hernia inguinal", "Apendicitis aguda", "Colecistitis aguda",
          "Oclusión intestinal" },
        // Ortopedia
        { "Fractura de fémur", "Fractura de radio", "Lumbalgia crónica",
          "Lesión de menisco" },
        // Neurocirugía
        { "Hernia discal lumbar", "Hernia discal cervical",
          "Hidrocefalia", "Tumor cerebral" },
        // Cirugía Plástica
        { "Quemadura grado II", "Quemadura grado III",
          "Cicatriz queloidea", "Injerto cutáneo" },
        // Neonatología
        { "Prematuridad extrema", "Sepsis neonatal",
          "Ictericia neonatal", "Síndrome de distrés respiratorio" },
        // Pediatría General
        { "Bronquiolitis", "Otitis media aguda",
          "Gastroenteritis aguda", "Faringitis estreptocócica" },
        // Cardiopediatría
        { "Comunicación interventricular", "Comunicación interauricular",
          "Tetralogía de Fallot", "Conducto arterioso persistente" },
        // Pediatría Oncológica
        { "Leucemia linfoblástica aguda", "Linfoma de Hodgkin",
          "Tumor de Wilms", "Meduloblastoma" },
        // Ginecología
        { "Mioma uterino", "Endometriosis", "Ovario poliquístico",
          "Cáncer de cuello uterino" },
        // Obstetricia
        { "Embarazo normal", "Preeclampsia", "Diabetes gestacional",
          "Parto prematuro" },
        // Medicina Materno-Fetal
        { "Restricción del crecimiento intrauterino",
          "Incompetencia cervical", "Isoinmunización Rh",
          "Gestación múltiple" },
        // Psiquiatría General
        { "Depresión mayor", "Trastorno bipolar",
          "Esquizofrenia", "Trastorno de pánico" },
        // Psicología Clínica
        { "Ansiedad generalizada", "Fobia específica",
          "Trastorno obsesivo-compulsivo", "Trastorno de estrés postraumático" },
        // Adicciones
        { "Dependencia al alcohol", "Dependencia a opioides",
          "Consumo de sustancias psicoactivas", "Trastorno por juego" },
        // Psiquiatría Infantil
        { "TDAH", "Trastorno del espectro autista",
          "Trastorno de conducta", "Enuresis" },
        // Urgencias Generales
        { "Politraumatismo", "Intoxicación aguda",
          "Dolor torácico", "Crisis hipertensiva" },
        // Urgencias Pediátricas
        { "Convulsión febril", "Broncoespasmo severo",
          "Deshidratación grave", "Cuerpo extraño en vía aérea" },
        // Trauma
        { "Traumatismo craneoencefálico", "Fractura de pelvis",
          "Lesión esplénica", "Neumotórax traumático" },
        // Reanimación
        { "Paro cardiorrespiratorio", "Shock anafiláctico",
          "Shock séptico", "Insuficiencia respiratoria aguda" }
    };
    return diags;
}

} // namespace CostaRicanData
