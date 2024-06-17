#include "GARunner.h"

GASettings GARunner::_gaSettings;
std::vector<Individual> GARunner::_generation;
//std::mt19937 GARunner::rng{42};
ConsoleRenderer GARunner::_renderer;
TargetCheck GARunner::_checker;
ThreadPool GARunner::_threadPool;

GARunner::GARunner(const GASettings &settings, const TargetCheck &targetCheck, size_t threadCount) {
    _gaSettings = settings;
    _generation.resize(settings.populationSize, Individual{ std::string(targetCheck.getTargetSize(), ' ') });
    _threadCount = threadCount;
    _threadsChunks.reserve(threadCount);
    _checker = targetCheck;
    assignThreadChunkForThreadsValue();
    _threadPool.initialize(threadCount);
}

void GARunner::assignThreadChunkForThreadsValue() {
    int chunkSize = _generation.size() / _threadCount;
    int remainder = _generation.size() % _threadCount;
    int start = 0;

    for (int threadIndex = 0; threadIndex < _threadCount; ++threadIndex) {
        int additional = (threadIndex < remainder) ? 1 : 0;
        int chunkStart = start;
        int chunkEnd = start + chunkSize + additional - 1;
        _threadsChunks.emplace_back(chunkStart, chunkEnd);
        start = chunkEnd + 1;
    }
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
        // if(_generation[0].error == 0) {
        //     break;
        // }

        prepare();
        render();

        std::vector<std::thread> threads;
        std::vector<Individual> threadedGenerations = _generation;
        for(size_t thCnt = 0; thCnt < _threadCount; thCnt++) {
            //std::vector<Individual> a = threadedGenerations;
            _threadPool.enqueue(processBatchGeneration, std::ref(threadedGenerations), _threadsChunks[thCnt].first, _threadsChunks[thCnt].second);
        }

        _threadPool.waitForCompletion();
        threadedPrepare(threadedGenerations);
        _generation.swap(threadedGenerations);
    }
    prepare();
}

void GARunner::processBatchGeneration(std::vector<Individual>& threadGeneration, size_t batch_start, size_t batch_end) {
    thread_local std::mt19937 rng{42}; // Each thread gets its own RNG
    crossover(threadGeneration, rng, batch_start, batch_end);
    mutate(threadGeneration, rng, batch_start, batch_end);
}

void GARunner::threadedPrepare(std::vector<Individual>& threadedGenerations) {
    for (int c = 0; c < int(threadedGenerations.size()); c++) {
        threadedGenerations[c].error = _checker.distance(threadedGenerations[c].dna);
    }
    std::sort(threadedGenerations.begin(), threadedGenerations.end(), [](const Individual &a, const Individual &b) {
        return a.error < b.error;
    });
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

void GARunner::crossover(std::vector<Individual>& threadGeneration, std::mt19937& rng, size_t batchStart, size_t batchEnd) {
    int c = getEliteCount();
    if(batchStart > c) {
        c = batchStart;
    }

    UniformIntRange parentPick(0, int(_generation.size() - 1));
    for (; c < batchEnd; c++) {
        const int left = parentPick(rng);
        const int right = parentPick(rng);
        const Individual child = crossover(left, right, rng);
        threadGeneration[c] = child;
    }
}

int GARunner::getEliteCount() {
    // eliteKeep in percentage
    const int eliteCount = int(_gaSettings.eliteKeep * float(_generation.size()));
    return std::max(1, eliteCount);
}

Individual GARunner::crossover(int left, int right, std::mt19937& rng) {
    const Individual *parents[2] = { &_generation[left], &_generation[right] };
    Individual newIndividual{ std::string(_checker.getTargetSize(), ' ') };
    UniformIntRange parentPick(0, 1);
    for (int c = 0; c < int(newIndividual.dna.size()); c++) {
        int val = parentPick(rng);
        newIndividual.dna[c] = parents[val]->dna[c];
    }
    return newIndividual;
}

void GARunner::mutate(std::vector<Individual>& threadGeneration, std::mt19937& rng, size_t batchStart, size_t batchEnd) {
    UniformFloatRange mutateChance(0.f, 1.f);
    int c = getEliteCount();
    if(batchStart > c) {
        c = batchStart;
    }
    
    for (; c < batchEnd; c++) {
        if (mutateChance(rng) < _gaSettings.mutationRate) {
            mutate(threadGeneration[c], rng);
        }
    }
}

void GARunner::mutate(Individual &individual, std::mt19937& rng) {
    UniformFloatRange chanceMutate(0.f, 1.f);
    for (int c = 0; c < int(individual.dna.size()); c++) {
        if (chanceMutate(rng) < _gaSettings.mutationPower) {
            individual.dna[c] = _checker.newBase(rng);
        }
    }
}