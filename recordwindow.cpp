#include "recordwindow.h"
#include "gameconfig.h"
#include "SelectSongWindow.h" // 【新增】
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

    m_saveBtn = new QPushButton("EXPORT MAP", this);
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

        // 1. 召唤全新的选曲大厅，这样它就会自动重新扫描 maps 文件夹！
        SelectSongWindow *selectWin = new SelectSongWindow();
        selectWin->setAttribute(Qt::WA_DeleteOnClose);
        selectWin->showFullScreen();

        // 2. 安全销毁录音仪
        this->hide();
        this->deleteLater();
    });
    // 【新增】：监听音乐状态，放完直接自动结算！
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia && m_isRecording) {
            onStopAndSave(); // 音乐结束，自动执行导出！
        }
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
// 导出工程逻辑 (完美适配 MapParser 引擎格式)
// ==========================================
void RecordWindow::onStopAndSave() {
    m_isRecording = false;
    m_player->stop();
    m_statusLabel->setText("STATUS: EXPORTING... (正在适配 MapParser)");
    m_statusLabel->setStyleSheet("color: #32CD32; font-size: 24px;");

    if (m_recordedNotes.isEmpty()) {
        QMessageBox::warning(this, "Empty", "你一个音符都没录！");
        return;
    }

    // 1. 统一归档到 maps 文件夹 (跨平台绝对安全版)
    QString baseDir = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
    QDir macDir(baseDir);
    macDir.cdUp(); macDir.cdUp(); macDir.cdUp(); // 跳出 .app 黑盒
    baseDir = macDir.absolutePath();
#endif

    baseDir += "/maps";

    // 【无敌调试法】：打印出录制工具到底保存在了哪里！
    qDebug() << "========================================";
    qDebug() << "【录制保存路径】:" << baseDir;
    qDebug() << "========================================";

    QDir dir(baseDir);
    if (!dir.exists()) dir.mkpath(".");
    QString folderName = baseDir + QString("/CustomMap_%1").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    dir.mkpath(folderName);

    // 2. 生成完全兼容的 info.txt
    QFile infoFile(folderName + "/info.txt");
    if (infoFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&infoFile);
        QFileInfo audioInfo(m_player->source().toLocalFile());

        // 统一音频命名为 music.xxx，对齐你们旧谱面的习惯
        QString suffix = audioInfo.suffix();
        QString standardAudioName = "music." + suffix;

        // 【严格对齐 MapParser 的读取行数！】
        out << "BPM: 60\n";  // 第 1 行：必须是 BPM！我们用 60 来规避数学运算
        out << "SongName: " << audioInfo.completeBaseName() << "\n"; // 第 2 行：歌名
        out << "MusicTime: " << m_player->duration() << "\n"; // 第 3 行：MapParser::getMusicTime() 要用！
        out << "AudioFile: " << standardAudioName << "\n";
        out << "CoverFile: cover.jpg\n";

        infoFile.close();
        QFile::copy(audioInfo.filePath(), folderName + "/" + standardAudioName);
    }

    // 3. 将一个大列表，分别拆装进 Track0.txt 到 Track3.txt
    QFile trackFiles[4];
    QTextStream* trackStreams[4];
    for(int i = 0; i < 4; i++) {
        trackFiles[i].setFileName(folderName + QString("/Track%1.txt").arg(i));
        trackFiles[i].open(QIODevice::WriteOnly | QIODevice::Text);
        trackStreams[i] = new QTextStream(&trackFiles[i]);
    }

    // 4. 数据翻译：毫秒 -> 拍数 (beats)
    for (const RecordNote& note : m_recordedNotes) {
        int tid = note.trackId;
        if(tid < 0 || tid > 3) continue;

        // 【时间魔法】：将 ms 逆运算为 MapParser 想要的 beats
        double startBeats = (note.timeMs / 1000.0) + 1.0;

        if (note.type == 1) {
            // 我们的录音仪: 1=Tap -> MapParser: 0=Tap
            *(trackStreams[tid]) << "0 " << QString::number(startBeats, 'f', 3) << "\n";
        } else if (note.type == 2) {
            // 我们的录音仪: 2=Hold -> MapParser: 1=Hold
            double endBeats = (note.endTime / 1000.0) + 1.0;
            *(trackStreams[tid]) << "1 " << QString::number(startBeats, 'f', 3) << " " << QString::number(endBeats, 'f', 3) << "\n";
        }
    }

    // 清理内存
    for(int i = 0; i < 4; i++) {
        delete trackStreams[i];
        trackFiles[i].close();
    }

    QMessageBox::information(this, "Export Success",
                             QString("完美适配 MapParser！\n共封装 %1 个音符至 4 条轨道。\n已生成至：\n%2")
                                 .arg(m_recordedNotes.size()).arg(folderName));
}
