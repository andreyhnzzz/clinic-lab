// test_structures.cpp – Unit tests for core data structures and algorithms.
// No external test framework required; uses simple assertion macros.

#include <QCoreApplication>
#include <QDebug>
#include <cassert>
#include <iostream>

#include "Patient.h"
#include "PriorityQueue.h"
#include "DiagnosisTree.h"
#include "SearchAlgorithms.h"
#include "DataGenerator.h"
#include "CostaRicanData.h"
#include "TimerUtils.h"

// ── Helpers ───────────────────────────────────────────────────────────────────

#define TEST(name) \
    do { std::cout << "  [TEST] " << name << " ... "; } while(0)
#define PASS() \
    do { std::cout << "PASS\n"; } while(0)
#define FAIL(msg) \
    do { std::cout << "FAIL: " << msg << "\n"; return false; } while(0)

// ── Patient tests ─────────────────────────────────────────────────────────────

static bool testPatientValidation()
{
    TEST("Patient::isValidCedula - valid 9-digit");
    assert(Patient::isValidCedula("100000001"));
    assert(Patient::isValidCedula("501234567"));
    PASS();

    TEST("Patient::isValidCedula - invalid cases");
    assert(!Patient::isValidCedula("12345678"));   // 8 digits
    assert(!Patient::isValidCedula("1234567890")); // 10 digits
    assert(!Patient::isValidCedula("10000000A"));  // non-digit
    assert(!Patient::isValidCedula("000000001"));  // province 0
    PASS();

    TEST("Patient comparison operators");
    Patient critical("100000001","Alice",30,QDate(2024,1,1),Priority::Critical,BloodType::O_Pos,"");
    Patient normal  ("200000001","Bob",  25,QDate(2024,1,1),Priority::Normal,   BloodType::A_Pos,"");
    assert(critical < normal);  // critical (1) < normal (3)
    assert(normal   > critical);
    assert(!(normal < critical));
    PASS();

    TEST("Patient JSON round-trip");
    Patient p("123456789","Carlos Mora",40,QDate(2023,6,15),
               Priority::Urgent, BloodType::B_Pos,"Hipertensión");
    QJsonObject json = p.toJson();
    Patient p2 = Patient::fromJson(json);
    assert(p2.cedula()    == p.cedula());
    assert(p2.nombre()    == p.nombre());
    assert(p2.edad()      == p.edad());
    assert(p2.prioridad() == p.prioridad());
    PASS();

    return true;
}

// ── Priority Queue tests ──────────────────────────────────────────────────────

static bool testPriorityQueue()
{
    TEST("PriorityQueue basic enqueue/dequeue");
    PriorityQueue<Patient> pq;

    Patient p1("100000001","Alice",30,QDate(2024,1,1),Priority::Normal,   BloodType::O_Pos,"");
    Patient p2("200000001","Bob",  25,QDate(2024,1,2),Priority::Critical, BloodType::A_Pos,"");
    Patient p3("300000001","Carol",35,QDate(2024,1,3),Priority::Urgent,   BloodType::B_Pos,"");

    pq.enqueue(p1);
    pq.enqueue(p2);
    pq.enqueue(p3);

    assert(pq.size() == 3);
    Patient top = pq.dequeue();
    assert(top.cedula() == p2.cedula());  // Critical should come out first
    top = pq.dequeue();
    assert(top.cedula() == p3.cedula());  // Urgent second
    top = pq.dequeue();
    assert(top.cedula() == p1.cedula());  // Normal last
    assert(pq.isEmpty());
    PASS();

    TEST("PriorityQueue buildFromVector");
    QVector<Patient> patients;
    for (int i = 0; i < 100; ++i) {
        Priority p = static_cast<Priority>((i % 3) + 1);
        patients.append(Patient(
            QString("1%1").arg(i, 8, 10, QLatin1Char('0')),
            "Patient " + QString::number(i),
            20 + i % 60,
            QDate(2023, 1, 1).addDays(i),
            p, BloodType::O_Pos, "Diagnosis"));
    }
    pq.buildFromVector(patients);
    assert(pq.size() == 100);
    // First dequeued must be Critical (1)
    assert(pq.peek().priorityEnum() == Priority::Critical);
    PASS();

    return true;
}

// ── DiagnosisTree tests ───────────────────────────────────────────────────────

static bool testDiagnosisTree()
{
    TEST("DataGenerator::buildDiagnosisTree structure");
    DataGenerator gen;
    auto root = gen.buildDiagnosisTree();
    assert(root != nullptr);
    assert(!root->children().isEmpty());
    PASS();

    TEST("DiagnosisTree traversals");
    DiagnosisTree tree;
    tree.setRoot(root);

    auto preOrder   = tree.preOrder();
    auto levelOrder = tree.levelOrder();
    assert(!preOrder.isEmpty());
    assert(!levelOrder.isEmpty());
    assert(preOrder.size() == levelOrder.size());  // same nodes, different order
    PASS();

    TEST("DiagnosisTree::findByName");
    DiagnosticoNode *found = tree.findByName("Cardiología");
    assert(found != nullptr);
    assert(found->nombre() == "Cardiología");
    PASS();

    TEST("DiagnosisTree::findByName - not found returns nullptr");
    assert(tree.findByName("XYZ_nonexistent_123") == nullptr);
    PASS();

    TEST("DiagnosisTree height >= 3 (Area/Specialty/Diagnosis)");
    assert(tree.height() >= 3);
    PASS();

    return true;
}

