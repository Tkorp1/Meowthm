#include "recordwindow.h"
#include "gameconfig.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>

RecordWindow::RecordWindow(QWidget *parent) : QWidget(parent)
{
    // 全屏无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    showFullScreen();

    m_isRecording = false;
    for (int i = 0; i < 4; ++i) m_pressTime[i] = -1;

    // ==========================================
    // 音频引擎初始化
    // ==========================================
    m_audioOutput = new QAudioOutput(this);
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.8);

    // // 【TODO】：这里填入你想要制谱的歌曲路径
    // m_player->setSource(QUrl("qrc:/sound/sounds/test_song.mp3"));

    // ==========================================
    // 极简科幻 UI 布局
    // ==========================================
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(30);

    QLabel *title = new QLabel("LIVE RECORD TERMINAL // 盲打录制仪", this);
    title->setStyleSheet("color: #00BFFF; font-size: 40px; font-weight: 900; letter-spacing: 5px;");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    m_statusLabel = new QLabel("STATUS: STANDBY (准备就绪)", this);
    m_statusLabel->setStyleSheet("color: #A9A9A9; font-size: 24px;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_statusLabel);

    m_countLabel = new QLabel("RECORDED NOTES: 0", this);
    m_countLabel->setStyleSheet("color: white; font-size: 20px;");
    m_countLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_countLabel);

    // 按钮区
    QHBoxLayout *btnLayout = new QHBoxLayout();

    m_startBtn = new QPushButton("▶ START RECORD", this);
    m_startBtn->setFixedSize(250, 60);
    m_startBtn->setStyleSheet("QPushButton { background-color: rgba(0, 191, 255, 50); border: 2px solid #00BFFF; color: white; font-size: 20px; font-weight: bold; border-radius: 10px; } QPushButton:hover { background-color: #00BFFF; color: black; }");

    m_saveBtn = new QPushButton("💾 EXPORT MAP", this);
    m_saveBtn->setFixedSize(250, 60);
    m_saveBtn->setStyleSheet("QPushButton { background-color: rgba(50, 205, 50, 50); border: 2px solid #32CD32; color: white; font-size: 20px; font-weight: bold; border-radius: 10px; } QPushButton:hover { background-color: #32CD32; color: black; }");

    m_backBtn = new QPushButton("◀ RETURN", this);
    m_backBtn->setFixedSize(250, 60);
    m_backBtn->setStyleSheet("QPushButton { background-color: transparent; border: 2px solid #f44336; color: #f44336; font-size: 20px; font-weight: bold; border-radius: 10px; } QPushButton:hover { background-color: #f44336; color: white; }");

    btnLayout->addStretch();
    btnLayout->addWidget(m_startBtn);
    btnLayout->addWidget(m_saveBtn);
    btnLayout->addWidget(m_backBtn);
    btnLayout->addStretch();

    mainLayout->addLayout(btnLayout);

    // ==========================================
    // 信号连接
    // ==========================================
    connect(m_startBtn, &QPushButton::clicked, this, &RecordWindow::onStartRecord);
    connect(m_saveBtn, &QPushButton::clicked, this, &RecordWindow::onStopAndSave);
    connect(m_backBtn, &QPushButton::clicked, this, [this](){
        m_player->stop();
        this->deleteLater();
        // 如果这里要退回主菜单，记得加上 new MainWindow(); 之类的
    });

    this->setFocusPolicy(Qt::StrongFocus);
}

RecordWindow::~RecordWindow() {
    m_player->stop();
}

void RecordWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    // 画一个简单的深色背景
    painter.fillRect(rect(), QColor(20, 20, 25));
}

// ==========================================
// 盲打录制核心逻辑
// ==========================================
void RecordWindow::onStartRecord() {
    // 1. 【新增】：如果还没选歌，先弹窗让用户选
    if (m_player->source().isEmpty()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        "选择音频文件",
                                                        "",
                                                        "Audio Files (*.mp3 *.wav *.ogg)");

        if (fileName.isEmpty()) return; // 用户取消了选择

        m_player->setSource(QUrl::fromLocalFile(fileName));
    }

    // 2. 清空上一次的残余记录
    m_recordedNotes.clear();
    m_countLabel->setText("RECORDED NOTES: 0");

    // ... 后面的逻辑保持不变 ...
    m_isRecording = true;
    m_player->setPosition(0);
    m_player->play();
}

