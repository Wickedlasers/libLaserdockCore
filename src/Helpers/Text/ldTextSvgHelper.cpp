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

// ldTextSvgHelper.cpp
// Created by Eric Brugère on 8/dec/16.
// Copyright (c) 2016 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Text/ldTextSvgHelper.h"

#include <string>
#include <vector>
#include <algorithm>

#include <QtCore/QDebug>
#include <QRegularExpression>

#include <ldCore/ldCore.h>
#include <ldCore/Helpers/ldEnumHelper.h>

#include "ldCore/Helpers/SVG/ldSvgReader.h"

ldTextSvgHelper *ldTextSvgHelper::instance()
{
    static ldTextSvgHelper instance;
    return &instance;
}

// indexForSvgValidChars
int ldTextSvgHelper::indexForSvgValidChars(const QChar& p_str)
{
    const QChar& strCleaned = ldTextSvgHelper::replaceAccentAndSome(p_str);
    return m_svgValidChars.indexOf(strCleaned);
}

QChar ldTextSvgHelper::replaceAccentAndSome(const QChar& p_str)
{
    //
    if (p_str == QChar(0x02C3) || p_str == QChar(0x203A)) return '>';
    else if (p_str == QChar(0x00AF) || p_str == QChar(0x2010) || p_str == QChar(0x2013) || p_str == QChar(0x2014)) return '-';
    else if (p_str == QChar(0x02C2) || p_str == QChar(0x2039)) return '<';
    else if (p_str == "‘" || p_str == "’" || p_str == QChar(0x2027)) return '\'';
    else if (p_str == "「" || p_str == "」" || p_str == "«" || p_str == "»" || p_str == QChar(0x00AB) || p_str == QChar(0x00BB) || p_str == QChar(0x201C) || p_str == QChar(0x201D) ) return '"';
    else if (p_str == QChar(0x25E6) || p_str == QChar(0x25CF) || p_str == QChar(0x25CB) || p_str == QChar(0x2B24) || p_str == QChar(0x2B58)) return QChar(0x2022);

    else if (p_str == "\n" || p_str == "\t" || p_str == "\r") return ' ';

    return p_str;
}

// svgForChar
QString ldTextSvgHelper::svgPathForChar(const QChar& p_str, const ldFont::Family &font)
{

    QString res(ldCore::instance()->resourceDir() + "/svg/fonts/" + ldFont(font).prefix() + "/") ;

    res += "/";


    // replace accents
    const QChar& strCleaned = ldTextSvgHelper::replaceAccentAndSome(p_str);

    // not in svg files
    if (!isSvgChar(strCleaned)) {
        res.append("_char-undefined.svg");
        // return
        return res;
    }

    if(m_charMap.contains(strCleaned)) {
        res.append(m_charMap[strCleaned]);
    } else if (strCleaned.isUpper()) {
        res.append(strCleaned.toLower());
        res.append("-upper");
    } else res.append(strCleaned);
    // return path
    res.append(".svg");
    //
    return res;
}

// resizeSvg
std::vector<ldSvgLetter> ldTextSvgHelper::resizedSvgLetters(float p_scale, const ldFont::Family &font)
{
    // make a copy for res
    std::vector<ldSvgLetter> res = m_laserdockSvgLetters[font];

    //qDebug() << "(int)p_letters.size():" <<(int)p_letters.size();
    //qDebug() << "(int)res.size():" <<(int)res.size();

    // scale = font size !
    // work on A to have the standard decay for this font.
    // SVG have been all done at same scale, same y position, and all centered in x.
    // we resize the letters so A fit screen at scale 1, and start at the very left.
    // so lower case g or p will go below 0 in y.
    // so don't work at scale 1.0 if you are ot sure of what you are doing
    //
    // will return [0,1]x[0,1] coordinates, with the letter starting at 0,0 at any scale.
    //
    int a_upper_index = indexForSvgValidChars('A');

    if (a_upper_index >= (int)res.size()) return res;

    ldSvgLetter a_upper_svg = res[a_upper_index];

    //
    float hA = a_upper_svg.data().dim().height();
    // Vec2 trA = Vec2(-a_upper_dim.bottom_left.x, -a_upper_dim.bottom_left.y);
    // we gonna divide by hA
    // qDebug() << "hA:" <<hA;

    if (hA < 0.0000001) return res;

    for (ldSvgLetter &bezierCurves : res) {
        Vec2 tr1 = Vec2(0, -a_upper_svg.data().dim().bottom_left.y);
        bezierCurves.translate(tr1);
        bezierCurves.scale(p_scale/hA);
    }
    return res;
}

ldTextSvgHelper::ldTextSvgHelper()
{
    // init special char map
    initCharMap();

    // svg list of valid chars
    initSvgValidChars();

    // !!!should be inited after m_svgValidChars!!!
    initSvgLetters();
}

