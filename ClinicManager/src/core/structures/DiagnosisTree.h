#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <functional>
#include <memory>

#include "Diagnostico.h"

// N-ary tree built from DiagnosticoNode.
// Supports pre-order and level-order traversals.

class DiagnosisTree : public QObject
{
    Q_OBJECT

public:
    explicit DiagnosisTree(QObject *parent = nullptr);

    // Set/replace the root (takes ownership)
    void setRoot(std::shared_ptr<DiagnosticoNode> root);
    std::shared_ptr<DiagnosticoNode> root() const { return m_root; }

    bool isEmpty() const { return !m_root; }

    // ── Traversals ───────────────────────────────────────────────────────────

    // Pre-order: root → children left-to-right
    QVector<DiagnosticoNode*> preOrder() const;

    // Level-order (BFS)
    QVector<DiagnosticoNode*> levelOrder() const;

    // ── Search ───────────────────────────────────────────────────────────────

    // Find first node whose nombre or codigo contains the query (case-insensitive)
    DiagnosticoNode *findByName(const QString &query) const;
    DiagnosticoNode *findByCodigo(const QString &codigo) const;

    // ── Statistics ────────────────────────────────────────────────────────────

    int totalNodes()    const;
    int height()        const;

private:
    std::shared_ptr<DiagnosticoNode> m_root;

    void preOrderHelper(DiagnosticoNode *node, QVector<DiagnosticoNode*> &out) const;
    int  heightHelper(const DiagnosticoNode *node) const;
};
