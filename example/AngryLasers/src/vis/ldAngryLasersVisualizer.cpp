//
// Laser AngryLasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#include "ldAngryLasersVisualizer.h"

#include <cmath>

#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtCore/QString>

#include "ldCore/ldCore.h"
#include "ldCore/Helpers/Text/ldTextLabel.h"


using namespace std;


// ldAngryLasersVisualizer
ldAngryLasersVisualizer::ldAngryLasersVisualizer() : ldAbstractGameVisualizer()
{
    // Register sounds.
    m_soundEffects.insert(SFX::EXPLOSION, ldCore::instance()->resourceDir() + "/sound/shexplosion.wav");
    m_soundEffects.insert(SFX::POWERUP, ldCore::instance()->resourceDir() + "/sound/powerup.wav");
    m_soundEffects.insert(SFX::NEWLEVEL, ldCore::instance()->resourceDir() + "/sound/shlevelup.wav");
    m_soundEffects.insert(SFX::WOOD_IMPACT, ldCore::instance()->resourceDir() + "/sound/angrylasers/wood.wav");

    // Game score label.
    m_scoreLabel.reset(new ldTextLabel());
    m_scoreLabel->setColor(0xFFFFFF);

    // Current game state label.
    m_stateLabel.reset(new ldTextLabel());
    m_stateLabel->setColor(0xFFFFFF);

    // create box2d world
    b2_world = shared_ptr<b2World>(new b2World(b2Vec2(0, -10)));

    // create box2d ground box
    b2_ground_def.position.Set(0,-World_Size-World_Size/20+0.1f);
    b2_ground.SetAsBox(World_Size, World_Size/10.0f);
    b2_ground_body = b2_world->CreateBody(&b2_ground_def);
    b2_ground_body->CreateFixture(&b2_ground, 0.0f);
    ground_line_y = -0.95f; //(b2_ground_body->GetPosition().y + World_Size/20.0f) * World_Scale;

    m_ground = unique_ptr<ldGradientObject>(new ldGradientObject(0.33f));
    m_ground->addMesh();
    for (float gx=-1; gx<=1; gx+=0.03f)
    {
        int r = (rand() % 128);
        int g = (rand() % 128) + 100;
        int b = (rand() % 128);
        m_ground->addVertex(ldVec2(gx, ground_line_y), (r<<16)|(g<<8)|b);
    }

    m_player.initWorld(b2_world);
    setPosition(ccp(1, 1));

    connect(this, &ldAngryLasersVisualizer::scoreChanged, this, &ldAngryLasersVisualizer::updateScoreLabel);

    // add this as collisions listener
    b2_world->SetContactListener(this);
}


ldAngryLasersVisualizer::~ldAngryLasersVisualizer()
{
    b2_world->DestroyBody(b2_ground_body);
}


// Start a new game.
void ldAngryLasersVisualizer::resetMatch()
{
    emit scoreChanged(0);
    m_score = 0;
    m_level = 1;

    m_player.releaseKeys();
    m_player.onStopped = [this](ldAngryLasersPlayer *player) { return onPlayerStopped(player); };
    m_player.visible = true;
    boxes.clear();
    m_player.resetPosition();

    setStateText("Angry Lasers");

    loadNewLevel();
}


// quickly add a new box to the level
// this has origin at the bottom of the box
void ldAngryLasersVisualizer::addBox(float px, float py, float sx, float sy, ldAngryLasersBox::eAB_BoxType type)
{
    boxes.push_back(unique_ptr<ldAngryLasersBox>(new ldAngryLasersBox(b2_world, ldVec2(px, py+2.0f), ldVec2(sx, sy), type)));
}


// quickly add a new enemy to the level
// this has origin at the bottom of the enemy
void ldAngryLasersVisualizer::addEnemy(float px, float py, ldAngryLasersEnemy::eAB_EnemyType type)
{
    enemies.push_back(unique_ptr<ldAngryLasersEnemy>(new ldAngryLasersEnemy(b2_world, ldVec2(px, py+2.0f), type)));
}