void ldTextSvgHelper::initCharMap()
{
    auto toChar = [](const QString &unicodeChar) -> QChar {
        return unicodeChar[0];
    };
    m_charMap.insert('\'', "_apostrophe");
    m_charMap.insert(':', "_colon");
    m_charMap.insert(',', "_comma");
    m_charMap.insert('!', "_exclamation");
    m_charMap.insert('>', "_greater");
    m_charMap.insert('-', "_hyphen");
    m_charMap.insert('<', "_lower");
    m_charMap.insert('(', "_parentheses_left");
    m_charMap.insert(')', "_parentheses_right");
    m_charMap.insert('.', "_period");
    m_charMap.insert('?', "_question");
    m_charMap.insert('"', "_quote"); //  || p_str == '“' || p_str == '”'
    m_charMap.insert('%', "_pourcent");
    m_charMap.insert(';', "_semicolon");
    m_charMap.insert('@', "_arobase");
    m_charMap.insert(QChar(0x00B0), "_degree"); // °
    m_charMap.insert('/', "_slash");
    m_charMap.insert('#', "_sharp");
    m_charMap.insert('_', "_underscore");
    m_charMap.insert('$', "_dollar");
    m_charMap.insert(QChar(0x00A3), "_pound"); // 00A3 £
    m_charMap.insert(QChar(0x00A5), "_yen"); // ¥
    m_charMap.insert('&', "_ampersand");
    m_charMap.insert(QChar(0x20AC), "_euro"); // €
    m_charMap.insert(QChar(0x00A9), "_copyright"); // ©
    m_charMap.insert(QChar(0x00AE), "_registered"); // ®
    m_charMap.insert(QChar(0x2122), "_trademark"); // ™
    m_charMap.insert('*', "_star");
    m_charMap.insert('+', "_plus");
    m_charMap.insert('=', "_equals");
    m_charMap.insert('[', "_bracket_left");
    m_charMap.insert(']', "_bracket_right");
    m_charMap.insert('\\', "_backslash");
    m_charMap.insert('^', "_caret");
    m_charMap.insert('{', "_brace_left");
    m_charMap.insert('}', "_brace_right");
    m_charMap.insert('|', "_pipe");
    m_charMap.insert('~', "_tilde");
    m_charMap.insert(QChar(0x201E), "_quote_lower"); // „
    m_charMap.insert(QChar(0x00B4), "_quote_back"); // ´
    m_charMap.insert(QChar(0x00B2), "_power2"); // ²
    m_charMap.insert(QChar(0x2026), "_period_triple"); // …
    m_charMap.insert(QChar(0x00B7), "_period_centered"); // ·
    m_charMap.insert(QChar(0x2022), "_circle"); // •
    m_charMap.insert(QChar(0x00F7), "_divide"); // ÷
    m_charMap.insert(QChar(0x00D7), "_cross"); // ×
    m_charMap.insert(QChar(0x2030), "_permille"); // ‰
    // ¤ ¶ § ` ¢ ¿ ¬ ½ ¼ ¡ ¦ ± † ‡ ˆ ˜ ¨ ¯ ³ ¹ ¸ ¾ ø Ø Ð ˊ
    // Ÿ œ ß µ š Š Œ þ P ƒ á í ó ú ñ Ñ À Á Â Ã Ä Å È É Ê Ë Ì Í Î Ï Ò Ó Ô Õ Ö Ù Ú Û Ü Ý Þ ã ð õ ü ý Ç ü é â ä à å ç ê ë è ï î ì æ Æ ô ö ò û ù ÿ ČčƠơŨũŽž
    m_charMap.insert(toChar("ˊ"), "_acute"); // ˊ
    m_charMap.insert(toChar("¦"), "_broken-bar"); // ¦
    m_charMap.insert(toChar("¢"), "_cent"); // ¢
    m_charMap.insert(toChar("ˆ"), "_circumflex"); // ˆ
    m_charMap.insert(toChar("¤"), "_currency"); // ¤
    m_charMap.insert(toChar("¨"), "_diaeresis"); // ¨
    m_charMap.insert(toChar("ß"), "_eszett"); // ß
    m_charMap.insert(toChar("ƒ"), "_florin"); // ƒ
    m_charMap.insert(toChar("¼"), "_fourth"); // ¼
    m_charMap.insert(toChar("`"), "_grave"); // `
    m_charMap.insert(toChar("½"), "_half"); // ½
    m_charMap.insert(toChar("¯"), "_macron"); // ¯
    m_charMap.insert(toChar("¬"), "_negation"); // ¬
    m_charMap.insert(toChar("†"), "_obelus"); // †
    m_charMap.insert(toChar("¶"), "_pilcrow"); // ¶
    m_charMap.insert(toChar("±"), "_plus-minus"); // ±
    m_charMap.insert(toChar("‡"), "_plus-plus"); // ‡
    m_charMap.insert(toChar("¹"), "_pow-one"); // ¹
    m_charMap.insert(toChar("³"), "_pow-three"); // ³
    m_charMap.insert(toChar("˚"), "_ring"); // ˚
    m_charMap.insert(toChar("§"), "_section-sign"); // §
    m_charMap.insert(toChar("Þ"), "_thorn"); // Þ
    m_charMap.insert(toChar("þ"), "_thorn"); // þ
    m_charMap.insert(toChar("¾"), "_three-fourth"); // ¾
    m_charMap.insert(toChar("˜"), "_up-tilde"); // ˜
    m_charMap.insert(toChar("Á"), "a-acute-upper"); // Á
    m_charMap.insert(toChar("á"), "a-acute"); // á
    m_charMap.insert(toChar("Ă"), "a-breve-upper"); // Ă
    m_charMap.insert(toChar("ă"), "a-breve"); // ă
    m_charMap.insert(toChar("Â"), "a-circumflex-upper"); // Â
    m_charMap.insert(toChar("â"), "a-circumflex"); // â
    m_charMap.insert(toChar("Ä"), "a-diaeresis-upper"); // Ä
    m_charMap.insert(toChar("ä"), "a-diaeresis"); // ä
    m_charMap.insert(toChar("À"), "a-grave-upper"); // À
    m_charMap.insert(toChar("à"), "a-grave"); // à
    m_charMap.insert(toChar("Å"), "a-ring-upper"); // Å
    m_charMap.insert(toChar("å"), "a-ring"); // å
    m_charMap.insert(toChar("Ã"), "a-tilde-upper"); // Ã
    m_charMap.insert(toChar("ã"), "a-tilde"); // å
    m_charMap.insert(toChar("Æ"), "ae-upper"); // Æ
    m_charMap.insert(toChar("æ"), "ae"); // æ
    m_charMap.insert(toChar("Č"), "c-caron-upper"); // Č
    m_charMap.insert(toChar("č"), "c-caron"); // č
    m_charMap.insert(toChar("Ç"), "c-cedilla-upper"); // Ç
    m_charMap.insert(toChar("ç"), "c-cedilla"); // ç
    m_charMap.insert(toChar("Ð"), "d-bar-upper"); // Ð
    m_charMap.insert(toChar("ð"), "d-bar"); // ð
    m_charMap.insert(toChar("É"), "e-acute-upper"); // É
    m_charMap.insert(toChar("é"), "e-acute"); // é
    m_charMap.insert(toChar("Ê"), "e-circumflex-upper"); // Ê
    m_charMap.insert(toChar("ê"), "e-circumflex"); // ê
    m_charMap.insert(toChar("Ë"), "e-diaeresis-upper"); // Ë
    m_charMap.insert(toChar("ë"), "e-diaeresis"); // ë
    m_charMap.insert(toChar("È"), "e-grave-upper"); // È
    m_charMap.insert(toChar("è"), "e-grave"); // è
    //m_charMap.insert(toChar(""), "e-ring-upper"); //
    //m_charMap.insert(toChar(""), "e-ring"); //
    //m_charMap.insert(toChar(""), "e-tilde-upper"); //
    //m_charMap.insert(toChar(""), "e-tilde"); //
    m_charMap.insert(toChar("Í"), "i-acute-upper"); // Í
    m_charMap.insert(toChar("í"), "i-acute"); // í
    m_charMap.insert(toChar("Î"), "i-circumflex-upper"); // Î
    m_charMap.insert(toChar("î"), "i-circumflex"); // î
    m_charMap.insert(toChar("Ï"), "i-diaeresis-upper"); // Ï
    m_charMap.insert(toChar("ï"), "i-diaeresis"); // ï
    m_charMap.insert(toChar("Ì"), "i-grave-upper"); // Ì
    m_charMap.insert(toChar("ì"), "i-grave"); // ì
    //m_charMap.insert(toChar(""), "i-ring-upper"); //
    //m_charMap.insert(toChar(""), "i-ring"); //
    //m_charMap.insert(toChar(""), "i-tilde-upper"); //
    //m_charMap.insert(toChar(""), "i-tilde"); //
    m_charMap.insert(toChar("Ñ"), "n-tilde-upper"); // Ñ
    m_charMap.insert(toChar("ñ"), "n-tilde"); // ñ
    m_charMap.insert(toChar("Ó"), "o-acute-upper"); // Ó
    m_charMap.insert(toChar("ó"), "o-acute"); // ó
    m_charMap.insert(toChar("Ø"), "o-bar-upper"); // Ø
    m_charMap.insert(toChar("ø"), "o-bar"); // ø
    m_charMap.insert(toChar("Ô"), "o-circumflex-upper"); // Ô
    m_charMap.insert(toChar("ô"), "o-circumflex"); // ô
    m_charMap.insert(toChar("Ơ"), "o-cornus-upper"); // Ơ
    m_charMap.insert(toChar("ơ"), "o-cornus"); // ơ
    m_charMap.insert(toChar("Ö"), "o-diaeresis-upper"); // Ö
    m_charMap.insert(toChar("ö"), "o-diaeresis"); // ö
    m_charMap.insert(toChar("Ò"), "o-grave-upper"); // Ò
    m_charMap.insert(toChar("ò"), "o-grave"); // ò
    //m_charMap.insert(toChar(""), "o-ring-upper"); //
    //m_charMap.insert(toChar(""), "o-ring"); //
    m_charMap.insert(toChar("Õ"), "o-tilde-upper"); // Õ
    m_charMap.insert(toChar("õ"), "o-tilde"); // õ
    m_charMap.insert(toChar("Œ"), "oe-upper"); // Œ
    m_charMap.insert(toChar("œ"), "oe"); // œ
    m_charMap.insert(toChar("Š"), "s-caron-upper"); // Š
    m_charMap.insert(toChar("š"), "s-caron"); // š
    m_charMap.insert(toChar("Ú"), "u-acute-upper"); // Ú
    m_charMap.insert(toChar("ú"), "u-acute"); // ú
    m_charMap.insert(toChar("Û"), "u-circumflex-upper"); // Û
    m_charMap.insert(toChar("û"), "u-circumflex"); // û
    //m_charMap.insert(toChar(""), "u-cornus-upper"); //
    //m_charMap.insert(toChar(""), "u-cornus"); //
    m_charMap.insert(toChar("Ü"), "u-diaeresis-upper"); // Ü
    m_charMap.insert(toChar("ü"), "u-diaeresis"); // ü
    m_charMap.insert(toChar("Ù"), "u-grave-upper"); // Ù
    m_charMap.insert(toChar("ù"), "u-grave"); // ù
    //m_charMap.insert(toChar(""), "u-ring-upper"); //
    //m_charMap.insert(toChar(""), "u-ring"); //
    m_charMap.insert(toChar("Ũ"), "u-tilde-upper"); // Ũ
    m_charMap.insert(toChar("ũ"), "u-tilde"); // ũ
    m_charMap.insert(toChar("Ý"), "y-acute-upper"); // Ý
    m_charMap.insert(toChar("ý"), "y-acute"); // ý
    m_charMap.insert(toChar("Ÿ"), "y-diaeresis-upper"); // Ÿ
    m_charMap.insert(toChar("ÿ"), "y-diaeresis"); // ÿ
    m_charMap.insert(toChar("Ž"), "z-caron-upper"); // Ž
    m_charMap.insert(toChar("ž"), "z-caron"); // ž
    m_charMap.insert(toChar("¡"), "_exclamation_invert"); // ¡
    m_charMap.insert(toChar("¿"), "_question_invert"); // ¿
    m_charMap.insert(toChar("µ"), "_mu"); // µ
}

