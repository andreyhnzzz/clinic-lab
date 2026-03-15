#include "JsonPersistence.h"
#include "ClinicDataStore.h"
#include "EventLog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QSet>
#include <QDate>
#include <QRegularExpression>

JsonPersistence::JsonPersistence(QObject* parent) : QObject(parent) {}

QString JsonPersistence::defaultFilePath() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/clinic_data.json";
}

bool JsonPersistence::saveToFile(const QString& filePath) {
    auto& store = ClinicDataStore::instance();

    QJsonArray pacientesArr;
    for (const auto& p : store.pacientes()) {
        QJsonObject obj;
        obj["cedula"] = QString::fromStdString(p.cedula);
        obj["nombre"] = QString::fromStdString(p.nombre);
        obj["edad"] = p.edad;
        obj["fechaRegistro"] = QString::fromStdString(p.fechaRegistro);
        obj["prioridad"] = p.prioridad;
        obj["tipoSangre"] = QString::fromStdString(p.tipoSangre);
        obj["diagnostico"] = QString::fromStdString(p.diagnostico);
        obj["telefono"] = QString::fromStdString(p.telefono);
        obj["canton"] = QString::fromStdString(p.canton);
        pacientesArr.append(obj);
    }

    QJsonArray consultasArr;
    for (const auto& c : store.consultas()) {
        QJsonObject obj;
        obj["idConsulta"] = QString::fromStdString(c.idConsulta);
        obj["cedulaPaciente"] = QString::fromStdString(c.cedulaPaciente);
        obj["fecha"] = QString::fromStdString(c.fecha);
        obj["medicoTratante"] = QString::fromStdString(c.medicoTratante);
        obj["diagnostico"] = QString::fromStdString(c.diagnostico);
        obj["codigoDiagnostico"] = QString::fromStdString(c.codigoDiagnostico);
        obj["areaDiagnostico"] = QString::fromStdString(c.areaDiagnostico);
        obj["especialidadDiagnostico"] = QString::fromStdString(c.especialidadDiagnostico);
        obj["gravedad"] = c.gravedad;
        obj["costo"] = c.costo;
        obj["notas"] = QString::fromStdString(c.notas);
        consultasArr.append(obj);
    }

    QJsonObject root;
    root["version"] = "1.0";
    root["pacientes"] = pacientesArr;
    root["consultas"] = consultasArr;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QString err = QString("No se pudo abrir archivo: %1").arg(filePath);
        emit errorOccurred(err);
        EventLog::instance().error("Persistencia", err);
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();

    EventLog::instance().success("Persistencia",
        QString("Datos guardados: %1 pacientes, %2 consultas en %3")
            .arg(store.pacienteCount()).arg(store.consultaCount()).arg(filePath));
    emit saved(filePath);
    return true;
}