// ── Search algorithms tests ───────────────────────────────────────────────────

static bool testSearchAlgorithms()
{
    // Build a small deterministic dataset
    QVector<Patient> data;
    for (int i = 0; i < 50; ++i) {
        data.append(Patient(
            QString("1%1").arg(i, 8, 10, QLatin1Char('0')),
            "Patient " + QString::number(i),
            20 + i, QDate(2023, 1, 1).addDays(i),
            Priority::Normal, BloodType::O_Pos, "Dx"));
    }

    QString targetCedula = "100000025";

    TEST("LinearSearch finds existing patient");
    SearchResult lr = SearchAlgorithms::linearSearchByCedula(data, targetCedula);
    assert(lr.index != -1);
    assert(data[lr.index].cedula() == targetCedula);
    assert(lr.comparisons > 0);
    PASS();

    TEST("LinearSearch returns -1 for missing patient");
    SearchResult miss = SearchAlgorithms::linearSearchByCedula(data, "999999999");
    assert(miss.index == -1);
    assert(miss.comparisons == data.size());
    PASS();

    TEST("BinarySearch finds existing patient");
    QVector<Patient> sorted = SearchAlgorithms::sortByCedula(data);
    SearchResult br = SearchAlgorithms::binarySearchByCedula(sorted, targetCedula);
    assert(br.index != -1);
    assert(sorted[br.index].cedula() == targetCedula);
    PASS();

    TEST("BinarySearch returns -1 for missing patient");
    SearchResult bmiss = SearchAlgorithms::binarySearchByCedula(sorted, "999999999");
    assert(bmiss.index == -1);
    PASS();

    TEST("Binary search uses fewer comparisons than linear on average");
    // For n=50, binary should use at most log2(50)+1 ≈ 7 comparisons
    assert(br.comparisons <= 7);
    PASS();

    return true;
}

// ── DataGenerator tests ───────────────────────────────────────────────────────

static bool testDataGenerator()
{
    TEST("DataGenerator produces valid patients");
    DataGenerator gen;
    gen.setSeed(42);
    QVector<Patient> patients = gen.generatePatients(100);
    assert(patients.size() == 100);
    for (const Patient &p : patients) {
        assert(Patient::isValidCedula(p.cedula()));
        assert(!p.nombre().isEmpty());
        assert(p.edad() >= 0 && p.edad() <= 120);
        assert(p.fechaReg().isValid());
        assert(p.prioridad() >= 1 && p.prioridad() <= 3);
    }
    PASS();

    TEST("DataGenerator is reproducible with same seed");
    DataGenerator gen2;
    gen2.setSeed(42);
    QVector<Patient> patients2 = gen2.generatePatients(100);
    assert(patients2.size() == patients.size());
    for (int i = 0; i < patients.size(); ++i)
        assert(patients[i].cedula() == patients2[i].cedula());
    PASS();

    TEST("DataGenerator produces valid consultas");
    QVector<Consulta> consultas = gen.generateConsultas(patients, 50);
    assert(consultas.size() == 50);
    for (const Consulta &c : consultas) {
        assert(c.idConsulta() > 0);
        assert(!c.cedulaPaciente().isEmpty());
        assert(c.fecha().isValid());
        assert(c.gravedad() >= 1 && c.gravedad() <= 5);
        assert(c.costo() >= 0.0);
    }
    PASS();

    return true;
}

// ── TimerUtils tests ──────────────────────────────────────────────────────────

static bool testTimerUtils()
{
    TEST("TimerUtils::ScopedTimer measures positive elapsed time");
    TimerUtils::ScopedTimer t;
    volatile int sum = 0;
    for (int i = 0; i < 10000; ++i) sum += i;
    qint64 us = t.elapsedMicroseconds();
    (void)sum;
    assert(us >= 0);
    PASS();

    TEST("TimerUtils::measureMicroseconds works");
    qint64 measured = TimerUtils::measureMicroseconds([]{ volatile int x = 0; for(int i=0;i<1000;++i) x+=i; });
    assert(measured >= 0);
    PASS();

    return true;
}

// ── Main ──────────────────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    int passed = 0, failed = 0;

    auto runSuite = [&](const char *name, bool(*fn)()) {
        std::cout << "\n=== " << name << " ===\n";
        if (fn()) { ++passed; std::cout << "  -> ALL PASSED\n"; }
        else       { ++failed; std::cout << "  -> FAILED\n"; }
    };

    runSuite("Patient Validation",    testPatientValidation);
    runSuite("Priority Queue",        testPriorityQueue);
    runSuite("Diagnosis Tree",        testDiagnosisTree);
    runSuite("Search Algorithms",     testSearchAlgorithms);
    runSuite("Data Generator",        testDataGenerator);
    runSuite("Timer Utils",           testTimerUtils);

    std::cout << "\n============================\n";
    std::cout << "Suites passed: " << passed << "\n";
    std::cout << "Suites failed: " << failed << "\n";
    std::cout << "============================\n";

    return (failed == 0) ? 0 : 1;
}