void ldAngryLasersVisualizer::parseLevel(std::vector<std::string> level)
{
    map<char, ldRect> rects;
    float line_offset = World_Size * 2 - level.size();

    for (unsigned int r=0; r<level.size(); r++)
    {
        string row = level[r];
        if (row == "") continue;

        float px = -World_Size + 0.25f;
        float py = World_Size - r - line_offset - 0.5f;

        for (unsigned int c=0; c<row.length(); c++)
        {
            char ch = row[c];
            if (ch != ' ')
            {
                if (ch >= '0' && ch <= '9')
                {
                    // enemy
                    addEnemy(px, py-0.49f, static_cast<ldAngryLasersEnemy::eAB_EnemyType>(ch-48));
                }
                else
                {
                    // box
                    if (rects.count(ch) > 0)
                    {
                        // box is found
                        // update rectangle
                        ldRect rect = rects[ch];

                        if (px <= rect.left())
                        {
                            rect.bottom_left.x = px = -0.25f;
                        }

                        if (px >= rect.right())
                        {
                            rect.top_right.x = px + 0.25f;
                        }

                        if (py <= rect.bottom())
                        {
                            rect.bottom_left.y = py - 0.5f;
                        }

                        if (py >= rect.top())
                        {
                            rect.top_right.y = py + 0.5f;
                        }

                        rects[ch] = rect;
                    }
                    else
                    {
                        // new box
                        ldRect rect;
                        rect.bottom_left = ldVec2(px-0.25f, py-0.5f);
                        rect.top_right = ldVec2(px+0.25f, py+0.5f);
                        rects[ch] = rect;
                    }
                }
            }

            px += 0.5f;
        }
    }

    // loop through stored rects,
    // and generate boxes
    map<char, ldRect>::iterator it_rects = rects.begin();
    while (it_rects != rects.end())
    {
        ldAngryLasersBox::eAB_BoxType type;

        char ch = (*it_rects).first;
        if (ch >= 'a' && ch <= 'z') type = ldAngryLasersBox::eAB_BoxType::Wood;
        else if (ch >= 'A' && ch <= 'Z') type = ldAngryLasersBox::eAB_BoxType::Metal;
        else type = ldAngryLasersBox::eAB_BoxType::Fixed;

        ldRect rect = (*it_rects).second;
        addBox(rect.center().x, rect.center().y, rect.width()/2, rect.height()/2, type);
        ++it_rects;
    }
}


