#include "app/GameApp.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
    bool harnessMode = false;
    bool showWindow = false;
    std::string harnessScript;
    std::string harnessFile;
    std::string screenshotPath;
    std::string statePath;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--harness" && i + 1 < argc) {
            harnessMode = true;
            harnessScript = argv[++i];
        } else if (arg == "--harness-file" && i + 1 < argc) {
            harnessMode = true;
            harnessFile = argv[++i];
        } else if (arg == "--screenshot" && i + 1 < argc) {
            screenshotPath = argv[++i];
        } else if (arg == "--state" && i + 1 < argc) {
            statePath = argv[++i];
        } else if (arg == "--show-window") {
            showWindow = true;
        }
    }

    if (!harnessFile.empty()) {
        std::ifstream in(harnessFile);
        if (!in.is_open()) {
            std::cerr << "Failed to open harness file: " << harnessFile << "\n";
            return 1;
        }
        std::ostringstream ss;
        ss << in.rdbuf();
        if (!harnessScript.empty()) harnessScript += "\n";
        harnessScript += ss.str();
    }

    App::GameApp app;
    if (!app.init(harnessMode && !showWindow)) {
        return 1;
    }

    if (harnessMode) {
        app.runHarnessScript(harnessScript, screenshotPath, statePath, showWindow);
        if (statePath.empty()) {
            std::cout << app.dumpStateJson();
        }
        return 0;
    }

    app.run();
    return 0;
}
