#ifndef PROFILEWINDOW_H
#define PROFILEWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
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
    // 昵称相关
    void onNicknameTriggerClicked();  // 修改昵称按钮
    void onNicknameSaveClicked();     // 保存昵称

    // 简介相关（多行文本）
    void onBioTriggerClicked();       // 编辑简介按钮
    void onBioSaveClicked();          // 保存简介

    // 返回主界面
    void onBackClicked();

private:
    // 昵称控件
    QPushButton *m_nicknameTriggerBtn;
    QLineEdit   *m_nicknameEdit;
    QPushButton *m_nicknameSaveBtn;
    QLabel      *m_nicknameDisplayLabel;

    // 简介控件（多行文本）
    QPushButton *m_bioTriggerBtn;
    QTextEdit   *m_bioEdit;
    QPushButton *m_bioSaveBtn;
    QLabel      *m_bioDisplayLabel;

    // 返回按钮
    QPushButton *m_backBtn;

    // 预留功能：显示rating
    // 预留功能：根据获取的成就更改背景图
    // 预留功能：根据获取的成就更改头像
};

#endif // PROFILEWINDOW_H