void ldAngryLasersVisualizer::loadNewLevel()
{
    enemies.clear();
    boxes.clear();
    m_player.resetPosition();
    m_levelWinTimer = 2;
    m_balls = max_balls;
    m_player.visible = false;
    setStateText("LEVEL " + QString::number(m_level));

    switch (m_level)
    {
        case 1:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                                                        aaaaaaaaaaaaa                   ",
                "                                                                                         cc       dd                    ",
                "                                                                                         cc       dd                    ",
                "                                                                                         cc   0   dd                    ",
                "                                                                                         cc       dd                    ",
                "                                                                                        bbbbbbbbbbbbb                   ",
                "                                                                                         ee       ff                    ",
                "                                                                                         ee       ff                    ",
                "                                                                                         ee       ff                    ",
                "                                                                                         ee       ff                    ",
            };

            parseLevel(level);

            break;
        }

        case 2:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                                                        aaaaaaaaaaaaa                   ",
                "                                                                                         cc       dd                    ",
                "                                                                                         cc       dd                    ",
                "                                                                                         cc       dd    0               ",
                "                                                                                         cc       dd                    ",
                "                                                                                        bbbbbbbbbbbhhhhhhhhhhhh         ",
                "                                                                                         ee       ff         gg         ",
                "                                                                                         ee       ff         gg         ",
                "                                                                                         ee   1   ff         gg         ",
                "                                                                                         ee       ff         gg         ",
            };

            parseLevel(level);

            break;
        }

        case 3:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                                                        aaaaaaaaaaaaa                   ",
                "                                                                                         cc       dd                    ",
                "                                                                                         cc       dd                    ",
                "                                                                                         cc   0   dd                    ",
                "                                                                                         cc       dd                    ",
                "                                                                                        BBBBBBBBBBBBB                   ",
                "                                                                                         EE       FF                    ",
                "                                                                        gggggggggggg     EE       FF                    ",
                "                                                                       hhh        ii     EE       FF                    ",
                "                                                                       hhh        ii     EE       FF                    ",
                "                                                                       hhh  0     ii     EE    0  FF                    ",
                "                                                                       hhh        ii     EE       FF                    ",
            };

            parseLevel(level);

            break;
        }

        case 4:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                                                         AAAAAAAAAAA                    ",
                "                                                                                         BB       CC                    ",
                "                                                                       JJJJJJJJJJJJ      BB       CC                    ",
                "                                                                         MM       LL     BB    1  CC                    ",
                "                                                                         MM       LL     DDDDDDDDDDD                    ",
                "                                                                        KKK   1   LL     EEE     FFF                    ",
                "                                                                        KKK       LL     EEE     FFF                    ",
                "                                                                        GGGGGGGGGGGG     EEE     FFF                    ",
                "                                                                        HHH     IIII     EEE     FFF                    ",
                "                                                                        HHH     IIII     EEE     FFF                    ",
                "                                                                        HHH     IIII     EEE     FFF                    ",
                "                                                                        HHH     IIII     EEE     FFF                    ",
            };

            parseLevel(level);

            break;
        }

        case 5:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                                                        AAAAAAAAAAAAA                   ",
                "                                                                                         BB       dd                    ",
                "                                                                                         BB       dd                    ",
                "                                                                                         BB    1  dd                    ",
                "                                                                               1         BB       dd                    ",
                "                                                                                        DDDDDDDDDDDDD                   ",
                "                                                                            ggggggggggg  ee       ff                    ",
                "                                                                            CCC      ii  ee       ff                    ",
                "                                                                            CCC      ii  ee   0   ff                    ",
                "                                                                            CCC      ii  ee       ff                    ",
            };

            parseLevel(level);

            break;
        }

        case 6:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                                                             BBB                        ",
                "                                                                                             CCC                        ",
                "                                                                                              2                         ",
                "                                                                                             DDD                        ",
                "                                                                                             EEE                        ",
                "                                                                                             FFF                        ",
                "                                                                                              2                         ",
                "                                                                                             GGG                        ",
                "                                                                                         1   GGG                        ",
                "                                                                                             HHH                        ",
            };

            parseLevel(level);

            break;
        }

        case 7:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                                                        AA                              ",
                "                                                                                        AA                              ",
                "                                                                                        AA    1                         ",
                "                                                                                        AA                              ",
                "                                                                                       BBBBBBBBBBBBB                    ",
                "                                                          GGGGGGGGGGGGGGGG             HHH        bb                    ",
                "                                                                FFFF                   HHH        bb                    ",
                "                                                                FFFF 0                 HHH        bb                    ",
                "                                                               EEEEEEEEEEEEE           HHH        bb                    ",
                "                                                               CCCC     DDDD           HHH        bb                    ",
                "                                                               CCCC     DDDD           HHH        bb                    ",
                "                                                               CCCC     DDDD           HHH        bb  0                 ",
            };

            parseLevel(level);

            break;
        }

        case 8:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                              BB                                                        ",
                "                                                              BB                                       dddddddddddd     ",
                "                                                              BB                                       eee       ff     ",
                "                                                              BB                                       eee       ff     ",
                "                                                              BB                                       eee   1   ff     ",
                "                                                              BB                                     CCCCCCCCCCCCCC     ",
                "                                         AAA                  BB                                     DDDD     EEEEE     ",
                "                                         AAA                  BB                                     DDDD     EEEEE     ",
                "                                         AAA                  BB                                     DDDD     EEEEE     ",
                "                                         AAA                  cc                                     DDDD     EEEEE     ",
                "                                         AAA                  cc                                     DDDD     EEEEE     ",
                "                                         AAA                  cc                                     DDDD     EEEEE     ",
                "                                         AAA                  cc                                     DDDD     EEEEE     ",
                "                                         AAA                  cc                                     DDDD     EEEEE     ",
                "                                         AAA                  cc                                     DDDD     EEEEE     ",
                "                                         AAA    0             cc                                     DDDD     EEEEE     ",
            };

            parseLevel(level);

            break;
        }

        case 9:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                                                                                        ",
                "                                                                                                            0           ",
                "                                                                                                                        ",
                "                                                                                                          JJJJJJ        ",
                "                                                                                                         HHH  II        ",
                "                                                                                                         HHH  II        ",
                "                                                                                          1              HHH  II        ",
                "                                                                                                         HHH  II        ",
                "                                                                                        GGGGGG           HHH  II        ",
                "                                                                                        EE  FF           HHH  II        ",
                "                                                                                        EE  FF           HHH  II        ",
                "                                                                     0                  EE  FF           HHH  II        ",
                "                                                                 AAAAAAAAA              EE  FF           HHH  II        ",
                "                                                                 CC     DD              EE  FF           HHH  II        ",
                "                                                                 CC     DD              EE  FF           HHH  II        ",
                "                                                                 CC     DD              EE  FF           HHH  II        ",
            };

            parseLevel(level);

            break;
        }

        case 10:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                                                                                        ",
                "                                                                                                                        ",
                "                                                                                                           1            ",
                "                                                                                                                        ",
                "                                                                                                  cc      HHHH          ",
                "                                                                                                  cc      HHHH          ",
                "                                                                                                  cc      HHHH          ",
                "                                                                                                  cc      HHHH          ",
                "                                                                                              BBBBBB      HHHH          ",
                "                                                                                              BBBBBB      HHHH          ",
                "                                                                                              BBBBBB      HHHH          ",
                "                                                                                 AAAAA        BBBBBB      HHHH          ",
                "                                                                                 AAAAA        BBBBBB      HHHH          ",
                "                                                                                 AAAAA        BBBBBB      HHHH          ",
                "                                                                                 AAAAA        BBBBBB      HHHH          ",
                "                                                                                 AAAAA  0     BBBBBB   0  HHHH          ",
            };

            parseLevel(level);

            break;
        }

        case 11:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                                                                                        ",
                "                                                                                                                        ",
                "                                                   bbbbbbb                                        1                     ",
                "                                                  aa     cc                                                             ",
                "                                                  aa  1  cc                                     ++++++                  ",
                "                                                  aa     cc                                     ++++++                  ",
                "                                                ============                                    ++++++                  ",
                "                                                ============                                                            ",
                "                                                                                                                        ",
                "                                                       0                                                                ",
                "                                                AAAAAAAAAA                                                              ",
                "                                                AAAAAAAAA                                                               ",
                "                                                  BBBBBB                                                                ",
                "                                                  BBBBBB                                                                ",
                "                                                  BBBBBB                                                                ",
                "                                                  BBBBBB                                                                ",
                "                                                  BBBBBB                                                                ",
                "                                                  BBBBBB                                                                ",
                "                                                  BBBBBB                                                                ",
                "                                                  BBBBBB                                                                ",
                "                                                  BBBBBB                                                                ",
            };

            m_balls++;
            parseLevel(level);

            break;
        }

        case 12:
        {
            std::vector<std::string> level =
            {
//              "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
                "                                                                                                                        ",
                "                                                                     ff                                                 ",
                "                                                                     ff                                                 ",
                "                                                                     ff                                                 ",
                "                                                                     ff                               hhhhhhhhhh        ",
                "                                                                     ff                               gg      ii        ",
                "                                                                     ff 0                             gg      ii        ",
                "                                                                     ff                               gg      ii        ",
                "                                                                  +++++++                             gg    1 ii        ",
                "                                                                  +++++++                             gg      ii        ",
                "                                                                  +++++++                            ****************   ",
                "                                                                                                                        ",
                "                                                    ---                                                                 ",
                "                                                    ---                         dddddddddddd                            ",
                "                                                    ---                              ee                                 ",
                "                                                                                     ee                                 ",
                "                                                 aaaaaaaaa  bb                       ee                                 ",
                "                                                 aaaaaaaaa  bb                       ee                                 ",
                "                                                    jjj     bb  1                    ee                                 ",
                "                                                    jjj     bb                       ee                                 ",
                "                                                    jjj     bb                       ee    010                          ",
            };

            m_balls++;
            parseLevel(level);

            break;
        }

    }

