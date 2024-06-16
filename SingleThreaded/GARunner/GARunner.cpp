#include "GARunner.h"

GARunner::GARunner(const GASettings &settings, const TargetCheck &targetCheck) : _checker(targetCheck) {
    _gaSettings = settings;
    _generation.resize(settings.populationSize, Individual{ std::string(targetCheck.getTargetSize(), ' ') });
}

void GARunner::runFor(int generationCount) {
    gassert(int(_generation.size()) == _gaSettings.populationSize && "Must be initialized");

    high_resolution_clock::time_point tick = high_resolution_clock::now();
    int lastGen = 0;
    for (int c = 0; generationCount == -1 || c < generationCount; c++) {
        high_resolution_clock::time_point tock = high_resolution_clock::now();
        if (duration_cast<milliseconds>(tock - tick) > 500ms) {
            _genPerSecond = (c - lastGen) * 2;
            lastGen = c;
            tick = tock;
        }
        _completed = float(c + 1) / generationCount;
        prepare();
        render();
        crossover();
        mutate();
    }
    prepare();
}

void GARunner::prepare() {
    for (int c = 0; c < int(_generation.size()); c++) {
        _generation[c].error = _checker.distance(_generation[c].dna);
    }
    std::sort(_generation.begin(), _generation.end(), [](const Individual &a, const Individual &b) {
        return a.error < b.error;
    });
}


void GARunner::render() {
    _renderer.clear();
    for (int c = 0; c < 3; c++) {
        _renderer.renderText(_generation[c].dna.c_str());
        _renderer.renderText(" ");
        _renderer.renderText(_generation[c].error);
        _renderer.newLine();
    }
    _renderer.renderText("Generations per second: ");
    _renderer.renderText(_genPerSecond);
    _renderer.renderText(" completed: ");
    _renderer.renderText((_completed * 10000) / 100);
    _renderer.renderText("%");
    _renderer.newLine();

}

void GARunner::crossover() {
    std::vector<Individual> newGeneration;
    newGeneration.reserve(_generation.size());
    int c = 0;
    const int eliteCount = getEliteCount();
    for (; c < eliteCount; c++) {
        newGeneration.push_back(_generation[c]);
    }

    UniformIntRange parentPick(0, int(_generation.size() - 1));
    for (; c < _generation.size(); c++) {
        const int left = parentPick(rng);
        const int right = parentPick(rng);
        const Individual child = crossover(left, right);
        newGeneration.push_back(child);
    }

    _generation.swap(newGeneration);
}
int GARunner::getEliteCount() const {
    // eliteKeep in percentage
    const int eliteCount = int(_gaSettings.eliteKeep * float(_generation.size()));
    return std::max(1, eliteCount);
}

Individual GARunner::crossover(int left, int right) const {
    const Individual *parents[2] = { &_generation[left], &_generation[right] };
    Individual newIndividual{ std::string(_checker.getTargetSize(), ' ') };
    UniformIntRange parentPick(0, 1);
    for (int c = 0; c < int(newIndividual.dna.size()); c++) {
        newIndividual.dna[c] = parents[parentPick(rng)]->dna[c];
    }
    return newIndividual;
}

void GARunner::mutate() {
    UniformFloatRange mutateChance(0.f, 1.f);
    const int eliteCount = getEliteCount();
    for (int c = eliteCount; c < int(_generation.size()); c++) {
        if (mutateChance(rng) < _gaSettings.mutationRate) {
            mutate(_generation[c]);
        }
    }
}

void GARunner::mutate(Individual &individual) {
    UniformFloatRange chanceMutate(0.f, 1.f);
    for (int c = 0; c < int(individual.dna.size()); c++) {
        if (chanceMutate(rng) < _gaSettings.mutationPower) {
            individual.dna[c] = _checker.newBase(rng);
        }
    }
}