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


// 数据分析
void GameState::recordHit(int trackId, qint64 deltaT, qint64 musicTime) {
    m_hitOffsets.append(deltaT);
    m_hitTimestamps.append(musicTime);

    // 轨道 0(D), 1(F) 算左手；轨道 2(J), 3(K) 算右手
    if (trackId == 0 || trackId == 1) {
        m_leftHandHits++;
    } else if (trackId == 2 || trackId == 3) {
        m_rightHandHits++;
    }
}

QList<qint64> GameState::getHitOffsets() const { return m_hitOffsets; }
QList<qint64> GameState::getHitTimestamps() const { return m_hitTimestamps; }
int GameState::getLeftHandHits() const { return m_leftHandHits; }
int GameState::getRightHandHits() const { return m_rightHandHits; }
