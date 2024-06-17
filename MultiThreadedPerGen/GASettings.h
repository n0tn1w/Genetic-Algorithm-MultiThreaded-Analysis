#pragma once

struct GASettings {
    int populationSize = 2048;
    float eliteKeep = 0.05f;
    float mutationRate = 0.03f;
    float mutationPower = 0.1f;

    // int populationSize = 10;
    // float eliteKeep = 0.1f;
    // float mutationRate = 0.99f;
    // float mutationPower = 0.99f;
};