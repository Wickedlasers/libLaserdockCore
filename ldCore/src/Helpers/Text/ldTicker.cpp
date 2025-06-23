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
}

ldTicker::~ldTicker()
{
}

// calculate centered text position when speed is set to 0
void ldTicker::updateXPos()
{
    if (cmpf(m_speed,0.0f)){
        if(m_direction < ldTextDirection::VerticalDivider) {
            float xpos = 0.5f-(m_textLabel->getWidth()/2.0f);

            if(m_direction == ldTextDirection::Right) {
                if (xpos<0.01f) xpos = -1*m_textLabel->getWidth()+0.96f;
            } else {
                if (xpos<0.01f) xpos = 0.01f;
            }
            m_textLabel->setPosition(ldVec2(xpos, m_textLabel->getPosition().y));
        } else {
            float ypos = 0.5f-(m_textLabel->getHeight()/2.0f);

            float bottomBorder = 0.01;

            if(m_direction == ldTextDirection::Down) {
                if (ypos < bottomBorder) ypos = 0.01f;
            } else {
                if (ypos < bottomBorder) ypos = 0.99f - m_textLabel->getHeight();
            }
            m_textLabel->setPosition(ldVec2(calcTextX(), ypos));
        }
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

    // center height pos
    if(m_direction < ldTextDirection::VerticalDivider) {
        m_textLabel->setPosition(ldVec2(m_textLabel->getPosition().x, calcTextY()));
    } else {
        m_textLabel->setPosition(ldVec2(calcTextX(), m_textLabel->getPosition().y));
    }
}


void ldTicker::setFontSize(float fontSize)
{
    m_textLabel->setFontSize(fontSize);
    // center height pos
    if(m_direction < ldTextDirection::VerticalDivider) {
        m_textLabel->setPosition(ldVec2(m_textLabel->getPosition().x, calcTextY()));
    } else {
        m_textLabel->setPosition(ldVec2(calcTextX(), m_textLabel->getPosition().y));
    }
    updateXPos();
}

float ldTicker::getFontSize() const
{
    return m_textLabel->getFontSize();
}

void ldTicker::setFont(const QString &fontFamily)
{
    m_textLabel->setFont(fontFamily);
    updateXPos();
}

QString ldTicker::font() const
{
    return m_textLabel->font();
}

float ldTicker::letterSpace() const
{
    return m_textLabel->letterSpace();
}

void ldTicker::setLetterSpace(float letterSpace)
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

ldOLDrawer *ldTicker::drawer() const
{
    return m_textLabel->drawer();
}

void ldTicker::restart()
{
    if(m_direction < ldTextDirection::VerticalDivider) {
        float borderValue = m_direction == ldTextDirection::Right ? getLeftBorderPos()  : getRightBorderPos();
        m_textLabel->setPosition(ldVec2(borderValue, calcTextY()));
    } else {
        float borderValue = m_direction == ldTextDirection::Down
                ? getUpBorderPos()
                : getDownBorderPos();
        m_textLabel->setPosition(ldVec2(calcTextX(), borderValue));
    }

    updateXPos();
}

ldTicker *ldTicker::clone() const
{
    auto newTicker = new ldTicker();
    newTicker->m_textLabel.reset(m_textLabel->clone());
    newTicker->m_speed = m_speed;
    newTicker->m_direction = m_direction;
    newTicker->m_messages = m_messages;
    newTicker->m_currentMessageIndex = m_currentMessageIndex;
    newTicker->m_centerX = m_centerX;
    newTicker->m_centerY = m_centerY;

    return newTicker;
}

float ldTicker::calcTextX()
{
    return (m_centerX - m_textLabel->getWidth())/2.0f;
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

void ldTicker::setDirection(ldTextDirection::Direction direction)
{
    m_direction = direction;
    bool isVertical = m_direction > 1;
    m_textLabel->setVertical(isVertical);
    updateCurrentMessage();
}

ldTextDirection::Direction ldTicker::direction() const
{
    return m_direction;
}

bool ldTicker::draw(ldRendererOpenlase *p_renderer)
{
    bool isFinished = false;
    if(m_direction < ldTextDirection::VerticalDivider) {
        isFinished = drawHorizontal();
    } else {
        isFinished = drawVertical();
    }

    if(isFinished) {
        m_currentMessageIndex++;
        isFinished = updateCurrentMessage();
    }

    // draw
    m_textLabel->innerDraw(p_renderer);

    return isFinished;
}

bool ldTicker::drawHorizontal()
{
    // calc next x
    float borderValue = m_direction == ldTextDirection::Right ? getLeftBorderPos() : getRightBorderPos();

    float x = 0;
    if(m_direction == ldTextDirection::Right) {
        x = (m_textLabel->getPosition().x > getRightBorderPos())
                ? borderValue
                : m_textLabel->getPosition().x + m_speed;
    } else {
        x = (m_textLabel->getPosition().x < getLeftBorderPos())
                ? borderValue
                : m_textLabel->getPosition().x - m_speed;
    }

    // move x
    m_textLabel->setPosition(ldVec2(x, m_textLabel->getPosition().y));

    // if finished put next message
    return (m_textLabel->getPosition().x == borderValue);
}

bool ldTicker::drawVertical()
{
    // calc next x
    float borderValue = m_direction == ldTextDirection::Down ? getUpBorderPos() : getDownBorderPos();

    float y = 0;
    if(m_direction == ldTextDirection::Down) {
        y = (m_textLabel->getPosition().y < getDownBorderPos())
                ? borderValue
                : m_textLabel->getPosition().y - m_speed;
    } else {
        y = (m_textLabel->getPosition().y > getUpBorderPos() )
                ? borderValue
                : m_textLabel->getPosition().y + m_speed;
    }

//        qDebug() << y << m_textLabel->getHeight();

    // move x
    m_textLabel->setPosition(ldVec2(m_textLabel->getPosition().x, y));

    // if finished put next message
    return (m_textLabel->getPosition().y == borderValue);
}


bool ldTicker::updateCurrentMessage()
{
    bool isFinished = false;

    if(m_currentMessageIndex >= m_messages.size()) {
        m_currentMessageIndex = 0;
        isFinished = true;
    }

    QString text = m_currentMessageIndex >= m_messages.size() ? QString() : m_messages[m_currentMessageIndex];
    text.replace('\n', '\t');
    m_textLabel->setText(text);
    restart();
    return isFinished;
}


float ldTicker::getLeftBorderPos() const
{
    return -m_textLabel->getWidth();
}

float ldTicker::getRightBorderPos() const
{
    return 1.f;
}

float ldTicker::getUpBorderPos() const
{
    return 1.f;
}

float ldTicker::getDownBorderPos() const
{
    return -1.f * m_textLabel->getHeight();
}
