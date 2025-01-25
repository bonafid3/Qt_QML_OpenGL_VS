#pragma once

#include <QObject>
#include <iostream>
#include <chrono>

class FPSCounter : public QObject
{
	Q_OBJECT

    Q_PROPERTY(float fps     READ fps     NOTIFY fpsChanged)

public:
    FPSCounter(QObject* parent=nullptr) : QObject(parent), frameCount(0), lastTime(std::chrono::high_resolution_clock::now()), mFPS(0.0) {}

    // Call this every frame
    void update() {
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsedTime = currentTime - lastTime;

        // Update FPS every 1 second
        if (elapsedTime.count() >= 1.0f) {
            mFPS = frameCount / elapsedTime.count(); // Frames per second
            frameCount = 0; // Reset frame count
            lastTime = currentTime; // Reset timer
            Q_EMIT fpsChanged();
        }
    }

    // Get the current FPS value
    float fps() const {
        return mFPS;
    }
Q_SIGNALS:
    void fpsChanged();
private:
    int frameCount; // Number of frames rendered
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime; // Last time FPS was updated
    float mFPS; // Current FPS value
};
