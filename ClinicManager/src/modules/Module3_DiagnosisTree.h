#pragma once
#include <QObject>
#include <QVector>
#include <QString>
#include "../data_structures/DiagnosisTree.h"
#include "../models/Diagnostico.h"

class Module3_DiagnosisTree : public QObject {
    Q_OBJECT
public:
    explicit Module3_DiagnosisTree(QObject* parent = nullptr);

    DiagnosisTree& getTree() { return tree_; }
    const DiagnosisTree& getTree() const { return tree_; }

    QVector<Diagnostico> searchByName(const QString& query);
    QVector<Diagnostico> preOrderTraversal();
    QVector<Diagnostico> bfsTraversal();

    int totalNodes() const;
    int totalDiagnoses() const;

private:
    DiagnosisTree tree_;
};
