// UIManager.h
#pragma once

#include "aleUIbutton.h"

#include <vector>

class UIManager {
public:
    void AddButton(UIButton* button);
    void Update(almond::EventSystem& eventSystem);
    //void Render(BasicRenderer& renderer);

private:
    std::vector<UIButton*> buttons;
};