void RecordWindow::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat() || !m_isRecording) return;

    int trackId = -1;
    if (event->key() == Qt::Key_D) trackId = 0;
    else if (event->key() == Qt::Key_F) trackId = 1;
    else if (event->key() == Qt::Key_J) trackId = 2;
    else if (event->key() == Qt::Key_K) trackId = 3;

    // 按下时：不立刻生成音符，而是把当前时间“存”在对应的轨道里
    if (trackId != -1 && m_pressTime[trackId] == -1) {
        m_pressTime[trackId] = m_player->position();
    }
}

void RecordWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat() || !m_isRecording) return;

    int trackId = -1;
    if (event->key() == Qt::Key_D) trackId = 0;
    else if (event->key() == Qt::Key_F) trackId = 1;
    else if (event->key() == Qt::Key_J) trackId = 2;
    else if (event->key() == Qt::Key_K) trackId = 3;

    // 松开时：结账！计算按了多久！
    if (trackId != -1 && m_pressTime[trackId] != -1) {
        qint64 startTime = m_pressTime[trackId];
        qint64 endTime = m_player->position();
        qint64 duration = endTime - startTime;

        RecordNote note;
        note.trackId = trackId;
        note.timeMs = startTime;

        // 【分水岭】：150毫秒
        if (duration < 150) {
            note.type = 1; // 这是一个 Tap
            note.endTime = startTime; // Tap 的结束时间等于开始时间（或者填0也可以）
        } else {
            note.type = 2; // 这是一个 Hold 长条！
            note.endTime = endTime;
        }

        m_recordedNotes.append(note);

        // 结账完毕，清空这个轨道的按下状态
        m_pressTime[trackId] = -1;

        // 更新 UI
        m_countLabel->setText(QString("RECORDED NOTES: %1").arg(m_recordedNotes.size()));
    }
}


// ==========================================
// 导出工程逻辑 (生成完整的谱面文件夹)
// ==========================================
void RecordWindow::onStopAndSave() {
    m_isRecording = false;
    m_player->stop();
    m_statusLabel->setText("STATUS: EXPORTING... (正在封装谱面工程)");
    m_statusLabel->setStyleSheet("color: #32CD32; font-size: 24px;");

    if (m_recordedNotes.isEmpty()) {
        QMessageBox::warning(this, "Empty", "你一个音符都没录！");
        return;
    }

    // 1. 生成一个专属的“谱面工程文件夹”
    QString folderName = QString("CustomMap_%1").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    QDir dir;
    if (!dir.exists(folderName)) {
        dir.mkpath(folderName); // 自动创建文件夹
    }

    // 2. 生成音符数据文件 (假设你们叫 notes.txt)
    QFile notesFile(folderName + "/notes.txt");
    if (notesFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&notesFile);

        // 按照你们的格式写入音符数据
        out << m_recordedNotes.size() << "\n";
        for (const RecordNote& note : m_recordedNotes) {
            // 现在的格式：[类型 1=Tap/2=Hold] [轨道ID] [起始时间] [结束时间]

            out << note.type << " " << note.trackId << " " << note.timeMs << " " << note.endTime << "\n";

        }
        notesFile.close();
    } else {
        QMessageBox::critical(this, "Error", "音符数据写入失败！");
        return;
    }

    // 3. 自动生成曲目配置文件 (假设你们叫 info.txt 或 config.ini)
    // 这里提前帮你们的 MapParser 把壳子搭好！
    QFile infoFile(folderName + "/info.txt");
    if (infoFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&infoFile);
        QFileInfo audioInfo(m_player->source().toLocalFile());

        out << "SongName: 新建自制谱面\n";
        out << "Composer: Unknown\n";
        out << "Mapper: " << GameConfig::instance()->getCurrentPlayer() << "\n"; // 甚至可以自动填入玩家的ID！
        out << "AudioFile: audio.mp3\n"; // 占位符，提示玩家把音乐放进来
        out << "CoverFile: cover.jpg\n"; // 占位符
        out << "Offset: 0\n";

        infoFile.close();
        QFile::copy(audioInfo.filePath(), folderName + "/" + audioInfo.fileName());
    }

    // 4. 完美收官提示
    QString successMsg = QString(
                             "制谱成功！共录制了 %1 个音符。\n\n"
                             "已在游戏目录下生成谱面工程：\n【%2】\n\n"
                             "里面包含了 notes.txt 和 info.txt。\n"
                             "下一步：请手动将对应的 .mp3 音乐和曲绘图片放入该文件夹中，即可使用！"
                             ).arg(m_recordedNotes.size()).arg(folderName);

    QMessageBox::information(this, "Export Success", successMsg);
}
