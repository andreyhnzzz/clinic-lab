#include "DiagnosisTree.h"
#include <QQueue>

DiagnosisTree::DiagnosisTree() {
    root_ = new DiagnosisNode("Árbol de Diagnósticos", "", "", -1);
    loadDefaultData();
}

DiagnosisTree::~DiagnosisTree() {
    delete root_;
}

void DiagnosisTree::addArea(const QString& name) {
    auto* node = new DiagnosisNode(name, "", "", 0);
    node->parent = root_;
    root_->children.push_back(node);
}

void DiagnosisTree::addSpecialty(const QString& area, const QString& specialty) {
    DiagnosisNode* areaNode = findArea(area);
    if (!areaNode) return;
    auto* node = new DiagnosisNode(specialty, "", "", 1);
    node->parent = areaNode;
    areaNode->children.push_back(node);
}

void DiagnosisTree::addDiagnosis(const QString& area, const QString& specialty,
                                  const QString& code, const QString& name, const QString& description) {
    DiagnosisNode* areaNode = findArea(area);
    if (!areaNode) return;
    DiagnosisNode* specNode = findSpecialty(areaNode, specialty);
    if (!specNode) return;
    auto* node = new DiagnosisNode(name, code, description, 2);
    node->parent = specNode;
    specNode->children.push_back(node);
}

DiagnosisNode* DiagnosisTree::findArea(const QString& name) const {
    for (auto* child : root_->children)
        if (child->name == name) return child;
    return nullptr;
}

DiagnosisNode* DiagnosisTree::findSpecialty(DiagnosisNode* area, const QString& name) const {
    for (auto* child : area->children)
        if (child->name == name) return child;
    return nullptr;
}

void DiagnosisTree::preOrderHelper(DiagnosisNode* node, QVector<Diagnostico>& results) const {
    if (!node) return;
    if (node->level == 2) {
        Diagnostico d;
        d.codigo = node->code.toStdString();
        d.nombre = node->name.toStdString();
        d.categoria = node->parent ? node->parent->parent->name.toStdString() : "";
        d.subcategoria = node->parent ? node->parent->name.toStdString() : "";
        d.descripcion = node->description.toStdString();
        results.push_back(d);
    }
    for (auto* child : node->children)
        preOrderHelper(child, results);
}

QVector<Diagnostico> DiagnosisTree::preOrderTraversal() const {
    QVector<Diagnostico> results;
    preOrderHelper(root_, results);
    return results;
}

QVector<Diagnostico> DiagnosisTree::bfsTraversal() const {
    QVector<Diagnostico> results;
    if (!root_) return results;
    QQueue<DiagnosisNode*> queue;
    queue.enqueue(root_);
    while (!queue.isEmpty()) {
        DiagnosisNode* node = queue.dequeue();
        if (node->level == 2) {
            Diagnostico d;
            d.codigo = node->code.toStdString();
            d.nombre = node->name.toStdString();
            d.categoria = node->parent ? node->parent->parent->name.toStdString() : "";
            d.subcategoria = node->parent ? node->parent->name.toStdString() : "";
            d.descripcion = node->description.toStdString();
            results.push_back(d);
        }
        for (auto* child : node->children)
            queue.enqueue(child);
    }
    return results;
}

QVector<Diagnostico> DiagnosisTree::searchByName(const QString& query) const {
    QVector<Diagnostico> results;
    QString lower = query.toLower();
    QQueue<DiagnosisNode*> queue;
    queue.enqueue(root_);
    while (!queue.isEmpty()) {
        DiagnosisNode* node = queue.dequeue();
        if (node->level == 2 && node->name.toLower().contains(lower)) {
            Diagnostico d;
            d.codigo = node->code.toStdString();
            d.nombre = node->name.toStdString();
            d.categoria = node->parent ? node->parent->parent->name.toStdString() : "";
            d.subcategoria = node->parent ? node->parent->name.toStdString() : "";
            d.descripcion = node->description.toStdString();
            results.push_back(d);
        }
        for (auto* child : node->children)
            queue.enqueue(child);
    }
    return results;
}

