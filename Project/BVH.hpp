#pragma once

#include "AABB.h"
#include "GeometryNode.hpp"
#include <vector>
#include "SceneNode.hpp"

using namespace std;
using namespace glm;

struct BVHNode
{
    AABB aabb;
    vector<GeometryNode*> objects;
    bool simple_primitive = true;

    BVHNode* l = nullptr;
    BVHNode* r = nullptr;

    BVHNode() = default;
    BVHNode( BVHNode* l, BVHNode* r): l{l}, r{r} {};
};


class BVH
{
    BVHNode* tree = nullptr;

    void get_gnodes(SceneNode* root, vector<GeometryNode*> &result) const;
    BVHNode* construct_tree(vector<pair<AABB, GeometryNode*>> boxes);
    void collide_subtree(vec3 eye, vec3 ray, BVHNode* root, vector<GeometryNode*> &result) const;

public:
    // Ensure that root's world transforms have been calculated already!
    BVH(SceneNode* root);
    vector<GeometryNode*> candidate_geometry_nodes(vec3 eye, vec3 ray ) const;

};