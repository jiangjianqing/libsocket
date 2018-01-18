#ifndef MYEVENT_H
#define MYEVENT_H

#include <QEvent>

class MyEvent : public QEvent
{
public:
    explicit MyEvent(QEvent::Type type);

    void SetValue(int nValue)
        {
            m_nValue = nValue;
        }

    int GetValue()
        {
            return m_nValue;
        }

private:
    int m_nValue;
};

#endif // MYEVENT_H
