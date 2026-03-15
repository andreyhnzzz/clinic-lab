#pragma once
#include <QString>
#include <QVector>
#include "../models/Diagnostico.h"

struct DiagnosisNode {
    QString name;
    QString code;
    QString description;
    int level = 0;  // 0=area, 1=specialty, 2=diagnosis
    QVector<DiagnosisNode*> children;
    DiagnosisNode* parent = nullptr;

    explicit DiagnosisNode(const QString& n, const QString& c = "", const QString& d = "", int lvl = 0)
        : name(n), code(c), description(d), level(lvl) {}

    ~DiagnosisNode() {
        for (auto* child : children) delete child;
    }
};

class DiagnosisTree {
public:
    DiagnosisTree();
    ~DiagnosisTree();

    DiagnosisNode* root() const { return root_; }

    void addArea(const QString& name);
    void addSpecialty(const QString& area, const QString& specialty);
    void addDiagnosis(const QString& area, const QString& specialty,
                      const QString& code, const QString& name, const QString& description);

    QVector<Diagnostico> preOrderTraversal() const;
    QVector<Diagnostico> bfsTraversal() const;
    QVector<Diagnostico> postOrderTraversal() const;
    QVector<Diagnostico> searchByName(const QString& query) const;
    QVector<Diagnostico> searchByCode(const QString& code) const;
    QVector<Diagnostico> listBySpecialty(const QString& specialty) const;
    QStringList allSpecialties() const;

    // Full traversals that include ALL nodes (areas, specialties, diagnoses)
    // Returns a formatted string for each node: "[level] name (code)"
    QStringList fullPreOrderTraversal() const;
    QStringList fullBfsTraversal() const;
    QStringList fullPostOrderTraversal() const;

    int totalNodes() const;
    int totalDiagnoses() const;

    void loadDefaultData();

private:
    DiagnosisNode* root_ = nullptr;

    DiagnosisNode* findArea(const QString& name) const;
    DiagnosisNode* findSpecialty(DiagnosisNode* area, const QString& name) const;

    void preOrderHelper(DiagnosisNode* node, QVector<Diagnostico>& results) const;
    void postOrderHelper(DiagnosisNode* node, QVector<Diagnostico>& results) const;
    void fullPreOrderHelper(DiagnosisNode* node, QStringList& results, int depth) const;
    void fullPostOrderHelper(DiagnosisNode* node, QStringList& results, int depth) const;
    void countHelper(DiagnosisNode* node, int& count) const;
};
