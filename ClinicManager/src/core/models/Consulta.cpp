#include "Consulta.h"

Consulta::Consulta(QObject *parent)
    : QObject(parent) {}

Consulta::Consulta(int idConsulta,
                   const QString &cedulaPaciente,
                   const QDate &fecha,
                   const QString &medico,
                   const QString &diagnostico,
                   int gravedad,
                   double costo,
                   QObject *parent)
    : QObject(parent),
      m_idConsulta(idConsulta),
      m_cedulaPaciente(cedulaPaciente),
      m_fecha(fecha),
      m_medico(medico),
      m_diagnostico(diagnostico),
      m_gravedad(qBound(1, gravedad, 5)),
      m_costo(costo)
{}

Consulta::Consulta(const Consulta &other)
    : QObject(other.parent()),
      m_idConsulta(other.m_idConsulta),
      m_cedulaPaciente(other.m_cedulaPaciente),
      m_fecha(other.m_fecha),
      m_medico(other.m_medico),
      m_diagnostico(other.m_diagnostico),
      m_gravedad(other.m_gravedad),
      m_costo(other.m_costo)
{}

Consulta &Consulta::operator=(const Consulta &other)
{
    if (this == &other) return *this;
    m_idConsulta     = other.m_idConsulta;
    m_cedulaPaciente = other.m_cedulaPaciente;
    m_fecha          = other.m_fecha;
    m_medico         = other.m_medico;
    m_diagnostico    = other.m_diagnostico;
    m_gravedad       = other.m_gravedad;
    m_costo          = other.m_costo;
    return *this;
}

// ── Setters ──────────────────────────────────────────────────────────────────

void Consulta::setIdConsulta(int v)            { if (m_idConsulta != v)     { m_idConsulta = v;         emit idConsultaChanged(); } }
void Consulta::setCedulaPaciente(const QString &v){ if (m_cedulaPaciente != v){ m_cedulaPaciente = v;    emit cedulaPacienteChanged(); } }
void Consulta::setFecha(const QDate &v)        { if (m_fecha != v)          { m_fecha = v;              emit fechaChanged(); } }
void Consulta::setMedico(const QString &v)     { if (m_medico != v)         { m_medico = v;             emit medicoChanged(); } }
void Consulta::setDiagnostico(const QString &v){ if (m_diagnostico != v)    { m_diagnostico = v;        emit diagnosticoChanged(); } }
void Consulta::setCosto(double v)              { if (m_costo != v)          { m_costo = v;              emit costoChanged(); } }

void Consulta::setGravedad(int v)
{
    int clamped = qBound(1, v, 5);
    if (m_gravedad != clamped) { m_gravedad = clamped; emit gravedadChanged(); }
}

// ── Validation ───────────────────────────────────────────────────────────────

bool Consulta::isValid() const
{
    return m_idConsulta > 0
        && !m_cedulaPaciente.isEmpty()
        && m_fecha.isValid()
        && !m_medico.isEmpty()
        && m_gravedad >= 1 && m_gravedad <= 5
        && m_costo >= 0.0;
}

// ── Comparison ───────────────────────────────────────────────────────────────

bool Consulta::operator<(const Consulta &other) const
{
    if (m_fecha != other.m_fecha) return m_fecha < other.m_fecha;
    return m_gravedad < other.m_gravedad;
}

bool Consulta::operator>(const Consulta &other) const { return other < *this; }
bool Consulta::operator==(const Consulta &other) const { return m_idConsulta == other.m_idConsulta; }

// ── Serialization ─────────────────────────────────────────────────────────────

QJsonObject Consulta::toJson() const
{
    QJsonObject obj;
    obj["idConsulta"]     = m_idConsulta;
    obj["cedulaPaciente"] = m_cedulaPaciente;
    obj["fecha"]          = m_fecha.toString(Qt::ISODate);
    obj["medico"]         = m_medico;
    obj["diagnostico"]    = m_diagnostico;
    obj["gravedad"]       = m_gravedad;
    obj["costo"]          = m_costo;
    return obj;
}

Consulta Consulta::fromJson(const QJsonObject &json, QObject *parent)
{
    Consulta c(parent);
    c.m_idConsulta     = json["idConsulta"].toInt();
    c.m_cedulaPaciente = json["cedulaPaciente"].toString();
    c.m_fecha          = QDate::fromString(json["fecha"].toString(), Qt::ISODate);
    c.m_medico         = json["medico"].toString();
    c.m_diagnostico    = json["diagnostico"].toString();
    c.m_gravedad       = qBound(1, json["gravedad"].toInt(1), 5);
    c.m_costo          = json["costo"].toDouble();
    return c;
}