QVector<Diagnostico> DiagnosisTree::searchByCode(const QString& code) const {
    QVector<Diagnostico> results;
    QString lower = code.toLower();
    QQueue<DiagnosisNode*> queue;
    queue.enqueue(root_);
    while (!queue.isEmpty()) {
        DiagnosisNode* node = queue.dequeue();
        if (node->level == 2 && node->code.toLower().contains(lower)) {
            Diagnostico d;
            d.codigo = node->code.toStdString();
            d.nombre = node->name.toStdString();
            d.categoria = node->parent ? node->parent->parent->name.toStdString() : "";
            d.subcategoria = node->parent ? node->parent->name.toStdString() : "";
            d.descripcion = node->description.toStdString();
            results.push_back(d);
        }
        for (auto* child : node->children)
            queue.enqueue(child);
    }
    return results;
}

QVector<Diagnostico> DiagnosisTree::listBySpecialty(const QString& specialty) const {
    QVector<Diagnostico> results;
    QString lower = specialty.toLower();
    QQueue<DiagnosisNode*> queue;
    queue.enqueue(root_);
    while (!queue.isEmpty()) {
        DiagnosisNode* node = queue.dequeue();
        if (node->level == 1 && node->name.toLower() == lower) {
            // Found specialty node - collect all diagnoses under it
            for (auto* diag : node->children) {
                Diagnostico d;
                d.codigo = diag->code.toStdString();
                d.nombre = diag->name.toStdString();
                d.categoria = node->parent ? node->parent->name.toStdString() : "";
                d.subcategoria = node->name.toStdString();
                d.descripcion = diag->description.toStdString();
                results.push_back(d);
            }
        }
        for (auto* child : node->children)
            queue.enqueue(child);
    }
    return results;
}

QStringList DiagnosisTree::allSpecialties() const {
    QStringList list;
    if (!root_) return list;
    for (auto* area : root_->children)
        for (auto* spec : area->children)
            list.push_back(spec->name);
    return list;
}

void DiagnosisTree::countHelper(DiagnosisNode* node, int& count) const {
    if (!node) return;
    count++;
    for (auto* child : node->children)
        countHelper(child, count);
}

int DiagnosisTree::totalNodes() const {
    int count = 0;
    countHelper(root_, count);
    return count;
}

int DiagnosisTree::totalDiagnoses() const {
    return static_cast<int>(preOrderTraversal().size());
}