#if false
    // create a pyramid
    // (physics world is assumed to be 100x50 meters)
    for (int r=0; r<5; r++)
    {
        for (int c=0; c<r+1; c++)
        {
            float px = - r / 2 - r * 0.2f + c + 20;
            float py = -(World_Size/2) + r * 3;

            boxes.push_back(unique_ptr<ldAngryLasersBox>(new ldAngryLasersBox(b2_world, ldVec2(px, py), ldVec2(1,1.5f), ldAngryLasersBox::eAB_BoxType::wood)));
        }
    }
#endif
}


void ldAngryLasersVisualizer::onPlayerStopped(ldAngryLasersPlayer * /*player*/)
{
    m_balls--;
    qDebug() << m_balls << " remaining";
    if (m_balls <= 0 && enemies.size() > 0)
    {
        setStateText("FAILED!");
        m_player.visible = false;
        m_levelLoseTimer = 4;
    }
}


void ldAngryLasersVisualizer::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    if (!m_player.launched) return;
    ldAngryLasersEnemy* enemy1 = static_cast<ldAngryLasersEnemy*>(contact->GetFixtureA()->GetUserData());
    ldAngryLasersEnemy* enemy2 = static_cast<ldAngryLasersEnemy*>(contact->GetFixtureB()->GetUserData());

    if (enemy1 || enemy2)
    {
        ldAngryLasersEnemy* enemy = (enemy1 ? enemy1 : enemy2);
        float total_impulse = abs(impulse->normalImpulses[0]); // + abs(impulse->normalImpulses[1]);
        //qDebug() << "COLLISION: " << impulse->normalImpulses[0] << ", " << impulse->normalImpulses[1] << " ==> " << total_impulse;

        if (total_impulse > enemy->resistence)
        {
            // if the impact is reasonable, destroy the enemy
            enemy->dead = true;
        }
    }
}

