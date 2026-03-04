#include "Diagnostico.h"
#include <QJsonArray>

DiagnosticoNode::DiagnosticoNode(QObject *parent)
    : QObject(parent) {}

DiagnosticoNode::DiagnosticoNode(const QString &codigo,
                                 const QString &nombre,
                                 const QString &categoria,
                                 const QString &subcategoria,
                                 const QString &descripcion,
                                 Level level,
                                 QObject *parent)
    : QObject(parent),
      m_codigo(codigo),
      m_nombre(nombre),
      m_categoria(categoria),
      m_subcategoria(subcategoria),
      m_descripcion(descripcion),
      m_level(level)
{}

void DiagnosticoNode::addChild(std::shared_ptr<DiagnosticoNode> child)
{
    m_children.append(std::move(child));
}

QJsonObject DiagnosticoNode::toJson() const
{
    QJsonObject obj;
    obj["codigo"]       = m_codigo;
    obj["nombre"]       = m_nombre;
    obj["categoria"]    = m_categoria;
    obj["subcategoria"] = m_subcategoria;
    obj["descripcion"]  = m_descripcion;
    obj["level"]        = static_cast<int>(m_level);

    QJsonArray childArray;
    for (const auto &child : m_children)
        childArray.append(child->toJson());
    obj["children"] = childArray;

    return obj;
}

std::shared_ptr<DiagnosticoNode> DiagnosticoNode::fromJson(const QJsonObject &json, QObject *parent)
{
    auto node = std::make_shared<DiagnosticoNode>(parent);
    node->m_codigo       = json["codigo"].toString();
    node->m_nombre       = json["nombre"].toString();
    node->m_categoria    = json["categoria"].toString();
    node->m_subcategoria = json["subcategoria"].toString();
    node->m_descripcion  = json["descripcion"].toString();
    node->m_level        = static_cast<Level>(json["level"].toInt(0));

    for (const QJsonValue &val : json["children"].toArray())
        node->addChild(DiagnosticoNode::fromJson(val.toObject(), parent));

    return node;
}
