#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <vector>


glm::vec2 Lerp(const glm::vec2 p1,const glm::vec2 p2,const float t) {
    glm::vec2 ans;
    ans.x = p1.x + (p2.x-p1.x)*t;
    ans.y = p1.y+(ans.x-p1.x)*((p2.y-p1.y)/(p2.x-p1.x));
    return ans;
}
class BezierCurve {
private:
    glm::vec2 calcLinePoint(const float t) {
        if(points.size()<2) {
            return {0,0};
        }
        std::vector<glm::vec2> tmp_points(points);
        while(tmp_points.size()>2) {
            for(int i=0;i<tmp_points.size()-1;i++) {
                tmp_points[i] = Lerp(tmp_points[i],tmp_points[i+1],t);
            }
            tmp_points.pop_back();
        }
        return Lerp(tmp_points[0],tmp_points[1],t);
    }
    float precision = 0.01f;
public:
	std::vector<glm::vec2> points;
	std::vector<glm::vec2> linePoints;
    BezierCurve() {
        linePoints.clear();
        linePoints.resize(1.0f/precision+1);
    }
    void RecalculateLine() {
        float t = 0;
        for(int i=0;i<linePoints.size();i++) {
            linePoints[i] = calcLinePoint(t);
            t+=precision;
        }
    }
    void SetPrecision(const float p){
        precision = p;
        linePoints.clear();
        linePoints.resize(1.0f/precision+1);
    }
    float GetPrecision() const {
        return precision;
    }
};