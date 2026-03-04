#pragma once

#include <QObject>
#include <QString>
#include <QDate>
#include <QJsonObject>

class Consulta : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int     idConsulta      READ idConsulta      WRITE setIdConsulta      NOTIFY idConsultaChanged)
    Q_PROPERTY(QString cedulaPaciente  READ cedulaPaciente  WRITE setCedulaPaciente  NOTIFY cedulaPacienteChanged)
    Q_PROPERTY(QDate   fecha           READ fecha           WRITE setFecha           NOTIFY fechaChanged)
    Q_PROPERTY(QString medico          READ medico          WRITE setMedico          NOTIFY medicoChanged)
    Q_PROPERTY(QString diagnostico     READ diagnostico     WRITE setDiagnostico     NOTIFY diagnosticoChanged)
    Q_PROPERTY(int     gravedad        READ gravedad        WRITE setGravedad        NOTIFY gravedadChanged)
    Q_PROPERTY(double  costo           READ costo           WRITE setCosto           NOTIFY costoChanged)

public:
    explicit Consulta(QObject *parent = nullptr);
    Consulta(int idConsulta,
             const QString &cedulaPaciente,
             const QDate &fecha,
             const QString &medico,
             const QString &diagnostico,
             int gravedad,
             double costo,
             QObject *parent = nullptr);

    Consulta(const Consulta &other);
    Consulta &operator=(const Consulta &other);

    // Getters
    int     idConsulta()     const { return m_idConsulta; }
    QString cedulaPaciente() const { return m_cedulaPaciente; }
    QDate   fecha()          const { return m_fecha; }
    QString medico()         const { return m_medico; }
    QString diagnostico()    const { return m_diagnostico; }
    int     gravedad()       const { return m_gravedad; }
    double  costo()          const { return m_costo; }

    // Setters
    void setIdConsulta(int v);
    void setCedulaPaciente(const QString &v);
    void setFecha(const QDate &v);
    void setMedico(const QString &v);
    void setDiagnostico(const QString &v);
    void setGravedad(int v);
    void setCosto(double v);

    // Validation
    bool isValid() const;

    // Serialization
    QJsonObject toJson() const;
    static Consulta fromJson(const QJsonObject &json, QObject *parent = nullptr);

    // Comparison (sort by fecha then gravedad)
    bool operator<(const Consulta &other) const;
    bool operator>(const Consulta &other) const;
    bool operator==(const Consulta &other) const;

signals:
    void idConsultaChanged();
    void cedulaPacienteChanged();
    void fechaChanged();
    void medicoChanged();
    void diagnosticoChanged();
    void gravedadChanged();
    void costoChanged();

private:
    int     m_idConsulta     = 0;
    QString m_cedulaPaciente;
    QDate   m_fecha;
    QString m_medico;
    QString m_diagnostico;
    int     m_gravedad       = 1;  // 1-5
    double  m_costo          = 0.0;
};
