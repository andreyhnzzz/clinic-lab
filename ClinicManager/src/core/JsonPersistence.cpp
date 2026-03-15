#include "JsonPersistence.h"
#include "ClinicDataStore.h"
#include "EventLog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

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

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        emit errorOccurred("Formato JSON invalido.");
        return false;
    }

    QJsonObject root = doc.object();
    auto& store = ClinicDataStore::instance();

    // Load patients
    QVector<Paciente> pacientes;
    QJsonArray pacArr = root["pacientes"].toArray();
    for (const auto& val : pacArr) {
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
        pacientes.push_back(p);
    }

    // Load consultations
    QVector<Consulta> consultas;
    QJsonArray conArr = root["consultas"].toArray();
    for (const auto& val : conArr) {
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
        consultas.push_back(c);
    }

    store.loadPacientes(pacientes);
    store.loadConsultas(consultas);

    EventLog::instance().success("Persistencia",
        QString("Datos cargados: %1 pacientes, %2 consultas desde %3")
            .arg(pacientes.size()).arg(consultas.size()).arg(filePath));
    emit loaded(filePath);
    return true;
}