void ldAngryLasersVisualizer::onGameReset()
{
    resetMatch();
}

void ldAngryLasersVisualizer::onGamePause()
{
    setStateText("PAUSE");
}

void ldAngryLasersVisualizer::onGamePlay()
{
    if(playingState() == ldPlayingState::InGame) {
        setStateText("");
    } else {
        onGameReset();
    }
}

// Update game elements.
void ldAngryLasersVisualizer::updateGame(float deltaTime)
{
    if (m_levelWinTimer > 0)
    {
        m_levelWinTimer -= deltaTime;

        if (m_levelWinTimer <= 0)
        {
            m_levelWinTimer = 0;
            setStateText("");
            m_player.visible = true;
            m_soundEffects.play(SFX::NEWLEVEL);
        }

        return;
    }
    else if (m_levelLoseTimer > 0)
    {
        m_levelLoseTimer -= deltaTime;
        if (m_levelLoseTimer <= 0)
        {
            m_levelLoseTimer = 0;
            loadNewLevel();
        }
    }

    // Update timers.
    m_gameTimer += deltaTime;

    m_player.update(deltaTime);
    b2_world->Step(TimeStep, VelocityIterations, PositionIterations);
}


// Draw all game graphics.
void ldAngryLasersVisualizer::draw()
{
    QMutexLocker lock(&m_mutex);
    ldAbstractGameVisualizer::draw();

    // Draw score label
    if (state() == ldGameState::Reset || m_levelWinTimer > 0 || m_levelLoseTimer > 0)
    {
        updateScoreLabel();
        m_scoreLabel->setColor(rand()%0xFFFFFF);
        m_scoreLabel->innerDraw(m_renderer);
    }

    // Execute game functions.
    if (state() == ldGameState::Playing)
    {
        updateGame(m_renderer->getLastFrameDeltaSeconds());
    }
    else
    {
    }

    //
    // actual drawings
    //

    m_stateLabel->innerDraw(m_renderer);

    // draw all enemies
    list<std::unique_ptr<ldAngryLasersEnemy>>::iterator it_enemy = enemies.begin();
    while (it_enemy != enemies.end())
    {
        if (!(*it_enemy)->render(m_renderer))
        {
            if (m_levelLoseTimer == 0)
            {
                // enemy is inaccessible, out of screen
                // mission failed!
                setStateText("FAILED!");
                m_player.visible = false;
                m_levelLoseTimer = 4;
            }
        }

        if ((*it_enemy)->dead)
        {
            m_soundEffects.play(SFX::EXPLOSION);
            addFireworks((*it_enemy)->get_position());
            m_score += 10 - (max_balls - m_balls);
            it_enemy = enemies.erase(it_enemy);

            if (enemies.size() == 0)
            {
                // level up!
                m_level++;
                m_player.resetPosition();
                loadNewLevel();
                return;
            }
        }
        else ++it_enemy;
    }

    // draw all boxes
    list<std::unique_ptr<ldAngryLasersBox>>::iterator it_box = boxes.begin();
    while (it_box != boxes.end())
    {
        (*it_box)->render(m_renderer);
        ++it_box;
    }

    m_player.draw(m_renderer);
    m_ground->renderGradient(m_renderer, 1);
}


