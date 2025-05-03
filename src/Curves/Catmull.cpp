#include <iostream>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Catmull.h"
#include "Keyframe.h"

using namespace glm;
using namespace std;

Catmull::Catmull()
{
    keyframes = vector<Keyframe>();
    ALTable = vector<pair<float, float>>();

    B = mat4();
    B[0] = vec4(0, 2, 0, 0);
    B[1] = vec4(-1, 0, 1, 0);
    B[2] = vec4(2, -5, 4, -1);
    B[3] = vec4(-1, 3, -3, 1);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            B[i][j] *= .5;
        }
    }
}

void Catmull::addKeyframe(Keyframe kf)
{
    keyframes.push_back(kf);

    // Check dot product of last two keyframes (if there are atleast two)
    if (keyframes.size() > 1)
    {
        float val = dot(keyframes.at(keyframes.size() - 1).getRot(), keyframes.at(keyframes.size() - 2).getRot());
        if (val < 0)
        {
            keyframes.at(keyframes.size() - 1).setRot(-keyframes.at(keyframes.size() - 1).getRot());
        }
    }
}

void Catmull::drawSpline()
{
    glPushMatrix();
    mat4 G;

    if (keyframes.size() < 4)
    {
        return;
    }

    for (int i = 0; i < keyframes.size() - 3; i++)
    {
        G[0] = vec4(keyframes.at(i).getPos(), 0.0f);
        G[1] = vec4(keyframes.at(i + 1).getPos(), 0.0f);
        G[2] = vec4(keyframes.at(i + 2).getPos(), 0.0f);
        G[3] = vec4(keyframes.at(i + 3).getPos(), 0.0f);

        glLineWidth(1.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_STRIP);

        for (float u = 0; u <= 1; u += .01)
        {
            vec4 uVec(1.0f, u, u * u, u * u * u);
            vec4 p = G * (B * uVec);

            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }
    glPopMatrix();
}

vec3 Catmull::getInterpolatedPosition(int uMax, float u)
{
    mat4 G;

    if (keyframes.size() < 4)
    {
        return vec3(0, 0, 0);
    }

    // Truncate u to find first control point.
    int ci = (int)u;

    float uNorm = u - ci;

    vec4 uVec = vec4(1.0f, uNorm, uNorm * uNorm, uNorm * uNorm * uNorm);

    G[0] = vec4(keyframes.at(ci).getPos(), 0.0f);
    G[1] = vec4(keyframes.at(ci + 1).getPos(), 0.0f);
    G[2] = vec4(keyframes.at(ci + 2).getPos(), 0.0f);
    G[3] = vec4(keyframes.at(ci + 3).getPos(), 0.0f);

    vec4 p = G * (B * uVec);

    return vec3(p.x, p.y, p.z);
}

quat Catmull::getInterpolatedRotation(int uMax, float u)
{
    mat4 G;

    if (keyframes.size() < 4)
    {
        return quat();
    }
    int ci = (int)u;

    float uNorm = u - ci;

    vec4 uVec = vec4(1.0f, uNorm, uNorm * uNorm, uNorm * uNorm * uNorm);

    G[0] = vec4(keyframes.at(ci).getRot().x, keyframes.at(ci).getRot().y, keyframes.at(ci).getRot().z, keyframes.at(ci).getRot().w);
    G[1] = vec4(keyframes.at(ci + 1).getRot().x, keyframes.at(ci + 1).getRot().y, keyframes.at(ci + 1).getRot().z, keyframes.at(ci + 1).getRot().w);
    G[2] = vec4(keyframes.at(ci + 2).getRot().x, keyframes.at(ci + 2).getRot().y, keyframes.at(ci + 2).getRot().z, keyframes.at(ci + 2).getRot().w);
    G[3] = vec4(keyframes.at(ci + 3).getRot().x, keyframes.at(ci + 3).getRot().y, keyframes.at(ci + 3).getRot().z, keyframes.at(ci + 3).getRot().w);

    vec4 qVec = G * (B * uVec);
    return quat(qVec[3], qVec[0], qVec[1], qVec[2]);
}

void Catmull::buildALTable()
{
    ALTable.clear();
    ALTable.push_back(pair<float, float>(0, 0));

    for (int i = 0; i < (int)keyframes.size() - 3; i++)
    {
        mat4 G = mat4();
        G[0] = vec4(keyframes.at(i).getPos(), 0.0f);
        G[1] = vec4(keyframes.at(i + 1).getPos(), 0.0f);
        G[2] = vec4(keyframes.at(i + 2).getPos(), 0.0f);
        G[3] = vec4(keyframes.at(i + 3).getPos(), 0.0f);

        float uA = 0.0;

        for (float u = 0.2f; u <= 1; u += .2f)
        {
            vec4 uVecB = vec4(1.0, u, u * u, u * u * u);
            vec4 uVecA = vec4(1.0, uA, uA * uA, uA * uA * uA);
            vec4 b = (G * B * uVecB);
            vec4 a = (G * B * uVecA);
            if (ALTable.empty())
                ALTable.push_back(pair<float, float>(u + i, length(b - a)));
            else
                ALTable.push_back(pair<float, float>(u + i, ALTable.back().second + length(b - a)));
            uA = u;
        }
    }
}

vector<pair<float, float>> Catmull::getALTable()
{
    return ALTable;
}

float Catmull::s2u(float s)
{
    for (int i = 0; i < ((int)ALTable.size()) - 1; i++)
    {
        if (ALTable.at(i).second < s && ALTable.at(i + 1).second > s)
        {
            float alpha = (s - ALTable.at(i).second) / (ALTable.at(i + 1).second - ALTable.at(i).second);
            return (1 - alpha) * ALTable.at(i).first + alpha * ALTable.at(i + 1).first;
        }
    }
    return 0;
}
