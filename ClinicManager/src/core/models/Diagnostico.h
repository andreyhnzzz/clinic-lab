#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QList>
#include <memory>

// Represents one node in the N-ary diagnosis tree:
//   Root → Area (e.g. "Medicina Interna")
//          └─ Specialty (e.g. "Cardiología")
//                └─ Diagnosis (e.g. "Insuficiencia Cardíaca")

class DiagnosticoNode : public QObject
{
    Q_OBJECT
public:
    enum class Level { Area = 0, Specialty = 1, Diagnosis = 2 };

    explicit DiagnosticoNode(QObject *parent = nullptr);
    DiagnosticoNode(const QString &codigo,
                    const QString &nombre,
                    const QString &categoria,
                    const QString &subcategoria,
                    const QString &descripcion,
                    Level level,
                    QObject *parent = nullptr);

    // Getters
    QString codigo()       const { return m_codigo; }
    QString nombre()       const { return m_nombre; }
    QString categoria()    const { return m_categoria; }
    QString subcategoria() const { return m_subcategoria; }
    QString descripcion()  const { return m_descripcion; }
    Level   level()        const { return m_level; }

    // Setters
    void setCodigo(const QString &v)       { m_codigo = v; }
    void setNombre(const QString &v)       { m_nombre = v; }
    void setCategoria(const QString &v)    { m_categoria = v; }
    void setSubcategoria(const QString &v) { m_subcategoria = v; }
    void setDescripcion(const QString &v)  { m_descripcion = v; }
    void setLevel(Level l)                 { m_level = l; }

    // Tree structure
    void addChild(std::shared_ptr<DiagnosticoNode> child);
    const QList<std::shared_ptr<DiagnosticoNode>> &children() const { return m_children; }
    bool hasChildren() const { return !m_children.isEmpty(); }

    // Serialization
    QJsonObject toJson() const;
    static std::shared_ptr<DiagnosticoNode> fromJson(const QJsonObject &json, QObject *parent = nullptr);

    // Comparison
    bool operator==(const DiagnosticoNode &other) const { return m_codigo == other.m_codigo; }

private:
    QString m_codigo;
    QString m_nombre;
    QString m_categoria;
    QString m_subcategoria;
    QString m_descripcion;
    Level   m_level = Level::Area;
    QList<std::shared_ptr<DiagnosticoNode>> m_children;
};

// Convenience alias used by the rest of the codebase
using Diagnostico = DiagnosticoNode;
