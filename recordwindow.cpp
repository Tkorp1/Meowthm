#include "RecordWindow.h"
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

RecordWindow::RecordWindow(QWidget *parent) : QWidget(parent)
{
    // 全屏无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    showFullScreen();

    m_isRecording = false;

    // ==========================================
    // 音频引擎初始化
    // ==========================================
    m_audioOutput = new QAudioOutput(this);
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.8);

    // 【TODO】：这里填入你想要制谱的歌曲路径
    m_player->setSource(QUrl("qrc:/sound/sounds/test_song.mp3"));

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
    // 1. 清空上一次的残余记录
    m_recordedNotes.clear();
    m_countLabel->setText("RECORDED NOTES: 0");

    // 2. 状态切换
    m_isRecording = true;
    m_statusLabel->setText("STATUS: RECORDING... (正在录制中，请随音乐敲击 D/F/J/K)");
    m_statusLabel->setStyleSheet("color: #FF4500; font-size: 24px; font-weight: bold;"); // 红色警告色

    // 3. 从头开始放音乐
    m_player->setPosition(0);
    m_player->play();

    // 焦点抢回来，准备接收键盘
    this->setFocus();
}

void RecordWindow::keyPressEvent(QKeyEvent *event) {
    // 防呆：长按不放不应该算作无限个音符
    if (event->isAutoRepeat()) return;

    if (m_isRecording) {
        int trackId = -1;
        if (event->key() == Qt::Key_D) trackId = 0;
        else if (event->key() == Qt::Key_F) trackId = 1;
        else if (event->key() == Qt::Key_J) trackId = 2;
        else if (event->key() == Qt::Key_K) trackId = 3;

        // 如果按下了音游按键，立马记录下当前的时间戳！
        if (trackId != -1) {
            RecordNote note;
            note.trackId = trackId;
            note.timeMs = m_player->position(); // 【核心魔法】获取当前音乐播放到了哪一毫秒！

            m_recordedNotes.append(note);

            // 更新 UI 计数器
            m_countLabel->setText(QString("RECORDED NOTES: %1").arg(m_recordedNotes.size()));
        }
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
            out << note.trackId << " " << note.timeMs << "\n";
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

        out << "SongName: 新建自制谱面\n";
        out << "Composer: Unknown\n";
        out << "Mapper: " << GameConfig::instance()->getCurrentPlayer() << "\n"; // 甚至可以自动填入玩家的ID！
        out << "AudioFile: audio.mp3\n"; // 占位符，提示玩家把音乐放进来
        out << "CoverFile: cover.jpg\n"; // 占位符
        out << "Offset: 0\n";

        infoFile.close();
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
