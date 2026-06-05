#ifndef PROFILEWINDOW_H
#define PROFILEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>

class ProfileWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ProfileWindow(QWidget *parent = nullptr);
    ~ProfileWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onEditNickname();
    void onSaveNickname();
    void onEditBio();
    void onSaveBio();
    void onLoginClicked(); // 预留的登录/注册接口

private:
    // UI 控件指针
    QLabel *m_avatarLabel;

    // 昵称模块
    QLabel *m_nicknameDisplay;
    QLineEdit *m_nicknameEdit;
    QPushButton *m_nicknameEditBtn;
    QPushButton *m_nicknameSaveBtn;

    // 简介模块
    QLabel *m_bioDisplay;
    QTextEdit *m_bioEdit;
    QPushButton *m_bioEditBtn;
    QPushButton *m_bioSaveBtn;
};

#endif // PROFILEWINDOW_H
