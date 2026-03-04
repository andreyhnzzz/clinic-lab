#include "Patient.h"

#include <QJsonArray>
#include <stdexcept>

// ── Constructor ──────────────────────────────────────────────────────────────

Patient::Patient(QObject *parent)
    : QObject(parent) {}

Patient::Patient(const QString &cedula,
                 const QString &nombre,
                 int edad,
                 const QDate &fechaReg,
                 Priority prioridad,
                 BloodType tipoSangre,
                 const QString &diagnostico,
                 QObject *parent)
    : QObject(parent),
      m_cedula(cedula),
      m_nombre(nombre),
      m_edad(edad),
      m_fechaReg(fechaReg),
      m_prioridad(prioridad),
      m_tipoSangre(tipoSangre),
      m_diagnostico(diagnostico)
{}

Patient::Patient(const Patient &other)
    : QObject(other.parent()),
      m_cedula(other.m_cedula),
      m_nombre(other.m_nombre),
      m_edad(other.m_edad),
      m_fechaReg(other.m_fechaReg),
      m_prioridad(other.m_prioridad),
      m_tipoSangre(other.m_tipoSangre),
      m_diagnostico(other.m_diagnostico)
{}

Patient &Patient::operator=(const Patient &other)
{
    if (this == &other) return *this;
    m_cedula      = other.m_cedula;
    m_nombre      = other.m_nombre;
    m_edad        = other.m_edad;
    m_fechaReg    = other.m_fechaReg;
    m_prioridad   = other.m_prioridad;
    m_tipoSangre  = other.m_tipoSangre;
    m_diagnostico = other.m_diagnostico;
    return *this;
}

// ── Setters ──────────────────────────────────────────────────────────────────

void Patient::setCedula(const QString &v)      { if (m_cedula != v)      { m_cedula = v;      emit cedulaChanged(); } }
void Patient::setNombre(const QString &v)      { if (m_nombre != v)      { m_nombre = v;      emit nombreChanged(); } }
void Patient::setEdad(int v)                   { if (m_edad != v)        { m_edad = v;        emit edadChanged(); } }
void Patient::setFechaReg(const QDate &v)      { if (m_fechaReg != v)    { m_fechaReg = v;    emit fechaRegChanged(); } }
void Patient::setDiagnostico(const QString &v) { if (m_diagnostico != v) { m_diagnostico = v; emit diagnosticoChanged(); } }

void Patient::setPrioridad(int v)
{
    Priority p = static_cast<Priority>(v);
    if (m_prioridad != p) { m_prioridad = p; emit prioridadChanged(); }
}

void Patient::setPriorityEnum(Priority p)
{
    if (m_prioridad != p) { m_prioridad = p; emit prioridadChanged(); }
}

void Patient::setTipoSangre(const QString &v)
{
    BloodType bt = bloodTypeFromString(v);
    if (m_tipoSangre != bt) { m_tipoSangre = bt; emit tipoSangreChanged(); }
}

void Patient::setBloodTypeEnum(BloodType bt)
{
    if (m_tipoSangre != bt) { m_tipoSangre = bt; emit tipoSangreChanged(); }
}

// ── Getters ──────────────────────────────────────────────────────────────────

QString Patient::tipoSangre() const
{
    return bloodTypeLabel(m_tipoSangre);
}

// ── Validation ───────────────────────────────────────────────────────────────

bool Patient::isValidCedula(const QString &cedula)
{
    // Costa Rican national ID: exactly 9 digits
    if (cedula.length() != 9) return false;
    for (const QChar &c : cedula)
        if (!c.isDigit()) return false;

    // Basic range check: province digit 1-9
    int province = cedula.at(0).digitValue();
    if (province < 1 || province > 9) return false;

    return true;
}

bool Patient::isValid() const
{
    return isValidCedula(m_cedula)
        && !m_nombre.isEmpty()
        && m_edad >= 0 && m_edad <= 120
        && m_fechaReg.isValid();
}

// ── Comparison ───────────────────────────────────────────────────────────────

// Lower numeric priority value = higher urgency (1=critical beats 3=normal)
bool Patient::operator<(const Patient &other) const
{
    if (m_prioridad != other.m_prioridad)
        return static_cast<int>(m_prioridad) < static_cast<int>(other.m_prioridad);
    return m_fechaReg < other.m_fechaReg;
}

bool Patient::operator>(const Patient &other) const { return other < *this; }
bool Patient::operator==(const Patient &other) const { return m_cedula == other.m_cedula; }

// ── Serialization ─────────────────────────────────────────────────────────────

QJsonObject Patient::toJson() const
{
    QJsonObject obj;
    obj["cedula"]      = m_cedula;
    obj["nombre"]      = m_nombre;
    obj["edad"]        = m_edad;
    obj["fechaReg"]    = m_fechaReg.toString(Qt::ISODate);
    obj["prioridad"]   = static_cast<int>(m_prioridad);
    obj["tipoSangre"]  = bloodTypeLabel(m_tipoSangre);
    obj["diagnostico"] = m_diagnostico;
    return obj;
}

Patient Patient::fromJson(const QJsonObject &json, QObject *parent)
{
    Patient p(parent);
    p.m_cedula      = json["cedula"].toString();
    p.m_nombre      = json["nombre"].toString();
    p.m_edad        = json["edad"].toInt();
    p.m_fechaReg    = QDate::fromString(json["fechaReg"].toString(), Qt::ISODate);
    p.m_prioridad   = static_cast<Priority>(json["prioridad"].toInt(3));
    p.m_tipoSangre  = bloodTypeFromString(json["tipoSangre"].toString());
    p.m_diagnostico = json["diagnostico"].toString();
    return p;
}

// ── Static helpers ────────────────────────────────────────────────────────────

QString Patient::priorityLabel(Priority p)
{
    switch (p) {
    case Priority::Critical: return QStringLiteral("Crítico");
    case Priority::Urgent:   return QStringLiteral("Urgente");
    default:                 return QStringLiteral("Normal");
    }
}

QString Patient::bloodTypeLabel(BloodType bt)
{
    switch (bt) {
    case BloodType::A_Pos:  return "A+";
    case BloodType::A_Neg:  return "A-";
    case BloodType::B_Pos:  return "B+";
    case BloodType::B_Neg:  return "B-";
    case BloodType::AB_Pos: return "AB+";
    case BloodType::AB_Neg: return "AB-";
    case BloodType::O_Pos:  return "O+";
    case BloodType::O_Neg:  return "O-";
    }
    return "O+";
}

BloodType Patient::bloodTypeFromString(const QString &s)
{
    if (s == "A+")  return BloodType::A_Pos;
    if (s == "A-")  return BloodType::A_Neg;
    if (s == "B+")  return BloodType::B_Pos;
    if (s == "B-")  return BloodType::B_Neg;
    if (s == "AB+") return BloodType::AB_Pos;
    if (s == "AB-") return BloodType::AB_Neg;
    if (s == "O-")  return BloodType::O_Neg;
    return BloodType::O_Pos;
}
