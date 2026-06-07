/*
 * Colisiones.h
 *
 *  Created on: Dec 18, 2019
 *      Author: rey
 */

#ifndef COLISIONES_H_
#define COLISIONES_H_

#include <map>
#include <tuple>
#include <string>
#include <cmath>

#include "AbstractModel.h"

inline void addOrUpdateColliders(
	std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >& colliders,
	std::string name,
	AbstractModel::OBB collider,
	glm::mat4 transform
) {
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
		colliders.find(name);

	if (it != colliders.end()) {
		std::get<0>(it->second) = collider;
		std::get<2>(it->second) = transform;
	}
	else {
		colliders[name] = std::make_tuple(collider, glm::mat4(1.0), transform);
	}
}

inline void addOrUpdateColliders(
	std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >& colliders,
	std::string name
) {
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
		colliders.find(name);

	if (it != colliders.end()) {
		std::get<1>(it->second) = std::get<2>(it->second);
	}
}

inline void addOrUpdateColliders(
	std::map<std::string,
	std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >& colliders,
	std::string name,
	AbstractModel::SBB collider,
	glm::mat4 transform
) {
	std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
		colliders.find(name);

	if (it != colliders.end()) {
		std::get<0>(it->second) = collider;
		std::get<2>(it->second) = transform;
	}
	else {
		colliders[name] = std::make_tuple(collider, glm::mat4(1.0), transform);
	}
}

inline void addOrUpdateColliders(
	std::map<std::string,
	std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >& colliders,
	std::string name
) {
	std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
		colliders.find(name);

	if (it != colliders.end()) {
		std::get<1>(it->second) = std::get<2>(it->second);
	}
}

inline void addOrUpdateCollisionDetection(
	std::map<std::string, bool>& collisionDetector,
	std::string name,
	bool isCollision
) {
	std::map<std::string, bool>::iterator colIt = collisionDetector.find(name);

	if (colIt != collisionDetector.end()) {
		if (!colIt->second) {
			colIt->second = isCollision;
		}
	}
	else {
		collisionDetector[name] = isCollision;
	}
}

inline bool raySphereIntersect(
	glm::vec3 orig,
	glm::vec3 dest,
	glm::vec3 dir,
	AbstractModel::SBB sbb,
	float& t
) {
	glm::vec3 vDirToSphere = sbb.c - orig;

	float fLineLength = glm::distance(orig, dest);

	t = glm::dot(vDirToSphere, dir);

	glm::vec3 vClosestPoint;

	if (t <= 0.0f) {
		vClosestPoint = orig;
	}
	else if (t >= fLineLength) {
		vClosestPoint = dest;
	}
	else {
		vClosestPoint = orig + dir * t;
	}

	return glm::distance(sbb.c, vClosestPoint) <= sbb.ratio;
}

inline bool testSphereSphereIntersection(
	AbstractModel::SBB sbb1,
	AbstractModel::SBB sbb2
) {
	float d = glm::distance(sbb1.c, sbb2.c);

	if (d <= (sbb1.ratio + sbb2.ratio)) {
		return true;
	}

	return false;
}

inline bool testSphereOBox(
	AbstractModel::SBB sbb,
	AbstractModel::OBB obb
) {
	float d = 0.0f;

	glm::quat qinv = glm::inverse(obb.u);

	sbb.c = qinv * glm::vec4(sbb.c, 1.0f);
	obb.c = qinv * glm::vec4(obb.c, 1.0f);

	AbstractModel::AABB aabb;
	aabb.mins = obb.c - obb.e;
	aabb.maxs = obb.c + obb.e;

	if (
		sbb.c[0] >= aabb.mins[0] && sbb.c[0] <= aabb.maxs[0] &&
		sbb.c[1] >= aabb.mins[1] && sbb.c[1] <= aabb.maxs[1] &&
		sbb.c[2] >= aabb.mins[2] && sbb.c[2] <= aabb.maxs[2]
		) {
		return true;
	}

	for (int i = 0; i < 3; i++) {
		if (sbb.c[i] < aabb.mins[i]) {
			d += (sbb.c[i] - aabb.mins[i]) * (sbb.c[i] - aabb.mins[i]);
		}
		else if (sbb.c[i] > aabb.maxs[i]) {
			d += (sbb.c[i] - aabb.maxs[i]) * (sbb.c[i] - aabb.maxs[i]);
		}
	}

	if (d <= sbb.ratio * sbb.ratio) {
		return true;
	}

	return false;
}

