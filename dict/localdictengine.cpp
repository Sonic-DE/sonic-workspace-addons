/*
    SPDX-FileCopyrightText: 2025 Anirudh Vempati <rudy.vempati@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "localdictengine.h"

#include <KLocalizedString>

#include <QFile>
#include <QDir>

using namespace Qt::StringLiterals;

LocalDictEngine::LocalDictEngine(QObject* parent)
    : QObject(parent)
    , m_dictPath(QStringLiteral("/usr/share/wordnet-3.0/dict/"))
    , m_defTypes{
        QStringLiteral("adj"),
        QStringLiteral("adv"),
        QStringLiteral("noun"),
        QStringLiteral("verb"),
    }
{
}

LocalDictEngine::~LocalDictEngine()
{
}

static bool seekBackwardToLineStart(QFile &file, qsizetype leftBound = 0) {
    char garbageChar;
    qsizetype position = file.pos();

    file.peek(&garbageChar, 1);
    while (garbageChar != '\n' && position >= leftBound) {
        position -= 1;
        file.seek(position);
        file.peek(&garbageChar, 1);
    }
    file.peek(&garbageChar, 1);
    if (garbageChar != '\n') {
        return false;
    }

    file.getChar(&garbageChar);
    return true;
}

void LocalDictEngine::requestDefinition(const QString &query)
{
    QDir wnDictDir(m_dictPath);

    QString queryWord {query.toLower()};

    QStringList definitions;

    for (auto defType : m_defTypes) {
        QFile wnIndexFile(wnDictDir.filePath(QStringLiteral("index.")+defType));
        QFile wnDataFile(wnDictDir.filePath(QStringLiteral("data.")+defType));

        if (!wnIndexFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }

        if (!wnDataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }

        char garbageChar;
        
        int bufferLength = 1024;
        QByteArray garbageBuffer(bufferLength, '\0');
        
        // Read past the license at the start of each file
        // Each line of the license starts with a space
        wnIndexFile.getChar(&garbageChar);
        while (garbageChar == ' ') {
            wnIndexFile.readLine(garbageBuffer.data(), bufferLength);
            wnIndexFile.getChar(&garbageChar);
        }
        wnIndexFile.ungetChar(garbageChar);

        // Perform binary search on the file to find query word
        qsizetype liter = wnIndexFile.pos();
        qsizetype riter = wnIndexFile.size() - 1;

        QTextStream wnIndexStream {&wnIndexFile};

        bool foundQuery = false;
        while (liter < riter) {
            qsizetype mid = (liter+riter)/2;
            wnIndexFile.seek(mid);
            // Seek forward to reach start of a new line
            wnIndexFile.readLine(garbageBuffer.data(), bufferLength);

            // If we overshoot riter, we instead seek 
            // backward to reach the start of a new line
            qsizetype lineStartPosn = wnIndexFile.pos();
            if (lineStartPosn >= riter) {
                wnIndexFile.seek(mid);
                if (!seekBackwardToLineStart(wnIndexFile, liter)) {
                    break;
                }
                lineStartPosn = wnIndexFile.pos();
            }
            
            wnIndexStream.seek(wnIndexFile.pos());
            
            QString foundWord;
            wnIndexStream >> foundWord;

            // If we overshoot riter, we instead seek 
            // backward to reach the start of a new line
            qsizetype wordEndPosn = wnIndexStream.pos();
            if (wordEndPosn > riter)
            {
                wnIndexFile.seek(mid);
                if (!seekBackwardToLineStart(wnIndexFile, liter)) {
                    break;
                }
                lineStartPosn = wnIndexFile.pos();
                wnIndexStream.seek(wnIndexFile.pos());
                wnIndexStream >> foundWord;
                wordEndPosn = wnIndexStream.pos();
                wnIndexFile.seek(wordEndPosn);
            }

            if (foundWord < queryWord) {
                liter = wordEndPosn;
            }
            else if (foundWord > queryWord) {
                riter = lineStartPosn;
            }
            else {
                foundQuery = true;
                break;
            }
        }

        if (!foundQuery) {
            continue;
        }

        QString garbageString;
        // Get the part of speech
        wnIndexStream >> garbageString;
        // Get the synset count
        wnIndexStream >> garbageString;
        qsizetype num_meanings = garbageString.toLongLong();
        // Get the pointer count
        wnIndexStream >> garbageString;
        qsizetype pointerCount = garbageString.toLongLong();
        // Eat the pointers
        while (pointerCount--) {
            wnIndexStream >> garbageString;
        }
        // Get the sense count
        wnIndexStream >> garbageString;
        // Get the tag sense count
        wnIndexStream >> garbageString;
        // Get the offsets
        QList<qsizetype> offsets;
        while (num_meanings--) {
            wnIndexStream >> garbageString;
            offsets.push_back(garbageString.toLongLong());
        }

        QTextStream wnDataStream {&wnDataFile};
        for (size_t offset : offsets) {
            wnDataStream.seek(offset);
            // Get the offset
            wnDataStream >> garbageString;
            // Get the lexographer number
            wnDataStream >> garbageString;
            // Get the part of speech
            QString pos;
            wnDataStream >> pos;
            // Get the word count
            wnDataStream >> garbageString;
            qsizetype num_syn = garbageString.toLongLong();
            // Get the syns
            QString syns;
            while (num_syn--) {
                QString syn;
                wnDataStream >> syn;
                QString synlower {syn.toLower()};
                if (synlower != queryWord) {
                    if (syns.size() != 0) {
                        syns += QStringLiteral(", ");
                    }
                    syns += syn;
                }
                wnDataStream >> garbageString;
            }
            // Get the pointer count
            wnDataStream >> garbageString;
            pointerCount = garbageString.toLongLong();
            while (pointerCount--) {
                wnDataStream >> garbageString;
                wnDataStream >> garbageString;
                wnDataStream >> garbageString;
                wnDataStream >> garbageString;
            }
            // For verbs eat the verb frame block
            if (defType == QStringLiteral("verb")) {
                // Get frame count
                wnDataStream >> garbageString;
                qsizetype frameCount = garbageString.toLongLong();
                while (frameCount--) {
                    wnDataStream >> garbageString;
                    wnDataStream >> garbageString;
                    wnDataStream >> garbageString;
                }
            }
            // Get the separator
            wnDataStream >> garbageString;
            // Get the meaning
            QString meaning;
            /// Eat leading space
            wnDataStream.seek(wnDataStream.pos()+1);
            wnDataStream.readLineInto(&meaning);
            /// Eat trailing spaces
            meaning.removeLast();
            meaning.removeLast();
            definitions.push_back(pos + QStringLiteral(": ") + meaning + QStringLiteral(" [syn: ") + syns + QStringLiteral("]"));
        }
    }

    Q_EMIT definitionRecieved(definitions);
}