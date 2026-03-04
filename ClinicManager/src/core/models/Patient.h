#pragma once

#include <QObject>
#include <QString>
#include <QDate>
#include <QJsonObject>

// Priority levels for the waiting queue
enum class Priority {
    Critical = 1,
    Urgent   = 2,
    Normal   = 3
};

// ABO+Rh blood types
enum class BloodType {
    A_Pos, A_Neg,
    B_Pos, B_Neg,
    AB_Pos, AB_Neg,
    O_Pos, O_Neg
};

class Patient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString cedula      READ cedula      WRITE setCedula      NOTIFY cedulaChanged)
    Q_PROPERTY(QString nombre      READ nombre      WRITE setNombre      NOTIFY nombreChanged)
    Q_PROPERTY(int     edad        READ edad        WRITE setEdad        NOTIFY edadChanged)
    Q_PROPERTY(QDate   fechaReg    READ fechaReg    WRITE setFechaReg    NOTIFY fechaRegChanged)
    Q_PROPERTY(int     prioridad   READ prioridad   WRITE setPrioridad   NOTIFY prioridadChanged)
    Q_PROPERTY(QString tipoSangre  READ tipoSangre  WRITE setTipoSangre  NOTIFY tipoSangreChanged)
    Q_PROPERTY(QString diagnostico READ diagnostico WRITE setDiagnostico NOTIFY diagnosticoChanged)

public:
    explicit Patient(QObject *parent = nullptr);
    Patient(const QString &cedula,
            const QString &nombre,
            int edad,
            const QDate &fechaReg,
            Priority prioridad,
            BloodType tipoSangre,
            const QString &diagnostico,
            QObject *parent = nullptr);

    // Copy constructor and assignment (needed for containers)
    Patient(const Patient &other);
    Patient &operator=(const Patient &other);

    // Getters
    QString cedula()      const { return m_cedula; }
    QString nombre()      const { return m_nombre; }
    int     edad()        const { return m_edad; }
    QDate   fechaReg()    const { return m_fechaReg; }
    int     prioridad()   const { return static_cast<int>(m_prioridad); }
    Priority priorityEnum() const { return m_prioridad; }
    QString tipoSangre()  const;
    BloodType bloodTypeEnum() const { return m_tipoSangre; }
    QString diagnostico() const { return m_diagnostico; }

    // Setters
    void setCedula(const QString &v);
    void setNombre(const QString &v);
    void setEdad(int v);
    void setFechaReg(const QDate &v);
    void setPrioridad(int v);
    void setPriorityEnum(Priority p);
    void setTipoSangre(const QString &v);
    void setBloodTypeEnum(BloodType bt);
    void setDiagnostico(const QString &v);

    // Validation
    static bool isValidCedula(const QString &cedula);
    bool isValid() const;

    // Serialization
    QJsonObject toJson() const;
    static Patient fromJson(const QJsonObject &json, QObject *parent = nullptr);

    // Comparison operators (used by sorting algorithms)
    bool operator<(const Patient &other) const;
    bool operator>(const Patient &other) const;
    bool operator==(const Patient &other) const;

    // Human-readable priority label
    static QString priorityLabel(Priority p);
    static QString bloodTypeLabel(BloodType bt);
    static BloodType bloodTypeFromString(const QString &s);

signals:
    void cedulaChanged();
    void nombreChanged();
    void edadChanged();
    void fechaRegChanged();
    void prioridadChanged();
    void tipoSangreChanged();
    void diagnosticoChanged();

private:
    QString   m_cedula;
    QString   m_nombre;
    int       m_edad      = 0;
    QDate     m_fechaReg;
    Priority  m_prioridad = Priority::Normal;
    BloodType m_tipoSangre = BloodType::O_Pos;
    QString   m_diagnostico;
};