inline bool testOBBOBB(
	AbstractModel::OBB a,
	AbstractModel::OBB b
) {
	float EPSILON = 0.0001f;
	float ra;
	float rb;

	glm::mat3 R = glm::mat3(0.0f);
	glm::mat3 AbsR = glm::mat3(0.0f);

	glm::mat3 matA = glm::mat3(a.u);
	glm::mat3 matB = glm::mat3(b.u);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			R[i][j] = glm::dot(matA[i], matB[j]);
		}
	}

	glm::vec3 t = b.c - a.c;

	t = glm::vec3(
		glm::dot(t, matA[0]),
		glm::dot(t, matA[1]),
		glm::dot(t, matA[2])
	);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			AbsR[i][j] = fabs(R[i][j]) + EPSILON;
		}
	}

	for (int i = 0; i < 3; i++) {
		ra = a.e[i];
		rb = b.e[0] * AbsR[i][0] + b.e[1] * AbsR[i][1] + b.e[2] * AbsR[i][2];

		if (fabs(t[i]) > ra + rb) {
			return false;
		}
	}

	for (int i = 0; i < 3; i++) {
		ra = a.e[0] * AbsR[0][i] + a.e[1] * AbsR[1][i] + a.e[2] * AbsR[2][i];
		rb = b.e[i];

		if (fabs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) {
			return false;
		}
	}

	ra = a.e[1] * AbsR[2][0] + a.e[2] * AbsR[1][0];
	rb = b.e[1] * AbsR[0][2] + b.e[2] * AbsR[0][1];

	if (fabs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) {
		return false;
	}

	ra = a.e[1] * AbsR[2][1] + a.e[2] * AbsR[1][1];
	rb = b.e[0] * AbsR[0][2] + b.e[2] * AbsR[0][0];

	if (fabs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) {
		return false;
	}

	ra = a.e[1] * AbsR[2][2] + a.e[2] * AbsR[1][2];
	rb = b.e[0] * AbsR[0][1] + b.e[1] * AbsR[0][0];

	if (fabs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) {
		return false;
	}

	ra = a.e[0] * AbsR[2][0] + a.e[2] * AbsR[0][0];
	rb = b.e[1] * AbsR[1][2] + b.e[2] * AbsR[1][1];

	if (fabs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) {
		return false;
	}

	ra = a.e[0] * AbsR[2][1] + a.e[2] * AbsR[0][1];
	rb = b.e[0] * AbsR[1][2] + b.e[2] * AbsR[1][0];

	if (fabs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) {
		return false;
	}

	ra = a.e[0] * AbsR[2][2] + a.e[2] * AbsR[0][2];
	rb = b.e[0] * AbsR[1][1] + b.e[1] * AbsR[1][0];

	if (fabs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) {
		return false;
	}

	ra = a.e[0] * AbsR[1][0] + a.e[1] * AbsR[0][0];
	rb = b.e[1] * AbsR[2][2] + b.e[2] * AbsR[2][1];

	if (fabs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) {
		return false;
	}

	ra = a.e[0] * AbsR[1][1] + a.e[1] * AbsR[0][1];
	rb = b.e[0] * AbsR[2][2] + b.e[2] * AbsR[2][0];

	if (fabs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) {
		return false;
	}

	ra = a.e[0] * AbsR[1][2] + a.e[1] * AbsR[0][2];
	rb = b.e[0] * AbsR[2][1] + b.e[1] * AbsR[2][0];

	if (fabs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) {
		return false;
	}

	return true;
}

#endif /* COLISIONES_H_ */