bool JsonPersistence::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QString err = QString("No se pudo abrir archivo: %1").arg(filePath);
        emit errorOccurred(err);
        EventLog::instance().error("Persistencia", err);
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError) {
        QString err = QString("JSON invalido en %1: %2")
            .arg(filePath, parseError.errorString());
        emit errorOccurred(err);
        EventLog::instance().error("Persistencia", err);
        return false;
    }

    if (!doc.isObject()) {
        QString err = "Formato JSON invalido (raiz no es objeto).";
        emit errorOccurred(err);
        EventLog::instance().error("Persistencia", err);
        return false;
    }

    QJsonObject root = doc.object();

    // Strict schema/version validation
    QString version = root.value("version").toString();
    if (version != "1.0") {
        QString err = QString("Version de schema no soportada: '%1'. Se esperaba '1.0'.")
            .arg(version.isEmpty() ? "(vacia)" : version);
        emit errorOccurred(err);
        EventLog::instance().error("Persistencia", err);
        return false;
    }

    if (!root.value("pacientes").isArray() || !root.value("consultas").isArray()) {
        QString err = "Schema invalido: 'pacientes' y 'consultas' deben ser arreglos.";
        emit errorOccurred(err);
        EventLog::instance().error("Persistencia", err);
        return false;
    }

    auto isValidIsoDate = [](const QString& dateStr) -> bool {
        static const QRegularExpression rx("^\\d{4}-\\d{2}-\\d{2}$");
        if (!rx.match(dateStr).hasMatch()) return false;
        QDate d = QDate::fromString(dateStr, "yyyy-MM-dd");
        return d.isValid();
    };

    // Load patients
    QVector<Paciente> pacientes;
    QSet<QString> cedulas;
    QJsonArray pacArr = root["pacientes"].toArray();
    for (int i = 0; i < pacArr.size(); ++i) {
        const auto& val = pacArr[i];
        if (!val.isObject()) {
            QString err = QString("Paciente[%1] invalido: se esperaba objeto.").arg(i);
            emit errorOccurred(err);
            EventLog::instance().error("Persistencia", err);
            return false;
        }
        QJsonObject obj = val.toObject();
        Paciente p;
        p.cedula = obj["cedula"].toString().toStdString();
        p.nombre = obj["nombre"].toString().toStdString();
        p.edad = obj["edad"].toInt();
        p.fechaRegistro = obj["fechaRegistro"].toString().toStdString();
        p.prioridad = obj["prioridad"].toInt(3);
        p.tipoSangre = obj["tipoSangre"].toString().toStdString();
        p.diagnostico = obj["diagnostico"].toString().toStdString();
        p.telefono = obj["telefono"].toString().toStdString();
        p.canton = obj["canton"].toString().toStdString();

        QString cedula = QString::fromStdString(p.cedula).trimmed();
        QString fechaRegistro = QString::fromStdString(p.fechaRegistro).trimmed();
        if (cedula.isEmpty()) {
            QString err = QString("Paciente[%1] invalido: cedula vacia.").arg(i);
            emit errorOccurred(err);
            EventLog::instance().error("Persistencia", err);
            return false;
        }
        if (cedulas.contains(cedula)) {
            QString err = QString("Paciente[%1] invalido: cedula duplicada '%2'.")
                .arg(i).arg(cedula);
            emit errorOccurred(err);
            EventLog::instance().error("Persistencia", err);
            return false;
        }
        if (p.prioridad < 1 || p.prioridad > 3) {
            QString err = QString("Paciente[%1] invalido: prioridad fuera de rango (%2).")
                .arg(i).arg(p.prioridad);
            emit errorOccurred(err);
            EventLog::instance().error("Persistencia", err);
            return false;
        }
        if (!isValidIsoDate(fechaRegistro)) {
            QString err = QString("Paciente[%1] invalido: fechaRegistro '%2' no valida.")
                .arg(i).arg(fechaRegistro);
            emit errorOccurred(err);
            EventLog::instance().error("Persistencia", err);
            return false;
        }

        cedulas.insert(cedula);
        pacientes.push_back(p);
    }

    // Load consultations
    QVector<Consulta> consultas;
    QJsonArray conArr = root["consultas"].toArray();
    for (int i = 0; i < conArr.size(); ++i) {
        const auto& val = conArr[i];
        if (!val.isObject()) {
            QString err = QString("Consulta[%1] invalida: se esperaba objeto.").arg(i);
            emit errorOccurred(err);
            EventLog::instance().error("Persistencia", err);
            return false;
        }
        QJsonObject obj = val.toObject();
        Consulta c;
        c.idConsulta = obj["idConsulta"].toString().toStdString();
        c.cedulaPaciente = obj["cedulaPaciente"].toString().toStdString();
        c.fecha = obj["fecha"].toString().toStdString();
        c.medicoTratante = obj["medicoTratante"].toString().toStdString();
        c.diagnostico = obj["diagnostico"].toString().toStdString();
        c.codigoDiagnostico = obj["codigoDiagnostico"].toString().toStdString();
        c.areaDiagnostico = obj["areaDiagnostico"].toString().toStdString();
        c.especialidadDiagnostico = obj["especialidadDiagnostico"].toString().toStdString();
        c.gravedad = obj["gravedad"].toInt(1);
        c.costo = obj["costo"].toDouble();
        c.notas = obj["notas"].toString().toStdString();

        QString cedulaPaciente = QString::fromStdString(c.cedulaPaciente).trimmed();
        QString fecha = QString::fromStdString(c.fecha).trimmed();
        if (cedulaPaciente.isEmpty()) {
            QString err = QString("Consulta[%1] invalida: cedulaPaciente vacia.").arg(i);
            emit errorOccurred(err);
            EventLog::instance().error("Persistencia", err);
            return false;
        }
        if (!cedulas.contains(cedulaPaciente)) {
            QString err = QString("Consulta[%1] huerfana: cedulaPaciente '%2' no existe en pacientes.")
                .arg(i).arg(cedulaPaciente);
            emit errorOccurred(err);
            EventLog::instance().error("Persistencia", err);
            return false;
        }
        if (!isValidIsoDate(fecha)) {
            QString err = QString("Consulta[%1] invalida: fecha '%2' no valida.")
                .arg(i).arg(fecha);
            emit errorOccurred(err);
            EventLog::instance().error("Persistencia", err);
            return false;
        }
        if (c.gravedad < 1 || c.gravedad > 5) {
            QString err = QString("Consulta[%1] invalida: gravedad fuera de rango (%2).")
                .arg(i).arg(c.gravedad);
            emit errorOccurred(err);
            EventLog::instance().error("Persistencia", err);
            return false;
        }
        if (c.costo < 0.0) {
            QString err = QString("Consulta[%1] invalida: costo negativo (%2).")
                .arg(i).arg(c.costo);
            emit errorOccurred(err);
            EventLog::instance().error("Persistencia", err);
            return false;
        }

        consultas.push_back(c);
    }

    // Transactional commit: mutate store only after full validation passes.
    auto& store = ClinicDataStore::instance();
    store.loadPacientes(pacientes);
    store.loadConsultas(consultas);

    EventLog::instance().success("Persistencia",
        QString("Datos cargados: %1 pacientes, %2 consultas desde %3")
            .arg(pacientes.size()).arg(consultas.size()).arg(filePath));
    emit loaded(filePath);
    return true;
}
