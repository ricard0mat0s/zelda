#include "app/GameApp.hpp"

int main() {
    App::GameApp app;
    if (!app.init()) {
        return 1;
    }
    app.run();
    return 0;
}