/*
 * Label functions.
 */

// Set the state label's text and horizontally align it.
void ldAngryLasersVisualizer::setStateText(const QString &text)
{
    m_stateLabel->setText(text);

    float labelWidth = m_stateLabel->getWidth();
    m_stateLabel->setPosition(ldVec2((1.0f - labelWidth)/2.0f, 0.5f));
}


// Updates the score label.
void ldAngryLasersVisualizer::updateScoreLabel()
{
    QString scoreString = QString("%1").arg(QString::number(m_score));
    m_scoreLabel->setText(scoreString);

    float labelWidth = m_scoreLabel->getWidth();
    float labelHeight = m_scoreLabel->getHeight();
    m_scoreLabel->setPosition(ldVec2(0.5f - (labelWidth/2.0f), 0.25f + (labelHeight / 2.0f)));
}


void ldAngryLasersVisualizer::moveX(double x)
{
    QMutexLocker lock(&m_mutex);
    if(x < -0.2) {
        m_player.onPressedLeft(true);
        m_player.onPressedRight(false);
    } else if(x > 0.2){
        m_player.onPressedLeft(false);
        m_player.onPressedRight(true);
    } else {
        m_player.onPressedLeft(false);
        m_player.onPressedRight(false);
    }
}

void ldAngryLasersVisualizer::moveY(double y)
{
    QMutexLocker lock(&m_mutex);
    if(y < -0.2) {
        m_player.onPressedUp(false);
        m_player.onPressedDown(true);
    } else if(y > 0.2){
        m_player.onPressedUp(true);
        m_player.onPressedDown(false);
    } else {
        m_player.onPressedUp(false);
        m_player.onPressedDown(false);
    }
}

void ldAngryLasersVisualizer::onPressedLeft(bool pressed)
{
    QMutexLocker lock(&m_mutex);
    m_player.onPressedLeft(pressed);
}

void ldAngryLasersVisualizer::onPressedRight(bool pressed)
{
    QMutexLocker lock(&m_mutex);
    m_player.onPressedRight(pressed);
}

void ldAngryLasersVisualizer::onPressedUp(bool pressed)
{
    QMutexLocker lock(&m_mutex);
    m_player.onPressedUp(pressed);
}

void ldAngryLasersVisualizer::onPressedDown(bool pressed)
{
    QMutexLocker lock(&m_mutex);
    m_player.onPressedDown(pressed);
}

void ldAngryLasersVisualizer::onPressedShoot(bool pressed)
{
    QMutexLocker lock(&m_mutex);
    m_player.onPressedShoot(pressed);
}
