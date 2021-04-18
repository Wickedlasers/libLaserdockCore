/**
    libLaserdockCore
    Copyright(c) 2018 Wicked Lasers

    This file is part of libLaserdockCore.

    libLaserdockCore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libLaserdockCore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libLaserdockCore.  If not, see <https://www.gnu.org/licenses/>.
**/

#include "ldCore/Helpers/Text/ldTicker.h"

#include <QtCore/QDebug>

#include <ldCore/Helpers/Text/ldTextLabel.h>

ldTicker::ldTicker()
    : m_textLabel(new ldTextLabel())
{
    // initial pos, right
    m_textLabel->setPosition(ldVec2(1.f, calcTextY()));
}

ldTicker::~ldTicker()
{
}

// calculate centered text position when speed is set to 0
void ldTicker::updateXPos()
{
    if (cmpf(m_speed,0.0f)){
        float xpos = 0.5f-(m_textLabel->getWidth()/2.0f);
        if (xpos<0.0f) xpos = 0.0f;
        m_textLabel->setPosition(ldVec2(xpos, m_textLabel->getPosition().y));
    }
}

void ldTicker::setMessage(const QString &message)
{
    setMessages(QStringList{message});
}

void ldTicker::setMessages(const QStringList &messages)
{
    if(messages == m_messages)
        return;

    m_currentMessageIndex = 0;
    m_messages = messages;

    updateCurrentMessage();
}


void ldTicker::setFontSize(float fontSize)
{
    m_textLabel->setFontSize(fontSize);
    // center height pos
    m_textLabel->setPosition(ldVec2(m_textLabel->getPosition().x, calcTextY()));
    updateXPos();
}

float ldTicker::getFontSize() const
{
    return m_textLabel->getFontSize();
}

void ldTicker::setFont(int font)
{
    m_textLabel->setFont(font);
    updateXPos();
}

int ldTicker::font() const
{
    return m_textLabel->font();
}

double ldTicker::letterSpace() const
{
    return m_textLabel->letterSpace();
}

void ldTicker::setLetterSpace(double letterSpace)
{
    m_textLabel->setLetterSpace(letterSpace);
    updateXPos();
}

float ldTicker::getHeight() const
{
    return m_textLabel->getHeight();
}

void ldTicker::setCenterY(float y)
{
    m_centerY = y;
    m_textLabel->setPosition(ldVec2(m_textLabel->getPosition().x, calcTextY()));
}

ldBezierCurveDrawer *ldTicker::drawer() const
{
    return m_textLabel->drawer();
}

void ldTicker::restart()
{
    m_textLabel->setPosition(ldVec2(1.f, m_textLabel->getPosition().y));
}

float ldTicker::calcTextY()
{
    return (m_centerY - m_textLabel->getHeight())/2.0f;
}

void ldTicker::setSpeed(float speed)
{
    m_speed = speed;
    updateXPos();
}

float ldTicker::speed() const
{
    return m_speed;
}

void ldTicker::draw(ldRendererOpenlase *p_renderer)
{
    // calc next x
    float x = (m_textLabel->getPosition().x < -m_textLabel->getWidth())
                  ? 1.f
                  : m_textLabel->getPosition().x - m_speed;

    // move x
    m_textLabel->setPosition(ldVec2(x, m_textLabel->getPosition().y));

    // if finished put next message
    bool isFinished = (m_textLabel->getPosition().x == 1.f);
    if(isFinished) {
        m_currentMessageIndex++;
        updateCurrentMessage();
    }

    // draw
    m_textLabel->innerDraw(p_renderer);
}

void ldTicker::updateCurrentMessage()
{
    if(m_currentMessageIndex >= m_messages.size())
        m_currentMessageIndex = 0;

    QString text = m_currentMessageIndex >= m_messages.size() ? "No text" : m_messages[m_currentMessageIndex];
    text.replace('\n', '\t');
    m_textLabel->setText(text);
    restart();
    updateXPos();
}