void ldTextSvgHelper::initSvgValidChars()
{
    QString abc("abcdefghijklmnopqrstuvwxyz");
    abc.append("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    abc.append("0123456789");
    abc.append("':,!-().?\";_#/@°<>%");
    abc.append("$£¥&€©®™*+=[\\]^{}|~«»‹›„“”–—²…·•´÷×‰");
    abc.append("¡¿µ¤¶§`ˊ¢¿¬½¼¡¦±†‡ˆ˜¨¯³¹¾øØÐŸœßšŠŒþPƒáíóúñÑÀÁÂÃÄÅÈÉÊËÌÍÎÏÒÓÔÕÖÙÚÛÜÝÞãðõüýÇüéâäàåçêëèïîìæÆôöòûùÿČčƠơŨũŽž");
    for (const auto &it : abc ) m_svgValidChars << it;
}

void ldTextSvgHelper::initSvgLetters()
{
    // important: here same order than allValidChars, we will rely on that later to find the letter vector
    // add undefined char svg at the end
    // !!!should be inited after m_svgValidChars!!!
    for(const auto &font : ldEnumHelper::Enum<ldFont::Family>()) {
        QList<QChar> allValidChars = m_svgValidChars;
        for (const QChar &validChar : allValidChars) {
            // load svg file
            m_laserdockSvgLetters[font].push_back(ldSvgLetter(svgPathForChar(validChar, font)));
        }
        // add undefined char svg at the end
        m_laserdockSvgLetters[font].push_back(ldSvgLetter(svgPathForChar(' ', font)));
    }
}


// isSvgChar
bool ldTextSvgHelper::isSvgChar(const QChar& p_str)
{
    const QChar& strCleaned = ldTextSvgHelper::replaceAccentAndSome(p_str);
    return m_svgValidChars.contains(strCleaned);
}