void DiagnosisTree::loadDefaultData() {
    // Medicina Interna
    addArea("Medicina Interna");
    addSpecialty("Medicina Interna", "Cardiología");
    addDiagnosis("Medicina Interna", "Cardiología", "I21.0", "Infarto agudo de miocardio",
                 "Infarto de miocardio transmural de la pared anterior");
    addDiagnosis("Medicina Interna", "Cardiología", "I10", "Hipertensión arterial",
                 "Hipertensión esencial primaria");
    addDiagnosis("Medicina Interna", "Cardiología", "I48", "Fibrilación auricular",
                 "Arritmia cardíaca con contracción irregular de aurículas");
    addDiagnosis("Medicina Interna", "Cardiología", "I50.0", "Insuficiencia cardíaca congestiva",
                 "Falla del corazón para bombear sangre suficiente");
    addDiagnosis("Medicina Interna", "Cardiología", "I25.1", "Enfermedad coronaria aterosclerótica",
                 "Obstrucción de arterias coronarias por placas de ateroma");

    addSpecialty("Medicina Interna", "Endocrinología");
    addDiagnosis("Medicina Interna", "Endocrinología", "E11", "Diabetes mellitus tipo 2",
                 "Trastorno metabólico crónico con hiperglucemia");
    addDiagnosis("Medicina Interna", "Endocrinología", "E03.9", "Hipotiroidismo no especificado",
                 "Producción insuficiente de hormonas tiroideas");
    addDiagnosis("Medicina Interna", "Endocrinología", "E05.0", "Hipertiroidismo con bocio difuso",
                 "Producción excesiva de hormonas tiroideas");
    addDiagnosis("Medicina Interna", "Endocrinología", "E66.0", "Obesidad debida a exceso de calorías",
                 "IMC igual o superior a 30");

    addSpecialty("Medicina Interna", "Gastroenterología");
    addDiagnosis("Medicina Interna", "Gastroenterología", "K29.7", "Gastritis no especificada",
                 "Inflamación de la mucosa gástrica");
    addDiagnosis("Medicina Interna", "Gastroenterología", "K57.3", "Enfermedad diverticular del colon",
                 "Formación de divertículos en el colon");
    addDiagnosis("Medicina Interna", "Gastroenterología", "K74.6", "Cirrosis hepática no especificada",
                 "Fibrosis progresiva del tejido hepático");

    // Pediatría
    addArea("Pediatría");
    addSpecialty("Pediatría", "Neonatología");
    addDiagnosis("Pediatría", "Neonatología", "P36.9", "Sepsis neonatal",
                 "Infección bacteriana sistémica en recién nacidos");
    addDiagnosis("Pediatría", "Neonatología", "P59.9", "Ictericia neonatal no especificada",
                 "Coloración amarillenta de piel y mucosas en neonatos");
    addDiagnosis("Pediatría", "Neonatología", "P07.3", "Prematuridad extrema",
                 "Nacimiento antes de las 28 semanas de gestación");

    addSpecialty("Pediatría", "Infectología Pediátrica");
    addDiagnosis("Pediatría", "Infectología Pediátrica", "J21.0", "Bronquiolitis aguda por VSR",
                 "Inflamación de bronquiolos por virus sincitial respiratorio");
    addDiagnosis("Pediatría", "Infectología Pediátrica", "J18.9", "Neumonía pediátrica",
                 "Infección pulmonar en pacientes menores de 15 años");
    addDiagnosis("Pediatría", "Infectología Pediátrica", "A09", "Diarrea y gastroenteritis infecciosa",
                 "Infección gastrointestinal aguda en niños");

    // Cirugía
    addArea("Cirugía");
    addSpecialty("Cirugía", "Cirugía General");
    addDiagnosis("Cirugía", "Cirugía General", "K35.8", "Apendicitis aguda",
                 "Inflamación aguda del apéndice vermiforme");
    addDiagnosis("Cirugía", "Cirugía General", "K81.0", "Colecistitis aguda",
                 "Inflamación aguda de la vesícula biliar");
    addDiagnosis("Cirugía", "Cirugía General", "K40.9", "Hernia inguinal",
                 "Protrusión de tejido a través de la región inguinal");
    addDiagnosis("Cirugía", "Cirugía General", "K56.6", "Obstrucción intestinal",
                 "Bloqueo del tránsito intestinal");

    addSpecialty("Cirugía", "Cirugía Vascular");
    addDiagnosis("Cirugía", "Cirugía Vascular", "I83.9", "Varices de extremidades inferiores",
                 "Dilatación patológica de venas superficiales");
    addDiagnosis("Cirugía", "Cirugía Vascular", "I74.3", "Embolia arterial de extremidad inferior",
                 "Oclusión arterial aguda por émbolo");

    // Ginecología
    addArea("Ginecología");
    addSpecialty("Ginecología", "Obstetricia");
    addDiagnosis("Ginecología", "Obstetricia", "O80", "Parto único espontáneo",
                 "Parto vaginal sin complicaciones");
    addDiagnosis("Ginecología", "Obstetricia", "O82", "Parto único por cesárea",
                 "Extracción fetal por incisión abdominal y uterina");
    addDiagnosis("Ginecología", "Obstetricia", "O14.1", "Preeclampsia severa",
                 "Hipertensión con proteinuria durante el embarazo");
    addDiagnosis("Ginecología", "Obstetricia", "O42.0", "Ruptura prematura de membranas",
                 "Rotura del saco amniótico antes del inicio del parto");

    addSpecialty("Ginecología", "Ginecología General");
    addDiagnosis("Ginecología", "Ginecología General", "N83.2", "Quiste ovárico",
                 "Formación quística en tejido ovárico");
    addDiagnosis("Ginecología", "Ginecología General", "D25.9", "Leiomioma uterino",
                 "Tumor benigno de músculo liso uterino");

    // Ortopedia
    addArea("Ortopedia");
    addSpecialty("Ortopedia", "Traumatología");
    addDiagnosis("Ortopedia", "Traumatología", "S72.0", "Fractura de cuello del fémur",
                 "Fractura en la región proximal del fémur");
    addDiagnosis("Ortopedia", "Traumatología", "S40.0", "Luxación de hombro",
                 "Desplazamiento de la cabeza humeral fuera de la cavidad glenoidea");
    addDiagnosis("Ortopedia", "Traumatología", "S93.4", "Esguince de tobillo",
                 "Distensión o desgarro de ligamentos del tobillo");
    addDiagnosis("Ortopedia", "Traumatología", "S52.5", "Fractura de radio distal",
                 "Fractura en extremidad distal del hueso radio");

    addSpecialty("Ortopedia", "Columna Vertebral");
    addDiagnosis("Ortopedia", "Columna Vertebral", "M51.1", "Hernia de disco lumbar",
                 "Protrusión del núcleo pulposo del disco intervertebral");
    addDiagnosis("Ortopedia", "Columna Vertebral", "M47.8", "Espondilosis cervical",
                 "Degeneración de discos y articulaciones cervicales");

    // Neurología
    addArea("Neurología");
    addSpecialty("Neurología", "Neurología General");
    addDiagnosis("Neurología", "Neurología General", "I63.9", "ACV isquémico no especificado",
                 "Accidente cerebrovascular por oclusión arterial");
    addDiagnosis("Neurología", "Neurología General", "G43.9", "Migraña no especificada",
                 "Cefalea episódica severa con o sin aura");
    addDiagnosis("Neurología", "Neurología General", "G40.9", "Epilepsia no especificada",
                 "Trastorno neurológico con crisis convulsivas recurrentes");
    addDiagnosis("Neurología", "Neurología General", "G35", "Esclerosis múltiple",
                 "Enfermedad desmielinizante del sistema nervioso central");

    addSpecialty("Neurología", "Neurocirugía");
    addDiagnosis("Neurología", "Neurocirugía", "G91.9", "Hidrocefalia no especificada",
                 "Acumulación anormal de LCR en el sistema ventricular");
    addDiagnosis("Neurología", "Neurocirugía", "S06.0", "Conmoción cerebral",
                 "Traumatismo craneoencefálico leve con alteración transitoria");

    // Oncología
    addArea("Oncología");
    addSpecialty("Oncología", "Oncología Médica");
    addDiagnosis("Oncología", "Oncología Médica", "C50.9", "Cáncer de mama",
                 "Neoplasia maligna del tejido mamario");
    addDiagnosis("Oncología", "Oncología Médica", "C61", "Cáncer de próstata",
                 "Adenocarcinoma de la glándula prostática");
    addDiagnosis("Oncología", "Oncología Médica", "C18.9", "Cáncer colorrectal",
                 "Neoplasia maligna del colon y recto");
    addDiagnosis("Oncología", "Oncología Médica", "C34.9", "Cáncer de pulmón",
                 "Neoplasia maligna del bronquio y pulmón");

    // Psiquiatría
    addArea("Psiquiatría");
    addSpecialty("Psiquiatría", "Psiquiatría General");
    addDiagnosis("Psiquiatría", "Psiquiatría General", "F32.1", "Episodio depresivo moderado",
                 "Trastorno del estado de ánimo con síntomas depresivos");
    addDiagnosis("Psiquiatría", "Psiquiatría General", "F41.1", "Trastorno de ansiedad generalizada",
                 "Ansiedad persistente y excesiva no relacionada con causa específica");
    addDiagnosis("Psiquiatría", "Psiquiatría General", "F20.0", "Esquizofrenia paranoide",
                 "Trastorno psicótico con alucinaciones y delirios prominentes");
}
