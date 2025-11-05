//
// Created by chen2 on 2025/10/28.
//

#include "ui_conf.h"

uint8_t question1Flag;
uint8_t question2Flag;
uint8_t question3Flag;

uint8_t getQuestionFlag() {
    question1Flag = getUIQuestion1Flag();
    question2Flag = getUIQuestion2Flag();
    question3Flag = getUIQuestion3Flag();

    if (question1Flag == 1 && question2Flag == 0 && question3Flag == 0) {
        return 1;
    } else if (question2Flag == 1 && question1Flag == 0 && question3Flag == 0) {
        return 2;
    } else if (question3Flag == 1 && question1Flag == 0 && question2Flag == 0) {
        return 3;
    } else {
        return 0;
    }
}