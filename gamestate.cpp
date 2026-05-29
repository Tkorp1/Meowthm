#include "gamestate.h"

GameState::GameState(QString _currentSong){
    currentSong = _currentSong;

    currentScore = 0;

    currentAcc = 1.0;

    currentCombo = 0;

    maxCombo = 0;

    currentPerfect = 0;

    currentGood = 0;

    currentMiss = 0;
}

// 2.所有的读取函数
QString GameState::getCurrentSong() const{
    return currentSong;
}

int GameState::getCurrentScore() const{
    return currentScore;
}

double GameState::getCurrentAcc() const{
    return currentAcc;
}

int GameState::getCurrentCombo() const{
    return currentCombo;
}

int GameState::getMaxCombo() const{
    return maxCombo;
}

int GameState::getCurrentPerfect() const{
    return currentPerfect;
}

int GameState::getCurrentGood() const{
    return currentGood;
}

int GameState::getCurrentMiss() const{
    return currentMiss;
}

QVector<int> GameState::getAllJudge() const{
    return allJudge;
}
/*
    if(result == 1){ // Miss
        // 连击中断

        currentCombo = 0;
        //comboLabel->show();
    }else if(result >= 2){
        currentCombo++;
        // 目前暂定给分，之后调整
        currentScore += (result == 3) ? 100 : 70;
        comboLabel -> setText(QString("Combo: %1").arg(currentCombo));
        //comboLabel->show();
    }
*/
// 3.修改函数
void GameState::changeCurrentState(int judge){
    switch(judge){
    case 1:
        // 现在是miss
        currentCombo = 0;
        currentMiss++;
        currentAcc = ((double)currentPerfect + (double)currentGood * 0.65)/((double)(currentPerfect + currentGood + currentMiss));
        allJudge.append(judge);
        break;
    case 2:
        // 现在是good
        currentCombo++;
        maxCombo = (maxCombo > currentCombo ? maxCombo : currentCombo);
        currentGood++;
        currentScore+=70;
        currentAcc = ((double)currentPerfect + (double)currentGood * 0.65)/((double)(currentPerfect + currentGood + currentMiss));
        allJudge.append(judge);
        break;
    case 3:
        // 现在是perfect
        currentCombo++;
        maxCombo = (maxCombo > currentCombo ? maxCombo : currentCombo);
        currentPerfect++;
        currentScore+=100;
        currentAcc = ((double)currentPerfect + (double)currentGood * 0.65)/((double)(currentPerfect + currentGood + currentMiss));
        allJudge.append(judge);
        break;
    default:
        break;
    }
}