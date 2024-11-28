#ifndef USER_H
#define USER_H

#include <QString>
#include <QTextEdit>

class User
{
private:
    QString name;
    QTextEdit *chat;

public:
    User();
    User(const QString &name, QTextEdit *chat);
    User(const User &other);
    User &operator=(const User &other);
    ~User();

    inline void SetName(const QString &name) { this->name = name; }
    inline QString GetName() const { return name; }
    inline QTextEdit *GetChat() { return chat; }
};

#endif // USER_H
