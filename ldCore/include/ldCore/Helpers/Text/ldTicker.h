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

#ifndef LDTICKER_H
#define LDTICKER_H

#include <memory>

#include <ldCore/ldCore.h>
#include <ldCore/Helpers/Text/ldTextDirection.h>

class ldOLDrawer;
class ldRendererOpenlase;
class ldTextLabel;

class LDCORESHARED_EXPORT ldTicker
{
public:
    ldTicker();
    ~ldTicker();

    // text
    void setMessage(const QString &message);
    void setMessages(const QStringList &messages);

    // seepd
    void setSpeed(float speed);
    float speed() const;

    void setDirection(ldTextDirection::Direction direction);
    ldTextDirection::Direction direction() const;

    // text style
    void setFontSize(float fontSize);
    float getFontSize() const;

    void setFont(const QString &fontFamily);
    QString font() const;

    float letterSpace() const;
    void setLetterSpace(float letterSpace);

    float getHeight() const;

    void setCenterY(float y); // 0..2

    ldOLDrawer *drawer() const;

    // ticker
    bool draw(ldRendererOpenlase* p_renderer);
    void restart();

    ldTicker* clone() const;

private:
    float calcTextX();
    float calcTextY();
    bool updateCurrentMessage();
    void updateXPos();

    bool drawHorizontal();
    bool drawVertical();

    float getLeftBorderPos() const;
    float getRightBorderPos() const;
    float getUpBorderPos() const;
    float getDownBorderPos() const;

    std::unique_ptr<ldTextLabel> m_textLabel;

    float m_speed = 0.025f;

    ldTextDirection::Direction m_direction = ldTextDirection::Direction::First;

    QStringList m_messages;
    int m_currentMessageIndex = 0;

    float m_centerX = 1.;
    float m_centerY = 1.;
};

#endif // LDTICKER_H


