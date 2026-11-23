#include <iostream>

const int kMoistureMin = 0;
const int kMoistureMax = 100;
const int kOpenLimit = 60;
const int kPulseLimit = 85;
const int kPulseStep = 20;
const int kPulseVolume = 5;
const int kOperationLimit = 8;

enum class Action {
    Exit = 0,
    Status = 1,
    Open = 2,
    Close = 3,
    Pulse = 4
};

enum class ExitCode {
    Ok = 0,
    InputError = 1,
    InvalidMoisture = 2
};

struct Session {
    int moisture;
    bool valveOpen;
    int operations;
    int waterUsed;
};

void printTelemetry(const Session& session) {
    std::cout << "Valve open: " << session.valveOpen << '\n';
    std::cout << "Moisture: " << session.moisture << "%\n";
    std::cout << "Water used: " << session.waterUsed << " L\n";
}

bool canOpenValve(const Session& session) {
    return !session.valveOpen && session.moisture < kOpenLimit;
}

bool canPulseWater(const Session& session) {
    return !session.valveOpen && session.moisture < kPulseLimit;
}
void openValve(Session& session) {
    session.valveOpen = true;
    ++session.operations;
}

void closeValve(Session& session) {
    session.valveOpen = false;
    ++session.operations;
}

void pulseWater(Session& session) {
    session.moisture += kPulseStep;
    if (session.moisture > kMoistureMax) {
        session.moisture = kMoistureMax;
    }
    session.waterUsed += kPulseVolume;
    ++session.operations;
}

void showStatus(Session& session) {
    ++session.operations;
}

ExitCode readInt(const char* prompt, int& value) {
    std::cout << prompt;
    if (!(std::cin >> value)) {
        std::cout << "Input error\n";
        return ExitCode::InputError;
    }
    return ExitCode::Ok;
}

int main() {
    std::cout << std::boolalpha;

    int moisture = 0;
    if (readInt("Moisture: ", moisture) != ExitCode::Ok) {
        return static_cast<int>(ExitCode::InputError);
    }
    if (moisture < kMoistureMin || moisture > kMoistureMax) {
        std::cout << "Invalid moisture\n";
        return static_cast<int>(ExitCode::InvalidMoisture);
    }

    Session session{ moisture, false, 0, 0 };

    while (session.operations < kOperationLimit) {
        int code = 0;
        if (readInt("Action 0/1/2/3/4: ", code) != ExitCode::Ok) {
            return static_cast<int>(ExitCode::InputError);
        }

        if (code == static_cast<int>(Action::Exit)) {
            break;
        }
        else if (code == static_cast<int>(Action::Status)) {
            showStatus(session);
            std::cout << "Status\n";
            printTelemetry(session);
        }
        else if (code == static_cast<int>
            (Action::Open)) {
            if (canOpenValve(session)) {
                openValve(session);
                std::cout << "Valve opened\n";
                printTelemetry(session);
            }
            else {
                std::cout << "Open denied\n";
            }
        }
        else if (code == static_cast<int>(Action::Close)) {
            closeValve(session);
            std::cout << "Valve closed\n";
            printTelemetry(session);
        }
        else if (code == static_cast<int>(Action::Pulse)) {
            if (canPulseWater(session)) {
                pulseWater(session);
                std::cout << "Pulse completed\n";
                printTelemetry(session);
            }
            else {
                std::cout << "Pulse denied\n";
            }
        }
        else {
            std::cout << "Unknown action\n";
        }
    }

    std::cout << "Operations: " << session.operations << '\n';
    std::cout << "Total water: " << session.waterUsed << " L\n";
    return static_cast<int>(ExitCode::Ok);
}