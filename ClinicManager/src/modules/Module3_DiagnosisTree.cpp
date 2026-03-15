#include "Module3_DiagnosisTree.h"

Module3_DiagnosisTree::Module3_DiagnosisTree(QObject* parent)
    : QObject(parent) {}

QVector<Diagnostico> Module3_DiagnosisTree::searchByName(const QString& query) {
    return tree_.searchByName(query);
}

QVector<Diagnostico> Module3_DiagnosisTree::searchByCode(const QString& code) {
    return tree_.searchByCode(code);
}

QVector<Diagnostico> Module3_DiagnosisTree::listBySpecialty(const QString& specialty) {
    return tree_.listBySpecialty(specialty);
}

QStringList Module3_DiagnosisTree::allSpecialties() const {
    return tree_.allSpecialties();
}

QVector<Diagnostico> Module3_DiagnosisTree::preOrderTraversal() {
    return tree_.preOrderTraversal();
}

QVector<Diagnostico> Module3_DiagnosisTree::bfsTraversal() {
    return tree_.bfsTraversal();
}

QVector<Diagnostico> Module3_DiagnosisTree::postOrderTraversal() {
    return tree_.postOrderTraversal();
}

QStringList Module3_DiagnosisTree::fullPreOrderTraversal() {
    return tree_.fullPreOrderTraversal();
}

QStringList Module3_DiagnosisTree::fullBfsTraversal() {
    return tree_.fullBfsTraversal();
}

QStringList Module3_DiagnosisTree::fullPostOrderTraversal() {
    return tree_.fullPostOrderTraversal();
}

int Module3_DiagnosisTree::totalNodes() const {
    return tree_.totalNodes();
}

int Module3_DiagnosisTree::totalDiagnoses() const {
    return tree_.totalDiagnoses();
}
