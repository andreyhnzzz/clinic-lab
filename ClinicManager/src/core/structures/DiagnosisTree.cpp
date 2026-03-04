#include "DiagnosisTree.h"

#include <QQueue>

DiagnosisTree::DiagnosisTree(QObject *parent)
    : QObject(parent) {}

void DiagnosisTree::setRoot(std::shared_ptr<DiagnosticoNode> root)
{
    m_root = std::move(root);
}

// ── Traversals ────────────────────────────────────────────────────────────────

QVector<DiagnosticoNode*> DiagnosisTree::preOrder() const
{
    QVector<DiagnosticoNode*> result;
    if (m_root) preOrderHelper(m_root.get(), result);
    return result;
}

void DiagnosisTree::preOrderHelper(DiagnosticoNode *node, QVector<DiagnosticoNode*> &out) const
{
    if (!node) return;
    out.append(node);
    for (const auto &child : node->children())
        preOrderHelper(child.get(), out);
}

QVector<DiagnosticoNode*> DiagnosisTree::levelOrder() const
{
    QVector<DiagnosticoNode*> result;
    if (!m_root) return result;

    QQueue<DiagnosticoNode*> queue;
    queue.enqueue(m_root.get());

    while (!queue.isEmpty()) {
        DiagnosticoNode *current = queue.dequeue();
        result.append(current);
        for (const auto &child : current->children())
            queue.enqueue(child.get());
    }

    return result;
}

// ── Search ────────────────────────────────────────────────────────────────────

DiagnosticoNode *DiagnosisTree::findByName(const QString &query) const
{
    QString q = query.toLower();
    for (DiagnosticoNode *node : levelOrder()) {
        if (node->nombre().toLower().contains(q) ||
            node->codigo().toLower().contains(q))
            return node;
    }
    return nullptr;
}

DiagnosticoNode *DiagnosisTree::findByCodigo(const QString &codigo) const
{
    for (DiagnosticoNode *node : levelOrder()) {
        if (node->codigo() == codigo) return node;
    }
    return nullptr;
}

// ── Statistics ────────────────────────────────────────────────────────────────

int DiagnosisTree::totalNodes() const
{
    return levelOrder().size();
}

int DiagnosisTree::height() const
{
    return m_root ? heightHelper(m_root.get()) : 0;
}

int DiagnosisTree::heightHelper(const DiagnosticoNode *node) const
{
    if (!node || !node->hasChildren()) return 1;
    int maxChildHeight = 0;
    for (const auto &child : node->children())
        maxChildHeight = qMax(maxChildHeight, heightHelper(child.get()));
    return 1 + maxChildHeight;
}
