#include "User.h"

User::User()
{
    this->chat = nullptr;
}

User::User(const QString &name, QTextEdit *chat)
{
    this->name = name;
    this->chat = chat;
}

User::User(const User &other)
{
    this->name = other.name;
    if (other.chat == nullptr)
    {
        this->chat = nullptr;
    }
    else
    {
        this->chat = new QTextEdit();
        this->chat->setFont(other.chat->font());
        this->chat->setReadOnly(other.chat->isReadOnly());
    }
}

User &User::operator=(const User &other)
{
    this->name = other.name;
    if (other.chat == nullptr)
    {
        this->chat = nullptr;
    }
    else
    {
        if (this->chat != nullptr) delete this->chat;
        this->chat = new QTextEdit();
        this->chat->setFont(other.chat->font());
        this->chat->setReadOnly(other.chat->isReadOnly());
    }
    return *this;
}

User::~User()
{
    if (chat != nullptr) delete chat;
}

