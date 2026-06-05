#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginBtn;
    QPushButton *m_registerBtn;
    QPushButton *m_closeBtn;
};

#endif // LOGINWINDOW_H
