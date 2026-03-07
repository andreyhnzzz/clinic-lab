#include "Module3_DiagnosisTree.h"

Module3_DiagnosisTree::Module3_DiagnosisTree(QObject* parent)
    : QObject(parent) {}

QVector<Diagnostico> Module3_DiagnosisTree::searchByName(const QString& query) {
    return tree_.searchByName(query);
}

QVector<Diagnostico> Module3_DiagnosisTree::preOrderTraversal() {
    return tree_.preOrderTraversal();
}

QVector<Diagnostico> Module3_DiagnosisTree::bfsTraversal() {
    return tree_.bfsTraversal();
}

int Module3_DiagnosisTree::totalNodes() const {
    return tree_.totalNodes();
}

int Module3_DiagnosisTree::totalDiagnoses() const {
    return tree_.totalDiagnoses();
}
