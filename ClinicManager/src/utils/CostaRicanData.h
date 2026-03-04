#pragma once

#include <QString>
#include <QStringList>

// Static data pools for generating realistic Costa Rican patient records.
// All names, places, and formats follow Costa Rican conventions.
namespace CostaRicanData
{
    // Returns a random valid Costa Rican cédula (9 digits, province 1-9)
    // Uses a simple linear-congruential step seeded by the caller's RNG value.
    QString randomCedula(int seed);

    // First/last name pools
    const QStringList &firstNamesMale();
    const QStringList &firstNamesFemale();
    const QStringList &lastNames();

    // Medical staff names (include title)
    const QStringList &doctorNames();

    // Blood type distribution (approximate Costa Rican population distribution)
    const QStringList &bloodTypes();

    // Common diagnoses
    const QStringList &commonDiagnoses();

    // Medical specialties / areas used in the diagnosis tree
    const QStringList &medicalAreas();

    // Specialty names per area (parallel to medicalAreas())
    const QList<QStringList> &specialtiesByArea();

    // Sample diagnosis names per specialty
    const QList<QStringList> &diagnosesBySpecialty();
}
