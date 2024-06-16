#include "ConsoleRenderer.h"

void ConsoleRenderer::clear() {
    const auto now = high_resolution_clock::now();

    ++frameCounter;
    if (duration_cast<milliseconds>(now - lastSecond) > 1000ms) {
        lastSecond = now;
        averageFPS = frameCounter - lastSecondFrames;
        lastSecondFrames = frameCounter;
    }
    const int lastFrameFPS = 1s / duration_cast<microseconds>(now - lastFrame);
    lastFrame = now;

    printf("\033[%d;%dH", 0, 0);
    std::cout << std::setfill(' ') << std::setw(clearCount) << ' ';
    printf("\033[%d;%dH", 0, 0);

    clearCount = 0;
    renderText("FPS: ");
    renderText(lastFrameFPS);
    renderText(", frames for last second: ");
    renderText(averageFPS);
    newLine();
}

void ConsoleRenderer::renderText(const char *text) {
    lineLength += std::strlen(text);
    std::cout << text;
}
void ConsoleRenderer::renderText(int value) {
    lineLength += int(log10(value) + 1);
    std::cout << value;
}

void ConsoleRenderer::newLine() {

    struct winsize w;
    
    // Get the terminal size
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        perror("ioctl");
        throw;
    }
    clearCount += ((lineLength / w.ws_col) + 1) * w.ws_col;
    lineLength = 0;
    std::cout << std::endl;
}